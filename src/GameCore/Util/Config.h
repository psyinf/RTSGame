#pragma once
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <magic_enum.hpp>

#include <any>
#include <string>
#include <map>
#include <filesystem>


namespace config
{

// see https://github.com/mircodezorzi/tojson/blob/master/tojson.hpp
inline nlohmann::json parse_scalar(const YAML::Node& node)
{
    int         i;
    double      d;
    bool        b;
    std::string s;

    if (YAML::convert<int>::decode(node, i))
        return i;
    if (YAML::convert<double>::decode(node, d))
        return d;
    if (YAML::convert<bool>::decode(node, b))
        return b;
    if (YAML::convert<std::string>::decode(node, s))
        return s;

    return nullptr;
}

inline nlohmann::json yaml2json(const YAML::Node& root)
{
    nlohmann::json j{};

    switch (root.Type())
    {
    case YAML::NodeType::Null:
        break;
    case YAML::NodeType::Scalar: 
        return parse_scalar(root);
    case YAML::NodeType::Sequence:
        for (auto&& node : root)
            j.emplace_back(yaml2json(node));
        break;
    case YAML::NodeType::Map:
        for (auto&& it : root)
            j[it.first.as<std::string>()] = yaml2json(it.second);
        break;
    default: 
        throw std::invalid_argument(std::format("Unkown yaml node type {0} cannot be transformed to JSON", magic_enum::enum_name( root.Type()) ));
        break;
    }
    return j;
}


template <class T>
inline static T load(const std::string& fileName)
{
    
    static std::map<std::string, std::any> configItems;
    
    auto                                   iter = configItems.find(fileName);
    if (iter == configItems.end())
    {
        try
        {
            if (!std::filesystem::exists(fileName))
            {
                throw std::runtime_error("Cannot find config file: " + fileName);
            }
            if (fileName.ends_with(".json"))
            {
                auto conf_item = nlohmann::json::parse(std::ifstream(fileName)).get<T>();
                configItems.try_emplace(fileName, conf_item);
                return conf_item;
            }
            else if (fileName.ends_with(".yaml"))
            {
                YAML::Node config = YAML::LoadFile(fileName);
                auto conf_item = yaml2json(config);
                configItems.try_emplace(fileName, conf_item);
                return conf_item;
            }
            throw std::invalid_argument("No parser for " + fileName);
            
        }
        catch (const std::exception& e)
        {
            throw std::invalid_argument("Error reading " + fileName + ": " + e.what());
        }
    }
    else
    {
        auto conf_item = std::any_cast<T>((*iter).second);

        return conf_item;
    }
}


} // namespace config