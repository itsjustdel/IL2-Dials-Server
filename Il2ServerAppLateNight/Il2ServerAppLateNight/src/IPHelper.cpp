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

std::vector<std::string> IpAddresses()
{
	std::vector<std::string> ipv4Addresses;
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
					ipv4Addresses.push_back((socketAddress));
				
			}
				
			print_addr(ua);
		}
	}

	free(adapter_addresses);

	return ipv4Addresses;
}

std::vector<std::string> GetLocalIPAddresses()
{

	std::vector<std::string> ipv4Addresses;

	
	WSAData d;
	if (WSAStartup(MAKEWORD(2, 2), &d) != 0) {
		return ipv4Addresses;
	}

	ipv4Addresses = IpAddresses();

	WSACleanup();

	return ipv4Addresses;
}