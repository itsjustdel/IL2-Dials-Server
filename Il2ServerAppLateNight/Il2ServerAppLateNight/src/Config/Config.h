#pragma once
#include <string>
#include <map>
#include <unordered_map>

// Assuming nlohmann/json is added to the project
#include <nlohmann/json.hpp>

struct DialScaling {
    float min = 0.0f;
    float max = 1.0f;
    // Add other params if needed, e.g., offset
};

class Config {
public:
    static bool LoadConfig();
    static bool DownloadAndSaveConfig(const std::string& url);
    static DialScaling GetDialScaling(const std::string& dialName, const std::string& variant);
    static std::string GetPlaneDialVariant(const std::string& planeName, const std::string& dialType);

private:
    static std::string GetConfigPath();
    static bool LoadConfigFromFile(const std::string& filePath);

    static std::unordered_map<std::string, std::unordered_map<std::string, DialScaling>> dialVariants;
    static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> planes;

};