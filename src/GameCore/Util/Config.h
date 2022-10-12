#pragma once
#include <nlohmann/json.hpp>
#include <any>
#include <string>
#include <map>
#include <filesystem>


namespace config
{

template <class T>
static T load(const std::string& fileName)
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
            auto conf_item = nlohmann::json::parse(std::ifstream(fileName)).get<T>();
            configItems.emplace(fileName, conf_item);
            return conf_item;
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error("Error reading " + fileName + ": " + e.what());
        }
    }
    else
    {
        auto conf_item = std::any_cast<T>((*iter).second);

        return conf_item;
    }
}
} // namespace config