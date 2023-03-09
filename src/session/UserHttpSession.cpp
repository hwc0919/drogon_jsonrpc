#include "UserHttpSession.h"

using namespace drogon;

UserHttpSession::UserHttpSession(HttpRequestPtr req, std::function<void(HttpResponsePtr)> callback)
    : req_(std::move(req)), callback_(std::move(callback))
{
}

uint64_t UserHttpSession::queueRequest()
{
    return 0;
}

void UserHttpSession::queueResponse(uint64_t, std::string msg)
{
    auto resp = HttpResponse::newHttpResponse();
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    resp->setBody(std::move(msg));
    callback_(resp);
}

void UserHttpSession::flush()
{
}
