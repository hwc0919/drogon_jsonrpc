//
// Created by wanchen.he on 2023/3/7.
//
#include "JsonrpcServiceBase.h"
#include <trantor/utils/Logger.h>

using namespace drogon;

static std::string jsonrpcErrorResponse(JsonrpcErrorCode code, const nlohmann::json & id = nullptr)
{
    return (nlohmann::json{
                { "jsonrpc", "2.0" },
                { "id", id },
                { "error",
                  {
                      { "code", code },
                      { "message", jsonRpcErrorMessage.at(code) },
                  } },
            })
        .dump();
}

static std::string jsonrpcCustomErrorResponse(int code, const nlohmann::json & id, const std::string & message)
{
    return (nlohmann::json{
                { "jsonrpc", "2.0" },
                { "id", id },
                { "error",
                  {
                      { "code", code },
                      { "message", message },
                  } },
            })
        .dump();
}

void JsonrpcServiceBase::handleJsonrpc(const std::shared_ptr<UserSession> & sess, const std::string & message)
{
    uint64_t seq = sess->queueRequest();
    nlohmann::json reqJson;
    try
    {
        reqJson = nlohmann::json::parse(message);
    }
    catch (const std::exception & ex)
    {
        LOG_ERROR << "Failed to parse request body as json: " << ex.what();
        sess->queueResponse(seq, jsonrpcErrorResponse(kRpcParseError));
        sess->flush();
        return;
    }
    if (!reqJson.is_object())
    {
        // TODO: support batch
        sess->queueResponse(seq, jsonrpcErrorResponse(kRpcInvalidRequest));
        sess->flush();
        return;
    }

    if (!reqJson.contains("id") || reqJson["id"].is_null())
    {
        // Do not support push now
        sess->queueResponse(seq, jsonrpcErrorResponse(kRpcInvalidRequest));
        sess->flush();
        return;
    }
    nlohmann::json reqId = reqJson["id"].get<std::string>();

    // Check protocol tag
    if (!reqJson.contains("jsonrpc") || !reqJson["jsonrpc"].is_string() || reqJson["jsonrpc"].get<std::string>() != "2.0")
    {
        // Do not support push now
        sess->queueResponse(seq, jsonrpcErrorResponse(kRpcInvalidRequest, reqId));
        sess->flush();
        return;
    }

    // Check method
    if (!reqJson.contains("method") || !reqJson["method"].is_string())
    {
        // Do not support push now
        sess->queueResponse(seq, jsonrpcErrorResponse(kRpcInvalidRequest, reqId));
        sess->flush();
        return;
    }
    std::string method = reqJson["method"].get<std::string>();

    // Check params
    const nlohmann::json & params = reqJson["params"];
    if (!params.is_array() && !params.is_object())
    {
        sess->queueResponse(seq, jsonrpcErrorResponse(kRpcInvalidRequest, reqId));
        sess->flush();
        return;
    }

    // Invoke service
    auto factory = ServiceFactory::getFactoryByName(method);
    if (!factory)
    {
        sess->queueResponse(seq, jsonrpcErrorResponse(kRpcMethodNotFound, reqId));
        sess->flush();
        return;
    }
    auto service = (*factory)();

    drogon::async_run([=]() -> Task<> {
        try
        {
            nlohmann::json result = co_await service->handleRequest(sess, reqJson["params"]);
            LOG_INFO << "Request: " << reqJson.dump() << ", result: " << result.dump();
            nlohmann::json respJson{
                { "id", reqId },
                { "jsonrpc", "2.0" },
                { "result", std::move(result) }
            };
            sess->queueResponse(seq, respJson.dump());
            sess->flush();
        }
        catch (const JsonRpcException & ex)
        {
            LOG_ERROR << "Error handling request: " << ex.what();
            sess->queueResponse(seq, jsonrpcCustomErrorResponse(ex.code(), reqId, ex.what()));
            sess->flush();
        }
        catch (const std::exception & ex)
        {
            LOG_ERROR << "Error handling request: " << ex.what();
            sess->queueResponse(seq, jsonrpcErrorResponse(kRpcInternalError, reqId));
            sess->flush();
        } });
}
