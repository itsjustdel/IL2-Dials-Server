
#include <iostream> 
#include <windows.h> 
#include <tlhelp32.h> 
#include <tchar.h> 
#include <vector>
#include <string>
#include <fstream>


bool getFileContent(std::string fileName, std::vector<std::wstring>& vecOfStrs)
{
	// Open the File
	std::wifstream in(fileName.c_str());
	// Check if object is valid
	if (!in)
	{
		std::cerr << "Cannot open the File : " << fileName << std::endl;
		return false;
	}
	std::wstring str;
	// Read the next line from File untill it reaches the end.
	while (std::getline(in, str))
	{
		// Line contains string of length > 0 then save it in vector
		if (str.size() > 0)
			vecOfStrs.push_back(str);
	}
	//Close The File
	in.close();
	return true;
}

bool closeProcesses(std::vector<std::wstring> pNames)
{
	for (size_t i = 0; i < 2; i++)
	{
		HANDLE hProcessSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

		PROCESSENTRY32 ProcessEntry = { 0 };
		ProcessEntry.dwSize = sizeof(ProcessEntry);

		BOOL Return = FALSE;
	Label:Return = Process32First(hProcessSnapShot, &ProcessEntry);

		if (!Return)
		{
			goto Label;
		}

		do
		{
			int value = _tcsicmp(ProcessEntry.szExeFile, pNames[i].data());
			//replace the taskmgr.exe to the process u want to remove. 
			if (value == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, ProcessEntry.th32ProcessID);
				TerminateProcess(hProcess, 0);
				CloseHandle(hProcess);
			}

		} while (Process32Next(hProcessSnapShot, &ProcessEntry));

		CloseHandle(hProcessSnapShot);
	}

	return true;
}



int main()
{
	std::vector<std::wstring> pNames;
	// Get the contents of file in a vector
	bool fileFound = getFileContent("processesToClose.ini", pNames);

	//std::vector<std::wstring> pNames;// = { L"Il-2 Dials.exe", L"IL-2 Dials Server.exe" };

	if(fileFound)
		closeProcesses(pNames);


}