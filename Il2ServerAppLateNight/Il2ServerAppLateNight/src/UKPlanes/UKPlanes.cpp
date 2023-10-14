#include <string>
#include <unordered_set>

// Define a set of valid UK plane names
const std::unordered_set<std::string> ukPlaneNames = {
	"Spitfire Mk.Vb", "Spitfire Mk.IXe", "Tempest Mk.V ser.2",
	"Hurricane Mk.II", "Spitfire Mk.XIV","Spitfire Mk.XIVe",
	"Typhoon Mk.Ib", "Mosquito F.B. Mk.VI ser.2"
};

// Function to check if a name is a valid UK plane name
bool IsUKPlane(const std::string& name)
{
	return ukPlaneNames.find(name) != ukPlaneNames.end();
}

// Functions for specific UK plane names (optional)
bool IsSpitfireMkVb(const std::string& name)
{
	return (name == "Spitfire Mk.Vb");
}

bool IsSpitfireMkIXe(const std::string& name)
{
	return (name == "Spitfire Mk.IXe");
}

bool IsTempestMkVser2(const std::string& name)
{
	return (name == "Tempest Mk.V ser.2");
}

bool IsHurricaneMkII(const std::string& name)
{
	return (name == "Hurricane Mk.II");
}

bool IsSpitfireMkXIV(const std::string& name)
{
	return (name == "Spitfire Mk.XIV");
}

bool IsTyphoonMkIb(const std::string& name)
{
	return (name == "Typhoon Mk.Ib");
}

bool IsMosquitoFBMkVIser2(const std::string& name)
{
	return (name == "Mosquito F.B. Mk.VI ser.2");
}
