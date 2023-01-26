#include <string>

bool IsUSPlane(std::string name)
{
	std::string v = "P-38J-25";
	if (name.compare(v) == 0)	
		return true;

	v = "P-40E-1";
	if (name.compare(v) == 0)
		return true;
	
	v = "A-20B";
	if (name.compare(v) == 0)
		return true;	

	v = "P-39L-1";
	if (name.compare(v) == 0)
		return true;

	v = "P-47D-28";
	if (name.compare(v) == 0)
		return true;

	v = "P-51D-15";
	if (name.compare(v) == 0)
		return true;

	v = "B-25D";
	if (name.compare(v) == 0)
		return true;

	v = "P-47D-22";
	if (name.compare(v) == 0)
		return true;

	v = "C-47A";
	if (name.compare(v) == 0)
		return true;

	v = "P-51B-5";
	if (name.compare(v) == 0)
		return true;
	
	//else it is not US plane
	return false;
}

bool IsP38(std::string name)
{
	std::string v = "P-38J-25";
	if (name.compare(v) == 0)
		return true;


	//else it is not US plane
	return false;
}


