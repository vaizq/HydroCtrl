#pragma once

#include <fstream>
#include <nlohmann/json.hpp>





nlohmann::json jsonFromFile(const std::string& filename)
{
    std::ifstream f(filename);
    return nlohmann::json::parse(f);
}
