#include <string>
#include <unordered_set>

// Define a set of valid US plane names
const std::unordered_set<std::string> usPlaneNames = {
	"P-38J-25", "P-40E-1", "A-20B", "P-39L-1", "P-47D-28",
	"P-51D-15", "B-25D", "P-47D-22", "C-47A", "P-51B-5"
};

// Function to check if a name is a valid US plane name
bool IsUSPlane(const std::string& name)
{
	return usPlaneNames.find(name) != usPlaneNames.end();
}

// Functions for specific plane names (optional)
bool IsP38(const std::string& name)
{
	return (name == "P-38J-25");
}

bool IsP40E(const std::string& name)
{
	return (name == "P-40E-1");
}

bool IsA20B(const std::string& name)
{
	return (name == "A-20B");
}

bool IsP39L(const std::string& name)
{
	return (name == "P-39L-1");
}

bool IsP47D28(const std::string& name)
{
	return (name == "P-47D-28");
}

bool IsP51D15(const std::string& name)
{
	return (name == "P-51D-15");
}

bool IsB25D(const std::string& name)
{
	return (name == "B-25D");
}

bool IsP47D22(const std::string& name)
{
	return (name == "P-47D-22");
}

bool IsC47A(const std::string& name)
{
	return (name == "C-47A");
}

bool IsP51B5(const std::string& name)
{
	return (name == "P-51B-5");
}
