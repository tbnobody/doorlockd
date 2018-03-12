#include "util.h"

template <>
int getJson(const Json::Value &root, const std::string &key)
{
    auto val = root.get(key, Json::Value());
    if (val.isInt())
        return val.asInt();
    throw std::runtime_error("Json Type error");
}

template <>
std::string getJson(const Json::Value &root, const std::string &key)
{
    auto val = root.get(key, Json::Value());
    if (val.isString())
        return val.asString();
    throw std::runtime_error("Json Type error");
}

template <>
size_t getJson(const Json::Value &root, const std::string &key)
{
    auto val = root.get(key, Json::Value());
    if (val.isInt())
        return val.asUInt64();
    throw std::runtime_error("Json Type error");
}

template <>
bool getJson(const Json::Value &root, const std::string &key)
{
    auto val = root.get(key, Json::Value());
    if (val.isBool())
        return val.asBool();
    throw std::runtime_error("Json Type error");
}

template <>
Json::Value getJson(const Json::Value &root, const std::string &key)
{
    auto val = root.get(key, Json::Value());
    return val;
}
