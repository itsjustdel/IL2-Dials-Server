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

bool IsYak169(std::string name)
{
	std::string v = "Yak-1 ser.69";
	if (name.compare(v) == 0)
		return true;

	return false;
}

bool IsYaks127(std::string name)
{
	std::string v = "Yak-1 ser.127";
	if (name.compare(v) == 0)
		return true;

	return false;
}

bool IsYak7b36(std::string name)
{
	std::string v = "Yak-7B ser.36";
	if (name.compare(v) == 0)
		return true;

	return false;
}

bool IsLagg3s29(std::string name)
{
	std::string v = "LaGG-3 ser.29";
	if (name.compare(v) == 0)
		return true;

	return false;
}

bool IsIL2(std::string name)
{
	std::string v = "Il-2 mod.1941";
	if (name.compare(v) == 0)
		return true;
	v = "Il-2 mod.1942";
	if (name.compare(v) == 0)
		return true;
	v = "Il-2 mod.1943";
	if (name.compare(v) == 0)
		return true;

	return false;
}

bool IsLa5s8(std::string name)
{
	std::string v = "La-5 ser.8";
	if (name.compare(v) == 0)
		return true;

	return false;
}

bool IsI16(std::string name)
{
	std::string v = "I-16 type 24";
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

bool IsLa5fns2(std::string name)
{
	std::string v = "La-5 FN ser.2";
	if (name.compare(v) == 0)
		return true;

	return false;
}

bool IsPe2(std::string name)
{
	std::string v = "Pe-2 ser.87";
	if (name.compare(v) == 0)
		return true;
	v = "Pe-2 ser.35";
	if (name.compare(v) == 0)
		return true;

	return false;
}

bool IsRUPlane(std::string name)
{
	if (IsYak9(name))
		return true;
	else if (IsMig3(name))
		return true;

	return false;
}