#pragma once
#include <deque>
#include <drogon/WebSocketConnection.h>
#include <memory>
#include <optional>
#include <vector>

class UserSession
{
public:
    UserSession();
    void setRealAddr(const trantor::InetAddress & addr);
    const trantor::InetAddress & realAddr() const;
    virtual uint64_t queueRequest() = 0;
    virtual void queueResponse(uint64_t seq, std::string msg) = 0;
    virtual void flush() = 0;

protected:
    trantor::InetAddress realAddr_;
};
