#pragma once
#include "ServiceRegister.h"
#include "session/UserSession.h"
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/utils/coroutine.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <stdexcept>

enum JsonrpcErrorCode
{
    kRpcParseError = -32700,
    kRpcInvalidRequest = -32600,
    kRpcMethodNotFound = -32601,
    kRpcInvalidParams = -32602,
    kRpcInternalError = -32603
};

static const std::unordered_map<JsonrpcErrorCode, std::string> jsonRpcErrorMessage{
    { kRpcParseError, "Parse error" },
    { kRpcInvalidRequest, "Invalid request" },
    { kRpcMethodNotFound, "Method not found" },
    { kRpcInvalidParams, "Invalid params" },
    { kRpcInternalError, "Internal error" },
};

class JsonRpcException : public std::runtime_error
{
public:
    JsonRpcException(int code, const std::string & message)
        : std::runtime_error(message), code_(code)
    {
    }
    JsonRpcException(JsonrpcErrorCode code, const std::string & message)
        : std::runtime_error(message), code_(code)
    {
    }
    int code() const
    {
        return code_;
    }

private:
    int code_;
};

/**
 * @class JsonrpcServiceBase
 */
class JsonrpcServiceBase
{
public:
    static void handleJsonrpc(const std::shared_ptr<UserSession> & sess, const std::string & message);

    virtual ~JsonrpcServiceBase() = default;
    virtual drogon::Task<nlohmann::json> handleRequest(const std::shared_ptr<UserSession> & sess, const nlohmann::json & params) = 0;

    static std::shared_ptr<JsonrpcServiceBase> getInstanceByName(const std::string & name)
    {
        auto iter = instances().find(name);
        if (iter == instances().end())
        {
            return nullptr;
        }
        return iter->second;
    }

private:
    template <typename T, typename U>
    friend struct ServiceTraits;
    static std::unordered_map<std::string, std::shared_ptr<JsonrpcServiceBase>> & instances()
    {
        static std::unordered_map<std::string, std::shared_ptr<JsonrpcServiceBase>> m;
        return m;
    }
};

template <typename T>
struct ServiceTraits<T, typename std::enable_if<std::is_base_of<JsonrpcServiceBase, T>::value, T>::type>
{
    static void registerClass()
    {
        auto servicePtr = std::make_shared<T>();
        JsonrpcServiceBase::instances().emplace(T::JSON_RPC_METHOD, servicePtr);
    }
};
