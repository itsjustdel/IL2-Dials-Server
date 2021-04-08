#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <stdio.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

void print_adapter(PIP_ADAPTER_ADDRESSES aa)
{
	char buf[BUFSIZ];
	memset(buf, 0, BUFSIZ);
	WideCharToMultiByte(CP_ACP, 0, aa->FriendlyName, wcslen(aa->FriendlyName), buf, BUFSIZ, NULL, NULL);
	printf("adapter_name:%s\n", buf);
}

void print_addr(PIP_ADAPTER_UNICAST_ADDRESS ua)
{
	char buf[BUFSIZ];

	int family = ua->Address.lpSockaddr->sa_family;
	printf("\t%s ", family == AF_INET ? "IPv4" : "IPv6");

	memset(buf, 0, BUFSIZ);
	getnameinfo(ua->Address.lpSockaddr, ua->Address.iSockaddrLength, buf, sizeof(buf), NULL, 0, NI_NUMERICHOST);
	printf("%s\n", buf);
}

std::vector<std::string> IpAddresses()//this canr eturn more than 1, not using
{


	std::vector<std::string> ipv4Addresses;


	WSAData d;
	if (WSAStartup(MAKEWORD(2, 2), &d) != 0) {
		return ipv4Addresses;
	}


	DWORD rv, size;
	PIP_ADAPTER_ADDRESSES adapter_addresses, aa;
	PIP_ADAPTER_UNICAST_ADDRESS ua;

	rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &size);
	if (rv != ERROR_BUFFER_OVERFLOW) {
		fprintf(stderr, "GetAdaptersAddresses() failed...");
		return ipv4Addresses;
	}
	adapter_addresses = (PIP_ADAPTER_ADDRESSES)malloc(size);

	rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapter_addresses, &size);
	if (rv != ERROR_SUCCESS) {
		fprintf(stderr, "GetAdaptersAddresses() failed...");
		free(adapter_addresses);
		return ipv4Addresses;
	}

	for (aa = adapter_addresses; aa != NULL; aa = aa->Next) {
		print_adapter(aa);
		for (ua = aa->FirstUnicastAddress; ua != NULL; ua = ua->Next) 
		{	
			//This means it is an ipv4 address which is what we want, I'm not looking for ipv6 addresses
			if (ua->Address.lpSockaddr->sa_family == AF_INET)
			{		
				//check to see if it is the internal loopback function, we don't want this

				//get readable ipaddress of socket
				char buf[INET_ADDRSTRLEN];
				getnameinfo(ua->Address.lpSockaddr, ua->Address.iSockaddrLength, buf, sizeof(buf), NULL, 0, NI_NUMERICHOST);
				
				//convert to comparable strings
				std::string loopback("127.0.0.1");
				std::string socketAddress(buf);
				
				//add to list if not loopback function ("127.0.0.1!)
				if (socketAddress!= loopback)
					ipv4Addresses.push_back(socketAddress);
				
			}
				
			print_addr(ua);
		}
	}

	free(adapter_addresses);

	WSACleanup();
	return ipv4Addresses;
}

std::vector<std::string> GetMyIPAddress()
{
	std::vector<std::string> ipv4Addresses;
	WSADATA WSAData;

	// Initialize winsock dll
	if (::WSAStartup(MAKEWORD(1, 0), &WSAData))
	{
		//AfxMessageBox(_T("Failed to find the WinSock DLL"));
		return ipv4Addresses;
	}

	char szHostName[128] = "";

	//get the standard host name of the machine
	if (::gethostname(szHostName, sizeof(szHostName)))
	{
		//AfxMessageBox(_T("Failed to get the host name"));
	}


	struct sockaddr_in SocketAddress;
	struct hostent* pHost = 0;

	// Get local IP addresses
	pHost = ::gethostbyname(szHostName);

	if (!pHost)
	{
		//AfxMessageBox(_T("Failed to get the host information."));
		return ipv4Addresses;
	}

	char aszIPAddresses[10][16]; // maximum of ten IP addresses

	for (int nCount = 0; ((pHost->h_addr_list[nCount]) && (nCount < 10)); ++nCount)
	{
		memcpy(&SocketAddress.sin_addr, pHost->h_addr_list[nCount], pHost->h_length);
		strcpy(aszIPAddresses[nCount], inet_ntoa(SocketAddress.sin_addr));
	}

	//convert to readable format with inte-ntop (specialised function for this purpose)
	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(SocketAddress.sin_addr), str, INET_ADDRSTRLEN);
	
	ipv4Addresses.push_back(str);

	// Cleanup
	WSACleanup();

	return ipv4Addresses;
}

std::vector<std::string> GetLocalIPAddresses()
{
	std::vector<std::string> ipv4Addresses;
	ipv4Addresses = GetMyIPAddress();

	return ipv4Addresses;
}