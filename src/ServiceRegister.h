#pragma once

#include <memory>
#include <string>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif

template <typename T, typename D = T>
struct ServiceTraits;

class ServiceRegisterBase
{
public:
    virtual ~ServiceRegisterBase() = default;

    virtual const std::string & className() const
    {
        static const std::string name{ "ServiceRegisterBase" };
        return name;
    }
    static std::string demangle(const char * mangled_name)
    {
#ifndef _MSC_VER
        std::size_t len = 0;
        int status = 0;
        std::unique_ptr<char, decltype(&std::free)> ptr(
            __cxxabiv1::__cxa_demangle(mangled_name, nullptr, &len, &status),
            &std::free);
        if (status != 0)
        {
            throw std::runtime_error("Demangle error");
        }
        return { ptr.get() };
#else
        auto pos = strstr(mangled_name, " ");
        if (pos == nullptr)
            return std::string{ mangled_name };
        else
            return std::string{ pos + 1 };
#endif
    }
};

template <typename T>
class ServiceRegister : public virtual ServiceRegisterBase
{
public:
    const std::string & className() const override
    {
        return alloc_.className();
    }
    static const std::string & ClassName()
    {
        return alloc_.className();
    }

protected:
    // protect constructor to make this class only inheritable
    ServiceRegister() = default;
    ~ServiceRegister() override = default;

private:
    class Allocator
    {
    public:
        Allocator()
        {
            registerClass();
        }
        const std::string & className() const
        {
            static std::string className = demangle(typeid(T).name());
            return className;
        }
        void registerClass()
        {
            ServiceTraits<T>::registerClass();
        }
    };

    // use static val to register allocator function for class T;
    static Allocator alloc_;
};

template <typename T>
typename ServiceRegister<T>::Allocator ServiceRegister<T>::alloc_;
