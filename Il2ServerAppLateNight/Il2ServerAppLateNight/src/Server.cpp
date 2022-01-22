#pragma comment(lib, "ws2_32.lib") //https://stackoverflow.com/questions/57831867/do-i-actually-have-to-link-ws2-32-lib
#pragma warning(disable:4996)

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
//#include <codecvt> //use to convert PCWSTR
#include <sstream>

#include "Main.h"
#include "IPHelper.h"

using namespace System::Diagnostics;

int nServerPort = 11200;           
//how many ms between sends
int sendInterval = 30;

void SetPortNumber(int t)
{
    nServerPort = t;
}

sockaddr_in CreateServerSocketAddress(bool localIP)
{
    // setup address structure
    sockaddr_in serverSocketAddress;
    //clear
    memset((char*)&serverSocketAddress, 0, sizeof(serverSocketAddress));

    //get local ipdaddress
    std::vector<std::string> ipv4Addreses = GetIPAddresses(localIP);
    if (ipv4Addreses.size() == 0)
    {
        //returne mpty
        return serverSocketAddress;
    }

    std::string ipAddress = ipv4Addreses[0];
    const std::wstring wideIPstr = std::wstring(ipAddress.begin(), ipAddress.end());
    const wchar_t* c0 = wideIPstr.c_str();
    PCWSTR serverIP = (PCWSTR)c0;

   
    serverSocketAddress.sin_family = AF_INET;
    serverSocketAddress.sin_port = htons(nServerPort);

    //convert to inet format and save in serversocketaddress    
    InetPton(AF_INET, serverIP, &serverSocketAddress.sin_addr.s_addr);
 
    return serverSocketAddress;

}

SOCKET CreateClientSocket()
{

    SOCKET client_socket;
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) // <<< UDP socket
    {
        return 0;
    }

    return client_socket;
}

int InitialiseWinsockUDP() 
{
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
    {
        return 0;
    }

    return 1;
}



int SendPackageUDP(SOCKET client_socket, sockaddr_in serverSocketAddress)
{
    //we represent the data with floats in the app, so let's convert now and save network traffic
    float floatArray[14];

    //read memory only when requested
    ReadPlaneType();
    ReadCockpitInstruments();
    ReadAltimeter();
    ReadTurnNeedle();
    ReadTurnCoordinatorBall();


    //packing differecnt data types in to one char array for sending (serialisation)
    //https://stackoverflow.com/questions/1703322/serialize-strings-ints-and-floats-to-character-arrays-for-networking-without-li

    //version
    float programVersion = GetIL2DialsVersion();
    //planeType
    std::string planeType = GetPlaneType();
    //to send over tcp we need string size and data            

    //send blanks if no game found/injected
    if (GetInjected() == false)
    {
        planeType = "No Game Process/ Injection";

        for (size_t i = 0; i < 14; i++)
        {
            floatArray[i] = 0;
        }
        
    }
    else
    {
        //if we have found the altimeter struct we can read from here, this allows us to get the needle position as it moves so we don't need to calculate that ourselves
        floatArray[0] = (float)(GetAltitude());

        //mgh
        floatArray[1] = (float)(GetMMHg());
        //airspeed
        floatArray[2] = (float)(GetAirspeedFromCockpitStruct());
        //heading
        floatArray[3] = (float)(GetHeading());
        //Pitch
        floatArray[4] = (float)(GetPitch());
        //Roll
        floatArray[5] = (float)(GetRoll());
        //vertical speed 
        floatArray[6] = (float)(GetVSI());
        //ball
        floatArray[7] = (float)(GetTurnAndBankBall());
        //bank needle
        floatArray[8] = (float)(GetTurnAndBankNeedle());
        //rpm
        for (size_t i = 0; i < 4; i++)//4 engines max
        {
            //get rpm know where the rpm struct starts
            floatArray[9 + i] = (float)(GetRPM(i));
        }
    }

    // The buffer we will be writing bytes into
    //make space for
    //program version
    //planetype string size
    //planetype string data size
    //float array containing instrument/dial values
    //64 bit string (testing needed)
    unsigned char outBuf[sizeof(uint32_t) + sizeof(uint32_t) + 64 + sizeof(floatArray)];
    // A pointer we will advance whenever we write data
    unsigned char* p = outBuf;


    //float array to buffer - NOTE THESE MUST STAY THE FIRST POSITON IN THE STREAM - 3RD PARTY DEPENDENCIES
    //we know how big this float array will be so we don't need to send size
    memcpy(p, (char*)floatArray, sizeof(floatArray));
    p += sizeof(floatArray);//4btye float * array length

    // Serialize "program version" into outBuf
    //using uint32_t when serializing            
    //copy float to uint32 safely
    uint32_t fbits = 0;
    memcpy(&fbits, &programVersion, sizeof(programVersion));
    //copy to buffer
    memcpy(p, &fbits, sizeof(fbits));
    p += sizeof(fbits);

    // Serialize "planeType string length" into outBuf
    //size_t to uint32
    uint32_t neX = static_cast<unsigned int>(planeType.size());//note it is size_t and not the size of the data in the array
    memcpy(p, &neX, sizeof(neX));
    p += sizeof(neX);

    //string data already in correct format - copy to buffer            
    memcpy(p, planeType.data(), 64);
    p += 64;


    char* sendBuffer = (char*)outBuf;
    int sendLength = sizeof(outBuf);


    // send the message
    if (sendto(client_socket, sendBuffer, sendLength, 0, (sockaddr*)&serverSocketAddress, sizeof(sockaddr_in)) == SOCKET_ERROR)
    {
       // OutputDebugString(L"sendto() failed with error code");
        return 3;
    }

    return 1;
}

//WINAPI
BOOL __clrcall   SendThreadUDP(bool localIP, System::ComponentModel::BackgroundWorker^* pWorker)
{
   // OutputDebugString(L"UDP Client thread");
   // OutputDebugString(L"\n");

    //moving from managed to unmanaged doesn't allow a worker reference to be passed, so pass a pointer and rebuild as we want
   // System::ComponentModel::BackgroundWorker^* pWorker = (System::ComponentModel::BackgroundWorker^*)worker;
    System::ComponentModel::BackgroundWorker^ worker = *pWorker;

    // initialise winsock
    InitialiseWinsockUDP();

    // create client socket
    SOCKET client_socket = CreateClientSocket();
    if (client_socket == 0)    
        worker->ReportProgress(-10);
    
    //create server sock address
    sockaddr_in serverSocketAddress = CreateServerSocketAddress(localIP);
    //check for empty socket address
    if(serverSocketAddress.sin_family == 0)
        worker->ReportProgress(-20);

    // start communication
    while (true)
    {
        SendPackageUDP(client_socket, serverSocketAddress);
        
        Sleep(sendInterval);
    }
}

int StartServerUDP(System::ComponentModel::BackgroundWorker^ worker, bool localIP)
{
   // OutputDebugString(L"Starting Server UDP");
   // OutputDebugString(L"\n");

    //pass reference to worker so we can recast after we pass to unmanaged code
    SendThreadUDP(localIP, &worker);

    return 1;
}