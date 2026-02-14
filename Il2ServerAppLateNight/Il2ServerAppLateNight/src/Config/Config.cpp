#include "Config.h"
#include <fstream>
#include <iostream>
#include <windows.h>
#include <shlobj.h>

std::unordered_map<std::string, std::unordered_map<std::string, DialScaling>> Config::dialVariants;
std::unordered_map<std::string, std::unordered_map<std::string, std::string>> Config::planes;

std::string Config::GetConfigPath() {
    // First try appdata
    char* appDataPath = nullptr;
    if (SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &appDataPath) == S_OK) {
        std::string path = std::string(appDataPath) + "\\IL2Dials\\config.json";
        CoTaskMemFree(appDataPath);
        if (std::ifstream(path).good()) {
            return path;
        }
    }
    
    // Fallback to exe directory
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    std::string dir = std::string(exePath);
    size_t pos = dir.find_last_of("\\/");
    if (pos != std::string::npos) {
        dir = dir.substr(0, pos);
    }
    return dir + "\\config.json";
}

bool Config::LoadConfig() {
    std::string filePath = GetConfigPath();
    return LoadConfigFromFile(filePath);
}

bool Config::LoadConfigFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << filePath << std::endl;
        return false;
    }

    nlohmann::json j;
    file >> j;

    // Clear existing data
    dialVariants.clear();
    planes.clear();

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

bool Config::DownloadAndSaveConfig(const std::string& url) {
    // Placeholder - implement download logic
    // For now, return false
    return false;
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