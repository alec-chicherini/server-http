#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/dynamic_config/storage/component.hpp>
#include <userver/dynamic_config/value.hpp>
#include <userver/http/common_headers.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

#include <http_handler_subdomain_static.hpp>

namespace server_http {

    namespace {

        const dynamic_config::Key<dynamic_config::ValueDict<std::string>> kContentTypeMap{
            "USERVER_FILES_CONTENT_TYPE_MAP",
            dynamic_config::DefaultAsJsonString{
                R"(
{
  ".css": "text/css",
  ".gif": "image/gif",
  ".htm": "text/html",
  ".html": "text/html",
  ".jpeg": "image/jpeg",
  ".js": "application/javascript",
  ".json": "application/json",
  ".md": "text/markdown",
  ".png": "image/png",
  ".svg": "image/svg+xml",
  ".wasm": "application/wasm",
  "__default__": "text/plain"
}
)"},
        };

    } // namespace

    HttpHandlerSubdomainStatic::HttpHandlerSubdomainStatic(
        const components::ComponentConfig& config,
        const components::ComponentContext& context
    )
        : HttpHandlerBase(config, context),
        config_(context.FindComponent<components::DynamicConfig>().GetSource()),
        cache_age_(config["expires"].As<std::chrono::seconds>(600))
        {
            for (const auto& storage_name : config["fs-cache-storages"]) 
            {
                storages_.insert({ storage_name.As<std::string>(),
                                   &(context.FindComponent<components::FsCache>(storage_name.As<std::string>()).GetClient())});
            }
        }

    std::string HttpHandlerSubdomainStatic::HandleRequestThrow(const server::http::HttpRequest& request, server::request::RequestContext&) const {
        auto get_host = [&] 
            {
                using namespace std::literals;
                std::string result;
                result = request.GetHost();
                if (result.starts_with("www."sv)){
                    result.erase(0, 4);
                }
                size_t begin_of_port = result.find_last_of(":");
                if (begin_of_port != std::string::npos){
                    result.erase(begin_of_port);
                }
                return result;
            };
        const auto host = get_host();
        const auto path = request.GetRequestPath();
        const auto file = storages_.at(host)->TryGetFile(path);

        auto& response = request.GetHttpResponse();
        if (file) {
            const auto config = config_.GetSnapshot();
            response.SetHeader(USERVER_NAMESPACE::http::headers::kExpires, std::to_string(cache_age_.count()));
            response.SetContentType(config[kContentTypeMap][file->extension]);
            return file->data;
        }
        response.SetStatusNotFound();
        return "File not found";
    }

    yaml_config::Schema HttpHandlerSubdomainStatic::GetStaticConfigSchema() {
        return yaml_config::MergeSchemas<HttpHandlerBase>(R"(
type: object
description: |
    Handler that returns HTTP 200 if file exist
    and returns file data with mapped content/type for specific host name or subdomain
additionalProperties: false
properties:
    fs-cache-storages:
        type: array
        items:
            type: string
            description: Name of FsCache component
        description: List of FsCache components
        defaultDescription: fs-cache-storages
    expires:
        type: string
        description: Cache age in seconds
        defaultDescription: 600
)");
    }

}  // namespace server_http
