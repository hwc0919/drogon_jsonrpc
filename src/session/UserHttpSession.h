#pragma once
#include "UserSession.h"
#include <drogon/HttpClient.h>
#include <drogon/HttpResponse.h>

class UserHttpSession : public UserSession
{
public:
    UserHttpSession(drogon::HttpRequestPtr req, std::function<void(drogon::HttpResponsePtr)> callback);
    uint64_t queueRequest() override;
    void queueResponse(uint64_t seq, std::string msg) override;
    void flush() override;

private:
    drogon::HttpRequestPtr req_;
    std::function<void(drogon::HttpResponsePtr)> callback_;
};
