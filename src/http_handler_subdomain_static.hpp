#pragma once
#include <userver/utest/using_namespace_userver.hpp>
#include <userver/components/component_list.hpp>
#include <userver/components/fs_cache.hpp>
#include <userver/dynamic_config/source.hpp>
#include <userver/fs/fs_cache_client.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <map>

namespace server_http {
    class HttpHandlerSubdomainStatic final : public server::handlers::HttpHandlerBase {
    public:
        static constexpr std::string_view kName = "handler-subdomain-static";

        using server::handlers::HttpHandlerBase::HttpHandlerBase;

        HttpHandlerSubdomainStatic(const components::ComponentConfig& config, const components::ComponentContext& context);

        std::string HandleRequestThrow(const server::http::HttpRequest& request, server::request::RequestContext&) const override;

        static yaml_config::Schema GetStaticConfigSchema();

    private:
        dynamic_config::Source config_;
        const std::chrono::seconds cache_age_;
        std::map<std::string, const fs::FsCacheClient*> storages_;
    };
}