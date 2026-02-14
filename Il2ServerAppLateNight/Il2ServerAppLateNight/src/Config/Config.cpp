#include "Config.h"
#include <fstream>
#include <iostream>

std::unordered_map<std::string, std::unordered_map<std::string, DialScaling>> Config::dialVariants;
std::unordered_map<std::string, std::unordered_map<std::string, std::string>> Config::planes;

bool Config::LoadConfig(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << filePath << std::endl;
        return false;
    }

    nlohmann::json j;
    file >> j;

    // Load dial_variants
    if (j.contains("dial_variants")) {
        for (auto& [key, value] : j["dial_variants"].items()) {
            std::unordered_map<std::string, DialScaling> scalings;
            for (auto& [subkey, subvalue] : value.items()) {
                DialScaling ds;
                if (subvalue.contains("min")) ds.min = subvalue["min"];
                if (subvalue.contains("max")) ds.max = subvalue["max"];
                scalings[subkey] = ds;
            }
            dialVariants[key] = scalings;
        }
    }

    // Load planes
    if (j.contains("planes")) {
        for (auto& [key, value] : j["planes"].items()) {
            std::unordered_map<std::string, std::string> variants;
            for (auto& [subkey, subvalue] : value.items()) {
                variants[subkey] = subvalue;
            }
            planes[key] = variants;
        }
    }

    return true;
}

DialScaling Config::GetDialScaling(const std::string& dialName, const std::string& variant) {
    std::string key = dialName + "_" + variant;
    if (dialVariants.count(key) && dialVariants[key].count("max")) {
        return dialVariants[key]["max"];
    }
    return DialScaling{}; // Default
}

std::string Config::GetPlaneDialVariant(const std::string& planeName, const std::string& dialType) {
    if (planes.count(planeName) && planes[planeName].count(dialType)) {
        return planes[planeName][dialType];
    }
    return "A"; // Default variant
}