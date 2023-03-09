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
        {kRpcParseError, "Parse error"},
        {kRpcInvalidRequest, "Invalid request"},
        {kRpcMethodNotFound, "Method not found"},
        {kRpcInvalidParams, "Invalid params"},
        {kRpcInternalError, "Internal error"},
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
};

/**
 * @class ServiceFactory
 * Used to register and create service instance by name.
 */
struct ServiceFactory
{
public:
    virtual std::shared_ptr<JsonrpcServiceBase> operator()() = 0;

    static std::shared_ptr<ServiceFactory> getFactoryByName(const std::string & name)
    {
        auto iter = factories().find(name);
        if (iter == factories().end())
        {
            return nullptr;
        }
        return iter->second;
    }
    // Only call this method during static initialization period, to save the mutex
    template <typename T>
    static bool registerFactory(std::string name)
    {
        return factories().emplace(name, getFactoryInstance<T>()).second;
    }
    static const std::vector<std::string> & names()
    {
        return names_();
    }

private:
    static std::unordered_map<std::string, std::shared_ptr<ServiceFactory>> & factories()
    {
        static std::unordered_map<std::string, std::shared_ptr<ServiceFactory>> factories;
        return factories;
    }
    static std::vector<std::string> & names_()
    {
        static std::vector<std::string> names;
        return names;
    }
    template <typename T>
    static const std::shared_ptr<ServiceFactory> & getFactoryInstance();
};

template <typename T>
struct ServiceFactoryT : public ServiceFactory
{
    std::shared_ptr<JsonrpcServiceBase> operator()() override
    {
        return std::make_shared<T>();
    }
};

template <typename T>
const std::shared_ptr<ServiceFactory> & ServiceFactory::getFactoryInstance()
{
    static std::shared_ptr<ServiceFactory> factory = std::make_shared<ServiceFactoryT<T>>();
    return factory;
}

template <typename T>
struct ServiceTraits<T, typename std::enable_if<std::is_base_of<JsonrpcServiceBase, T>::value, T>::type>
{
    static void registerClass()
    {
        ServiceFactory::registerFactory<T>(T::JSON_RPC_METHOD);
    }
};
