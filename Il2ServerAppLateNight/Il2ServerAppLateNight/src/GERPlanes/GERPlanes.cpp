#pragma once
#include <string>
#include <unordered_set>

const std::unordered_set<std::string> germanPlaneNames = {
	"Bf 109 E-7",
	"Bf 109 F-2",
	"Bf 109 F-4",
	"Bf 109 G-2",
	"Bf 109 G-4",
	"Bf 109 G-6",
	"Bf 109 G-6 Late",
	"Bf 109 G-6AS"
	"Bf 109 G-14",
	"Bf 109 K-4",

	"Bf-110 E2",
	"Bf-110 G2",

	"Ju-87 D3",
	"Ju-88 A4",
	"Ju-88 C6",
	"Ju-52/3m g4e",

	"FW 190 A3",
	"FW 190 A5",
	"FW 190 A6",
	"FW 190 A8",
	"FW 190 D9",

	"He 111 H-6",
	"He 111 H-16",

	"Hs 129 B-2",

	"Ar 234 B-2",

	"Me 262 A",
	"Me 410 A-1"
};

// Function to check if a name is a valid German plane name
bool IsGERPlane(const std::string& name)
{
	return germanPlaneNames.find(name) != germanPlaneNames.end();
}

bool IsBf109E7(const std::string& name)
{
	return (name == "Bf 109 E-7");
}

bool IsBf109F2(const std::string& name)
{
	return (name == "Bf 109 F-2");
}

bool IsBf109F4(const std::string& name)
{
	return (name == "Bf 109 F-4");
}

bool IsBf109F6(const std::string& name)
{
	return (name == "Bf 109 F-6");
}

bool IsBf109G2(const std::string& name)
{
	return (name == "Bf 109 G-2");
}

bool IsBf109G4(const std::string& name)
{
	return (name == "Bf 109 G-4");
}

bool IsBf109G6(const std::string& name)
{
	return (name == "Bf 109 G-6");
}

bool IsBf109G6AS(const std::string& name)
{
	return (name == "Bf 109 G-6AS");
}

bool IsBf109G6Late(const std::string& name)
{
	return (name == "Bf 109 G-6 Late");
}

bool IsBf109G14(const std::string& name)
{
	return (name == "Bf 109 G-14");
}

bool IsBf109K4(const std::string& name)
{
	return (name == "Bf 109 K-4");
}

bool IsBf110E2(const std::string& name)
{
	return (name == "Bf-110 E2");
}

bool IsBf110G2(const std::string& name)
{
	return (name == "Bf-110 G2");
}

bool IsJu87D3(const std::string& name)
{
	return (name == "Ju-87 D3");
}

bool IsJu88A4(const std::string& name)
{
	return (name == "Ju-88 A4");
}


bool IsJu88C6(const std::string& name)
{
	return (name == "Ju-88 C6");
}

bool IsJu523mG4E(const std::string& name)
{
	return (name == "Ju-52/3m g4e");
}

bool IsFW190A3(const std::string& name)
{
	return (name == "FW 190 A3");
}

bool IsFW190A5(const std::string& name)
{
	return (name == "FW 190 A5");
}

bool IsFW190A6(const std::string& name)
{
	return (name == "FW 190 A6");
}

bool IsFW190A8(const std::string& name)
{
	return (name == "FW 190 A8");
}

bool IsFW190D9(const std::string& name)
{
	return (name == "FW 190 D9");
}

bool IsHe111H6(const std::string& name)
{
	return (name == "He 111 H-6");
}

bool IsHe111H16(const std::string& name)
{
	return (name == "He 111 H-16");
}

bool IsHs129B2(const std::string& name)
{
	return (name == "Hs 129 B-2");
}

bool IsMe262A(const std::string& name)
{
	return (name == "Me 262 A");
}

bool IsAR234B2(const std::string& name)
{
	return (name == "Ar 234 B-2");
}

bool IsME410A1(const std::string& name)
{
	return (name == "Me 410 A-1");
}

bool IsMC202s8(const std::string& name)
{
	return (name == "MC 202 s8");
}

bool IsIAR80(const std::string& name)
{
	return (name == "I.A.R. 80-A");
}