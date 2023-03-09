//
// Created by wanchen.he on 2023/3/4.
//
#include "JsonrpcServiceBase.h"

using namespace drogon;

class HelloWorldService : public JsonrpcServiceBase, public ServiceRegister<HelloWorldService>
{
public:
    static constexpr const char * JSON_RPC_METHOD = "hello_world";

    Task<nlohmann::json> handleRequest(const std::shared_ptr<UserSession> & sess, const nlohmann::json & params) override;
};

Task<nlohmann::json> HelloWorldService::handleRequest(const std::shared_ptr<UserSession> & sess, const nlohmann::json & params)
{
    co_return {
            {"status", "ok"},
            {"message", "hello world"},
    };
}
