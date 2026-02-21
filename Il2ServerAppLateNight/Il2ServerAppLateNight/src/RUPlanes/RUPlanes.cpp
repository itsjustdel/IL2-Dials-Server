#include <string>
#include <unordered_set>

// Define a set of valid Russian plane names
const std::unordered_set<std::string> ruPlaneNames = {
	"Yak-9 ser.1", "Yak-9T ser.1", "Yak-1 ser.69", "Yak-1 ser.127",
	"Yak-1 ser.23", "Yak-3 ser.9", "Yak-7B ser.36", "LaGG-3 ser.29", "LaGG-3 ser.4", "Il-2 mod.1941", "Il-2 mod.1942",
	"Il-2 mod.1943", "La-5 ser.8", "I-16 type 24", "MiG-3 ser.24",
	"La-5 FN ser.2", "La-7 ser.1", "I-153", "Pe-2 ser.87", "Pe-2 ser.35","Li-2"
};

// Function to check if a name is a valid Russian plane name
bool IsRUPlane(const std::string& name)
{
	return ruPlaneNames.find(name) != ruPlaneNames.end();
}

// Functions for specific Russian plane names

bool IsLi2(const std::string& name)
{
	return(name == "Li-2");
}
bool IsYak9(const std::string& name)
{
	return (name == "Yak-9 ser.1");
}

bool IsYak9T(const std::string& name)
{
	return (name == "Yak-9T ser.1");
}

bool IsYak169(const std::string& name)
{
	return (name == "Yak-1 ser.69");
}

bool IsYaks127(const std::string& name)
{
	return (name == "Yak-1 ser.127");
}

bool IsYak7b36(const std::string& name)
{
	return (name == "Yak-7B ser.36");
}

bool IsLagg3s29(const std::string& name)
{
	return (name == "LaGG-3 ser.29");
}

bool IsLagg3s4(const std::string& name)
{
	return (name == "LaGG-3 ser.4");
}

bool IsIL2(const std::string& name)
{
	return (name == "Il-2 mod.1941" || name == "Il-2 mod.1942" || name == "Il-2 mod.1943" || name == "Il-2 mod.1944");
}

bool IsLa5s8(const std::string& name)
{
	return (name == "La-5 ser.8");
}

bool IsI16(const std::string& name)
{
	return (name == "I-16 type 24");
}

bool IsI153(const std::string& name)
{
	return (name == "I-153");
}

bool IsMig3(const std::string& name)
{
	return (name == "MiG-3 ser.24");
}

bool IsLa5fns2(const std::string& name)
{
	return (name == "La-5 FN ser.2");
}

bool IsLa7s1(const std::string& name)
{
	return (name == "La-7 ser.1");
}

bool IsYak123(const std::string& name)
{
	return (name == "Yak-1 ser.23");
}

bool IsYak3s9(const std::string& name)
{
	return (name == "Yak-3 ser.9");
}

bool IsLa5fs38(const std::string& name)
{
	return (name == "La-5 F ser.38");
}

bool IsPe2(const std::string& name)
{
	return (name == "Pe-2 ser.87" || name == "Pe-2 ser.35");
}

bool IsPe2s35(const std::string& name)
{
	return (name == "Pe-2 ser.35");
}

bool IsPe2s87(const std::string& name)
{
	return (name == "Pe-2 ser.87");
}

bool IsU2VS(const std::string& name)
{
	return (name == "U-2VS");
}
