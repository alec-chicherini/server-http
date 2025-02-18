cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

#CMake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(
    -Werror
    -Wall
)
project(server-http)

message(INFO "CMAKE_CXX_COMPILE_FEATURES = ${CMAKE_CXX_COMPILE_FEATURES}")
message(INFO "CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}")

find_package(userver REQUIRED COMPONENTS core)

# The Service
set(SOURCE_FILES__ main.cpp http_handler_subdomain_static.cpp http_handler_subdomain_static.hpp )
list(TRANSFORM SOURCE_FILES__ PREPEND "src/")
add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES__})
target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_20)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} userver::core)

# Unit Tests
#add_executable(${PROJECT_NAME}_unittest
#    src/hello_test.cpp
#)
#target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_objs userver::utest)
#add_google_tests(${PROJECT_NAME}_unittest)

# Benchmarks
#add_executable(${PROJECT_NAME}_benchmark
#    src/hello_benchmark.cpp
#)
#target_link_libraries(${PROJECT_NAME}_benchmark PRIVATE ${PROJECT_NAME}_objs userver::ubench)
#add_google_benchmark_tests(${PROJECT_NAME}_benchmark)


#CPack
set(CMAKE_INSTALL_PREFIX /opt/${CMAKE_PROJECT_NAME})

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}
        DESTINATION bin
        PERMISSIONS WORLD_EXECUTE OWNER_WRITE OWNER_READ)

install(FILES configs/static_config.yaml
        DESTINATION /etc/${CMAKE_PROJECT_NAME}/)

install(FILES configs/config_vars.yaml
        DESTINATION /etc/${CMAKE_PROJECT_NAME}/)
execute_process(COMMAND mkdir ${CMAKE_CURRENT_BINARY_DIR}/symlink)
execute_process(COMMAND ln -s /opt/${CMAKE_PROJECT_NAME}/bin/${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/symlink/${CMAKE_PROJECT_NAME})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/symlink/server-http
        DESTINATION /usr/bin/
        PERMISSIONS WORLD_EXECUTE WORLD_READ)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_CONTACT "chicherin.alexey@ya.ru")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/alec-chicherini/wordle-task")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
set(CPACK_DEBIAN_PACKAGE_VERSION 25.02)
set(CPACK_DEBIAN_FILE_NAME ${CPACK_DEBIAN_PACKAGE_NAME}_${CPACK_DEBIAN_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb)
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)

#https://raw.githubusercontent.com/userver-framework/userver/refs/heads/develop/scripts/docs/en/deps/ubuntu-24.04.md
#set(CPACK_DEBIAN_PACKAGE_DEPENDS libqt6widgets6|libqt6widgets6t64 libqt6gui6|libqt6gui6t64 libqt6core6|libqt6core6t64 )

include(CPack)