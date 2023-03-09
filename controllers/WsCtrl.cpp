#include "JsonrpcServiceBase.h"
#include "session/UserWsSession.h"
#include <drogon/WebSocketController.h>
#include <drogon/plugins/RealIpResolver.h>
#include <drogon/utils/coroutine.h>

using namespace drogon;

class WsCtrl : public WebSocketController<WsCtrl>
{
public:
    void handleNewMessage(const WebSocketConnectionPtr &,
                          std::string &&,
                          const WebSocketMessageType &) override;
    void handleNewConnection(const HttpRequestPtr &,
                             const WebSocketConnectionPtr &) override;
    void handleConnectionClosed(const WebSocketConnectionPtr &) override;
    WS_PATH_LIST_BEGIN
    // list path definitions here;
    WS_PATH_ADD("/ws/jsonrpc");
    WS_PATH_ADD("/jsonrpc");
    WS_PATH_LIST_END
};

void WsCtrl::handleNewMessage(const WebSocketConnectionPtr & wsConn, std::string && message, const WebSocketMessageType & msgType)
{
    if (msgType == WebSocketMessageType::Ping || msgType == WebSocketMessageType::Pong)
    {
        return;
    }

    auto sess = wsConn->getContext<UserSession>();
    if (!sess)
    {
        LOG_ERROR << "Invalid websocket connection, no session.";
        wsConn->forceClose();
        return;
    }

    JsonrpcServiceBase::handleJsonrpc(sess, message);
}

void WsCtrl::handleNewConnection(const HttpRequestPtr & req, const WebSocketConnectionPtr & wsConn)
{
    // Create and save user data context
    std::shared_ptr<UserSession> sess = std::make_shared<UserWsSession>(wsConn);
    auto realAddr = plugin::RealIpResolver::GetRealAddr(req);
    sess->setRealAddr(realAddr);
    LOG_INFO << "New websocket connection from: " << realAddr.toIp();
    wsConn->setContext(sess);
}

void WsCtrl::handleConnectionClosed(const WebSocketConnectionPtr & wsConn)
{
    auto sess = wsConn->getContext<UserSession>();
    if (!sess)
    {
        return;
    }
    LOG_INFO << "Close websocket connection from: " << sess->realAddr().toIp();
    wsConn->clearContext();
}
