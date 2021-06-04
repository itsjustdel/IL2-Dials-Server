#pragma comment(lib, "ws2_32.lib") //https://stackoverflow.com/questions/57831867/do-i-actually-have-to-link-ws2-32-lib
#pragma warning(disable:4996)

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include "Main.h"
#include <string>
#include <vector>
#include <thread>
#include <locale>
#include <chrono>
#include <codecvt>

#include "IPHelper.h"

struct CLIENT_INFO
{
    SOCKET hClientSocket;
    struct sockaddr_in clientAddr;   
    //used to remove thread from thread list when closing
   // DWORD dwThreadId;    
    //needs to be a pointer because it is non managed. We will recast aftert it has been passsed through to worker function
    System::ComponentModel::BackgroundWorker^* worker;

};

 // Put here the IP address of the server
int nServerPort = 11200;                      // The server port that will be used by
wchar_t szServerIPAddr[20] = L"192.168.1.76"; //finding auto- may need for user input                                       // clients to talk with the server

bool InitWinSock2_0();
BOOL WINAPI ClientThread(LPVOID lpData);
std::vector<CLIENT_INFO> clientThreads;

void SetPortNumber(int t)
{
    nServerPort = t;
}

bool ClientInList(CLIENT_INFO clientInfo)
{
    bool inList = false;

    for (size_t i = 0; i < clientThreads.size(); i++)
    {
        if (clientInfo.hClientSocket == clientThreads[i].hClientSocket)
        {
            inList = true;
        }
    }

    return inList;
}

int StartServer(System::ComponentModel::BackgroundWorker^ worker, bool localIP)
{
    OutputDebugString(L"Starting Server");
    OutputDebugString(L"\n");

    //get our local ipdaddress
    std::vector<std::string> ipv4Addreses = GetIPAddresses(localIP);

    if (ipv4Addreses.size() > 1)
    {
        OutputDebugString(L"WARNING - Returned more than one IP address!");
        OutputDebugStringW(L"\n");
        //std::cout << "WARNING - Returned more than one IP address!" << std::endl;
        worker->ReportProgress(-1);
        return -1;
    }

    std::string ipAddress = ipv4Addreses[0];

    //std::cout << "Starting Server" << std::endl;

    if (!InitWinSock2_0())
    {
        OutputDebugString(L"Unable to Initialize Windows Socket environment");
        OutputDebugString(L"\n");
        std::cout << "Unable to Initialize Windows Socket environment" << WSAGetLastError() << std::endl;
        worker->ReportProgress(-2);
        //return -1;
    }

    SOCKET hServerSocket;

    hServerSocket = socket(
        AF_INET,        // The address family. AF_INET specifies TCP/IP
        SOCK_STREAM,    // Protocol type. SOCK_STREM specified TCP
        0               // Protoco Name. Should be 0 for AF_INET address family
    );
    if (hServerSocket == INVALID_SOCKET)
    {
        OutputDebugString(L"Unable to Create Socket");
        OutputDebugString(L"\n");
        std::cout << "Unable to create Server socket" << std::endl;
        // Cleanup the environment initialized by WSAStartup()
        WSACleanup();
        worker->ReportProgress(-3);
        return -1;
    }

    // Create the structure describing various Server parameters
    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;     // The address family. MUST be AF_INET
    //serverAddr.sin_addr.s_addr = inet_addr(szServerIPAddr); //https://stackoverflow.com/questions/36683785/inet-addr-use-inet-pton-or-inetpton-instead-or-define-winsock-deprecated
    PCWSTR ipFromVariable = (PCWSTR)(szServerIPAddr);
    
    //convert from string to wide char - converting to a const ensures it stays literal - 
    //https://stackoverflow.com/questions/13690306/conversion-from-string-literal-to-char-is-deprecated
    //If you want to safely assign a string literal to a pointer (which involves an implicit array-to-pointer conversion), you need to declare the target pointer as const char*, not just as char*.
    const std::wstring wideIPstr = std::wstring(ipAddress.begin(), ipAddress.end());
    const wchar_t* c0 = wideIPstr.c_str();

    //note this isn't byte perfect? compare to ipFromVariable - but working...
    PCWSTR ip = (PCWSTR)(c0);
   
    InetPton(AF_INET, ip, &serverAddr.sin_addr.s_addr); //changed to compile-del
    serverAddr.sin_port = htons(nServerPort);

    // Bind the Server socket to the address & port
    if (bind(hServerSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        OutputDebugString(L"Unable to bind to port");
        OutputDebugString(L"\n");
        std::cout << "Unable to bind to " << ip << " port " << nServerPort << std::endl;
        // Free the socket and cleanup the environment initialized by WSAStartup()
        closesocket(hServerSocket);
        WSACleanup();
        worker->ReportProgress(-4);
        return -1;
    }

    // Put the Server socket in listen state so that it can wait for client connections
    if (listen(hServerSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        OutputDebugString(L"Unable to put server in listen state");
        OutputDebugString(L"\n");
        std::cout << "Unable to put server in listen state" << std::endl;
        // Free the socket and cleanup the environment initialized by WSAStartup()
        closesocket(hServerSocket);
        WSACleanup();

        worker->ReportProgress(-5);
        return -1;
    }

    // As the socket is in listen mode there is a connection request pending.
        // Calling accept( ) will succeed and return the socket for the request.
  

    // Start loop if ...
    while (true)
    {
        SOCKET hClientSocket;
        struct sockaddr_in clientAddr;
        int nSize = sizeof(clientAddr);

        OutputDebugString(L"Starting blocking accept()");
        OutputDebugString(L"\n");
        std::cout << "Starting Accept (blocking function)" << std::endl;
        //blocking
        hClientSocket = accept(hServerSocket, (struct sockaddr*)&clientAddr, &nSize);
        OutputDebugString(L"Completed accept");
        OutputDebugString(L"\n");
        std::cout << "Completed Accept (blocking function)" << std::endl;

        if (hClientSocket == INVALID_SOCKET)
        {
            worker->ReportProgress(-6);
            OutputDebugString(L"accept()failed");
            std::cout << "accept( ) failed" << std::endl;
        }
        else
        {
            HANDLE hClientThread;
            struct CLIENT_INFO clientInfo;
            DWORD dwThreadId;

            clientInfo.clientAddr = clientAddr;
            clientInfo.hClientSocket = hClientSocket;
            clientInfo.worker = &worker;
            //check we don't already have this client in our list
            if (ClientInList(clientInfo))
            {
                OutputDebugString(L"Client connected");
                OutputDebugStringW(L"\n");
                continue;
            }

            
            OutputDebugString(L"Client connected");
            OutputDebugStringW(L"\n");
            std::cout << "Client connected from ..." << std::endl;// << inet_ntoa(clientAddr.sin_addr) << std::endl;
            
            // Start the client thread - could be background worker - keeping so I have an example of a thread started in this way
            hClientThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ClientThread, (LPVOID)&clientInfo, 0, &dwThreadId);


            if (hClientThread == NULL)
            {
                worker->ReportProgress(-7);
                OutputDebugString(L"Unable to create client thread");
                OutputDebugString(L"\n");
                std::cout << "Unable to create client thread" << std::endl;
            }
            else
            {
                CloseHandle(hClientThread);
            }
        }
    }

    closesocket(hServerSocket);
    WSACleanup();

    return 1;
}

bool InitWinSock2_0()
{
    WSADATA wsaData;
    WORD wVersion = MAKEWORD(2, 0);

    if (!WSAStartup(wVersion, &wsaData))
        return true;

    return false;
}


void RemoveClient(CLIENT_INFO clientInfo)
{
    //rebuild managed
    //rebuild managed
    System::ComponentModel::BackgroundWorker^* pWorker = (System::ComponentModel::BackgroundWorker^*)clientInfo.worker;
    System::ComponentModel::BackgroundWorker^ worker = *pWorker;

    //remove from our thread list
    for (size_t i = 0; i < clientThreads.size(); i++)
    {
        if (clientThreads[i].hClientSocket == clientInfo.hClientSocket)
        {
            //remove 
            clientThreads.erase(clientThreads.begin() + i);

            i = clientThreads.size();

            //report to background worker
            worker->ReportProgress(clientThreads.size());

            // Close this socket 
            closesocket(clientInfo.hClientSocket);    

        }
    }
}

BOOL WINAPI ClientThread(LPVOID lpData)
{

    CLIENT_INFO* pClientInfoA = (CLIENT_INFO*)lpData;
    //this is our instance, so it stays in this thread and doesn't get overwritten
    CLIENT_INFO clientInfo = *pClientInfoA;

    //rebuild managed
    System::ComponentModel::BackgroundWorker^* pWorker = (System::ComponentModel::BackgroundWorker^*)clientInfo.worker;
    System::ComponentModel::BackgroundWorker^ worker = *pWorker;

    //let's enter this in to the list of clients
    clientThreads.push_back(clientInfo);

    //report new client amount to background worker
    worker->ReportProgress(clientThreads.size());
    
    //Some variables to detect client disconnects - We will use a socket timeout and a client send timeout 

    //set socket so it timeouts after receiving no data for x seconds
    DWORD timeout = 5 * 1000;
    setsockopt(clientInfo.hClientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
   
    //we will use a timer to check for client timeout
    auto lastReceivedTime = std::chrono::system_clock::now();
    
    
    while (true)
    {   
        std::vector<char> buf(5000);
        int recv_size = 0;
        //read socket (with timeout set before while loop)
        recv_size = recv(clientInfo.hClientSocket, buf.data(), buf.size(), 0);

        //check for client exits or timeouts
        if (recv_size == 0)
        {
            //if we haven't received anything, start a timout timer
            auto currentTime = std::chrono::system_clock::now();

            //check for 5 secodns no receive
            auto timeInSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime- lastReceivedTime).count();
            if (timeInSeconds > 5)
            {
                // Close this socket 
                RemoveClient(clientInfo);                
                //and end thread
                return true;
            }
        }
        //check this works and thne figure out
        else if (recv_size == SOCKET_ERROR)
        {
            ////if we get a timeout or error, client must have gone.
            // Close this socket 
            RemoveClient(clientInfo);
            //and end thread
            return true;

        }
        else
        {
            //note last time we received this data
            lastReceivedTime = std::chrono::system_clock::now();
        }

        //
        if (recv_size > 5000)
        {
            //overflow? //takes a long time
            OutputDebugString(L"Connection problem, will lose data");
            OutputDebugStringW(L"\n");
            std::cout << "Connection problem - will lose some data" << std::endl;
        }
      
        //We have data to read! The client has requested something
        //check what event code we have -- if we have more than one code we will need to parse all bytes and check for differeing codes

        //at the the moment the only thing being sent is a request for cockpit instrument values
        //TODO - PARSE ALL CODES (ONLY NEEDED IF MORE THAN ONE CODE ADDED)
        char evCode = buf[0];

        if (evCode == 0x01)
        {
            //This is a request for cockpitData Values

//            std::cout << "evcode == 1 - cockpit and altimeter values" << std::endl;

            //we represent the data with floats in the app, so let's convert now and save network traffic
            float floatArray[3];            
            //read memory only when requested
            ReadPlaneType();
            ReadCockpitInstruments();
            ReadAltimeter();
      
            //packing differecnt data types in to one char array for sending (serialisation)
            //https://stackoverflow.com/questions/1703322/serialize-strings-ints-and-floats-to-character-arrays-for-networking-without-li
            
            //version
            float programVersion = GetIL2DialsVersion();
            //planeType
            std::string planeType = GetPlaneType();
            //to send over tcp we need string size and data            
            
            //if we have found the altimeter struct we can read from here, this allows us to get the needle position as it moves so we don't need to calculate that ourselves
            floatArray[0] = GetAltitude();            
            //mgh
            floatArray[1] =  GetMMHg();
            //airspeed
            floatArray[2] = GetAirspeedFromCockpitStruct();

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
            
            //float array to buffer
            //we know how big this float array will be so we don't need to send size
            memcpy(p, (char*)floatArray, sizeof(floatArray));
           // p += planeType.size();//not needed


            //send
            int nCntSend = 0;
            //char* sendBuffer = (char*)floatArray;
            //int sendLength = sizeof(floatArray);
            char* sendBuffer = (char*)outBuf;
            int sendLength =  sizeof(outBuf);
            


            // send( ) may not be able to send the complete data in one go.
            // So try sending the data in multiple requests
            
            

            while ((nCntSend = send(clientInfo.hClientSocket, sendBuffer, sendLength, 0) != sendLength))
            {
                if (nCntSend == -1)
                {
                    //timer and close handle?
                    OutputDebugString(L"error sending to client");
                    std::cout << "Error sending the data to ...";// << inet_ntoa(pClientInfo->clientAddr.sin_addr) << std::endl;
                    break;
                }
                if (nCntSend == sendLength)
                    break;

                sendBuffer += nCntSend;
                sendLength -= nCntSend;
            }

            
        }
    }

    return TRUE;
}