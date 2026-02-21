#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Ole32.lib")

#include "Config.h"
#include <fstream>
#include <iostream>
#include <windows.h>
#include <shlobj.h>

std::unordered_map<std::string, std::unordered_map<std::string, DialScaling>> Config::dialVariants;
std::unordered_map<std::string, std::unordered_map<std::string, std::string>> Config::planes;

std::string Config::GetConfigPath() {
	// First try appdata    
	PWSTR appDataPath = nullptr;

	if (SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &appDataPath) == S_OK) {
		std::wstring wpath(appDataPath);
		std::string path(wpath.begin(), wpath.end());
		path += "\\IL2Dials\\plane-config.json";

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
	return dir + "\\plane-config.json";
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
		std::vector<std::string> keys = j["dial_variants"].get<std::vector<std::string>>(); // JSON array of keys

		for (size_t i = 0; i < keys.size(); ++i) {
			std::string key = keys[i];
			nlohmann::json value = j["dial_variants"][key];

			std::unordered_map<std::string, DialScaling> scalings;

			std::vector<std::string> subkeys = value.get<std::vector<std::string>>();
			for (size_t j = 0; j < subkeys.size(); ++j) {
				std::string subkey = subkeys[j];
				nlohmann::json subvalue = value[subkey];

				DialScaling ds;
				if (subvalue.contains("min")) ds.min = subvalue["min"].get<float>();
				if (subvalue.contains("max")) ds.max = subvalue["max"].get<float>();

				scalings[subkey] = ds;
			}

			dialVariants[key] = scalings;
		}
	}


	// Load planes
	if (j.find("planes") != j.end()) {
		nlohmann::json::iterator planesIt = j["planes"].begin();
		nlohmann::json::iterator planesEnd = j["planes"].end();

		while (planesIt != planesEnd) {
			std::string planeName = planesIt.key(); // the outer key
			std::unordered_map<std::string, std::string> variants;

			nlohmann::json planeObj = planesIt.value();
			nlohmann::json::iterator variantIt = planeObj.begin();
			nlohmann::json::iterator variantEnd = planeObj.end();

			while (variantIt != variantEnd) {
				std::string dialType = variantIt.key();
				std::string variantName = variantIt.value().get<std::string>(); // convert JSON value to string
				variants[dialType] = variantName;

				++variantIt;
			}

			planes[planeName] = variants;

			++planesIt;
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