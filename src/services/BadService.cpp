//
// Created by wanchen.he on 2023/3/4.
//
#include "JsonrpcServiceBase.h"

using namespace drogon;

class BadService : public JsonrpcServiceBase, public ServiceRegister<BadService>
{
public:
    static constexpr const char * JSON_RPC_METHOD = "bad";

    Task<nlohmann::json> handleRequest(const std::shared_ptr<UserSession> & sess, const nlohmann::json & params) override;
};

Task<nlohmann::json> BadService::handleRequest(const std::shared_ptr<UserSession> & sess, const nlohmann::json & params)
{
    throw JsonRpcException(-1000, "This is bad!");
}
