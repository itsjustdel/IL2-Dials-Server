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