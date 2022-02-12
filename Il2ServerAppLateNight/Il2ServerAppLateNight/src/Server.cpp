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

int serverBroadcast()
{
    WSADATA wsaData;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock;

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    char broadcast = '1';

    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
    {

        closesocket(sock);

        return 0;

    }

    struct sockaddr_in Recv_addr;

    struct sockaddr_in Sender_addr;

    int len = sizeof(struct sockaddr_in);

    char sendMSG[] = "Broadcast message from IL-2 Dials Server";

    char recvbuff[50] = "";

    int recvbufflen = 50;

    Recv_addr.sin_family = AF_INET;

    Recv_addr.sin_port = htons(nServerPort);

    Recv_addr.sin_addr.s_addr = INADDR_BROADCAST;

    while (true)
    {
 

        sendto(sock, sendMSG, strlen(sendMSG) + 1, 0, (sockaddr*)&Recv_addr, sizeof(Recv_addr));
    }
    //recvfrom(sock, recvbuff, recvbufflen, 0, (sockaddr*)&Recv_addr, &len);

    closesocket(sock);

    WSACleanup();
}

int serverListen() {
    int iResult = 0;
    
    WSADATA wsaData;

    SOCKET RecvSocket;
    struct sockaddr_in RecvAddr;

    unsigned short Port = nServerPort;

    char RecvBuf[1024];
    int BufLen = 1024;

    struct sockaddr_in SenderAddr;
    int SenderAddrSize = sizeof(SenderAddr);

    //-----------------------------------------------
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        OutputDebugString(L"WSAStartup failed with error %d\n");
        return 1;
    }
    //-----------------------------------------------
    // Create a receiver socket to receive datagrams
    RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (RecvSocket == INVALID_SOCKET) {
        OutputDebugString(L"socket failed with error %d\n");
        return 1;
    }
    //-----------------------------------------------
    // Bind the socket to any address and the specified port.

    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(Port);
    RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    iResult = bind(RecvSocket, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
    if (iResult != 0) {
        OutputDebugString(L"bind failed with error %d\n");
        return 1;
    }

    while (true)
    {
        //-----------------------------------------------
        // Call the recvfrom function to receive datagrams
        // on the bound socket.
        wprintf(L"Receiving datagrams...\n");
        iResult = recvfrom(RecvSocket,
            RecvBuf, BufLen, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);
        if (iResult == SOCKET_ERROR) {
            OutputDebugString(L"recvfrom failed with error %d\n");
        }

        ////////////////Package//////////////////////
        //we represent the data with floats in the app, so let's convert now and save network traffic
        float floatArray[17];

        //read memory only when requested
        ReadPlaneType();
        ReadCockpitInstruments();
        ReadAltimeter();
        ReadTurnNeedle();
        ReadTurnCoordinatorBall();
        ReadManifolds();
        
        //packing differecnt data types in to one char array for sending (serialisation)
        //https://stackoverflow.com/questions/1703322/serialize-strings-ints-and-floats-to-character-arrays-for-networking-without-li

        //version
        float programVersion = GetIL2DialsVersion();
        //planeType
        std::string planeType = GetPlaneType();          

        //send blanks if no game found/injected
        if (GetInjected() == false)
        {
            planeType = "No Game Process/ Injection";

            for (size_t i = 0; i < 17; i++)
            {
                floatArray[i] = 0;
            }
            floatArray[1] = 760;//mmhg default
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
            //manifold(s)
            for (size_t i = 0; i < 4; i++)//4 engines max
            {                
                floatArray[13 + i] = (float)(GetManifold(i));
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

        // send to client (//sender is the client,it initiated         
        if (sendto(RecvSocket, sendBuffer, sendLength, 0, (struct sockaddr*)&SenderAddr, sizeof(SenderAddr)) < 0) {
            perror("sending error...\n");
            closesocket(RecvSocket);
            exit(-1);
        }
    }

    //-----------------------------------------------
    // Close the socket when finished receiving datagrams
    OutputDebugString(L"Finished receiving. Closing socket.\n");
    iResult = closesocket(RecvSocket);
    if (iResult == SOCKET_ERROR) {
        OutputDebugString(L"closesocket failed with error %d\n");
        return 1;
    }
    
    //-----------------------------------------------
    // Clean up and exit.
    OutputDebugString(L"Exiting.\n");
    WSACleanup();
    return 0;

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

    ///
 
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

int StartServerUDP(System::ComponentModel::BackgroundWorker^ worker, bool localIP)
{
    serverListen();

    return 1;
}