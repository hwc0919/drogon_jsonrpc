#include "UserWsSession.h"
#include <trantor/utils/Logger.h>

using namespace drogon;

UserWsSession::UserWsSession(const std::shared_ptr<WebSocketConnection> & wsConn)
    : weakConn_(wsConn)
{
}

uint64_t UserWsSession::queueRequest()
{
    uint64_t seq = ++seq_;
    respQueue_.emplace_back(seq, std::nullopt);
    return seq;
}

void UserWsSession::queueResponse(uint64_t seq, std::string msg)
{
    for (auto & item : respQueue_)
    {
        if (item.first == seq)
        {
            item.second.emplace(std::move(msg));
            return;
        }
    }
}

void UserWsSession::flush()
{
    auto connPtr = weakConn_.lock();
    if (!connPtr)
    {
        LOG_ERROR << "Calling flush() on closed connection.";
        return;
    }
    while (!respQueue_.empty() && respQueue_.front().second.has_value())
    {
        auto resp = std::move(respQueue_.front().second);
        respQueue_.pop_front();
        connPtr->send(resp.value());
    }
}
