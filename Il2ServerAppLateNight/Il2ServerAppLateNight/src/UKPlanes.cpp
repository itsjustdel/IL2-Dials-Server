#include <string>

bool IsUKPlane(std::string planeName)
{
	std::string v = "Spitfire Mk.Vb";
	if (planeName.compare(v) == 0)
		return true;

	v = "Spitfire Mk.IXe";
	if (planeName.compare(v) == 0)
		return true;

	v = "Tempest Mk.V ser.2";
	if (planeName.compare(v) == 0)
		return true;

	v = "Hurricane Mk.II";
	if (planeName.compare(v) == 0)
		return true;

	v = "Spitfire Mk.XIV";
	if (planeName.compare(v) == 0)
		return true;

	v = "Typhoon Mk.Ib";
	if (planeName.compare(v) == 0)
		return true;

	
	return false;
}
