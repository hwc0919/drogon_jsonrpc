#include "UserSession.h"

using namespace drogon;

UserSession::UserSession() = default;

void UserSession::setRealAddr(const trantor::InetAddress & addr)
{
    realAddr_ = addr;
}
const trantor::InetAddress & UserSession::realAddr() const
{
    return realAddr_;
}
