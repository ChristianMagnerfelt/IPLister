#ifndef DLL_TEST
	#include "ip_lister.hpp"
#endif

#include <winsock2.h>
#include <iphlpapi.h>
#include <IcmpAPI.h>
#include <Windows.h>

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <cstdio>

#include <chrono>

#if DLL_TEST
	typedef bool (*ListIPv4AddrFunc)(unsigned int, unsigned int, unsigned int **);
	typedef void (*DestroyIPv4Addr)(unsigned int **);
#endif

int main()
{	
	IPAddr ipaddr = inet_addr("192.168.0.0");
	IPAddr subnetaddr = inet_addr("255.255.255.0");
	
	unsigned int * addr = 0;

	// Load DLL and get addresses of repective function
	#if DLL_TEST
		ListIPv4AddrFunc func;
		DestroyIPv4Addr destroyFunc;
		HINSTANCE hInstLibrary = LoadLibrary("IPLister.dll");
		func = (ListIPv4AddrFunc)GetProcAddress(hInstLibrary, "getListofIPv4Addresses");
		destroyFunc = (DestroyIPv4Addr)GetProcAddress(hInstLibrary, "destroyIPv4Addr");
	#endif

	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

	#if DLL_TEST
		if(!func(ipaddr, subnetaddr, &addr))
		{
			printf("Fail");
		}
	#else
		if(!getListofIPv4Addresses(ipaddr, subnetaddr, &addr))
		{
			printf("fail");
		}
		
	#endif

	std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start;

	for(unsigned int i = 1; i < addr[0]; ++i)
	{
		struct in_addr replyAddr;
		replyAddr.S_un.S_addr = addr[i];
		printf("IP-Address: %s \n", inet_ntoa(replyAddr));
	}

	printf("Test took %f seconds\n", duration.count());

	#if DLL_TEST
		destroyFunc(&addr);
	#else
		destroyIPv4Addr(&addr);
	#endif

	_CrtDumpMemoryLeaks();

	system("pause");

	return 0;
}