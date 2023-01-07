#include <string>

bool IsYak9(std::string name)
{
	std::string v = "Yak-9 ser.1";
	if (name.compare(v) == 0)
		return true;

	v = "Yak-9T ser.1";
	if (name.compare(v) == 0)
		return true;

	return false;
}

bool IsMig3(std::string name)
{
	std::string v = "MiG-3 ser.24";
	if (name.compare(v) == 0)
		return true;

	return false;
}

bool isRUPlane(std::string name) {
	if (IsYak9(name))
		return true;
	else if (IsMig3(name))
		return true;

	return false;
}