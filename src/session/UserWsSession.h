#pragma once

#include "UserSession.h"

class UserWsSession : public UserSession
{
public:
    explicit UserWsSession(const std::shared_ptr<drogon::WebSocketConnection> & wsConn);
    uint64_t queueRequest() override;
    void queueResponse(uint64_t seq, std::string msg) override;
    void flush() override;

private:
    std::atomic_uint64_t seq_{ 0 };
    std::weak_ptr<drogon::WebSocketConnection> weakConn_;
    std::deque<std::pair<uint64_t, std::optional<std::string>>> respQueue_;
};
