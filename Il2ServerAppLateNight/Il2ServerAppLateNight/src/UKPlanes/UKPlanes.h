#pragma once
#include <string>

// Declarations for individual UK plane type checks
bool IsSpitfireMkVb(const std::string& name);
bool IsSpitfireMkIXc(const std::string& name);
bool IsSpitfireMkIXe(const std::string& name);
bool IsTempestMkVser2(const std::string& name);
bool IsHurricaneMkII(const std::string& name);
bool IsSpitfireMkXIV(const std::string& name);
bool IsSpitfireMkXIVe(const std::string& name);
bool IsTyphoonMkIb(const std::string& name);
bool IsMosquitoFBMkVIser2(const std::string& name);

// Declaration for checking if a name is a valid UK plane name
bool IsUKPlane(const std::string& name);
