#include "JsonrpcServiceBase.h"
#include "session/UserHttpSession.h"
#include <drogon/HttpController.h>
#include <drogon/plugins/RealIpResolver.h>

using namespace drogon;

class HttpCtrl : public drogon::HttpController<HttpCtrl>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(HttpCtrl::jsonrpc, "/api/jsonrpc", Post);
    ADD_METHOD_TO(HttpCtrl::jsonrpc, "/jsonrpc", Post);
    METHOD_LIST_END

    void jsonrpc(const HttpRequestPtr & req, std::function<void(const HttpResponsePtr &)> && callback) const;
};

void HttpCtrl::jsonrpc(const HttpRequestPtr & req, std::function<void(const HttpResponsePtr &)> && callback) const
{
    // Create and save user data context
    std::shared_ptr<UserSession> sess = std::make_shared<UserHttpSession>(req, std::move(callback));
    auto realAddr = plugin::RealIpResolver::GetRealAddr(req);
    sess->setRealAddr(realAddr);
    LOG_INFO << "New http request from: " << realAddr.toIp();

    JsonrpcServiceBase::handleJsonrpc(sess, std::string{req->getBody()});
}
