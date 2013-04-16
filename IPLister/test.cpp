#include <winsock2.h>
#include <iphlpapi.h>
#include <IcmpAPI.h>

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <cstdio>

#include <vector>
#include <algorithm>
#include <chrono>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

/* Count the number of bits in a integer type */
unsigned int countBits(unsigned int value)
{     
	unsigned int count = 0;
	for (; value != 0; ++count) 
		value &= value - 1;
	return count;
}
/* Constructs a mask from a bit count */
unsigned int constructMask(unsigned int numBits)
{
	unsigned int value = 0; 
	for(; numBits > 0; --numBits)
		value |= (1 << (numBits - 1));
	return value;
}

bool getListofIPv4Addresses(unsigned int networkAddr, unsigned int subnetMask, unsigned int * addr, unsigned int * numAddr)
{
	unsigned int count = countBits(subnetMask);

	// Check that the subnet mask is valid
	if(constructMask(count) != subnetMask)
		return false;

	unsigned int numAddrToPing = (0xFFFFFFFF ^ (subnetMask << (32 - count))) - 2;

	IPAddr ipaddr = networkAddr;
    DWORD replySize = sizeof(ICMP_ECHO_REPLY);

	

	std::vector<IPAddr> ipAddrVec(numAddrToPing);
	std::vector<ICMP_ECHO_REPLY> replyVec(numAddrToPing);
	std::vector<HANDLE> handleVec(numAddrToPing);

	// Create vector of all ips to ping
	ipaddr = ntohl(ipaddr);
	std::generate(ipAddrVec.begin(), ipAddrVec.end(), [&ipaddr](){
		return ++ipaddr;
	});
	std::for_each(ipAddrVec.begin(), ipAddrVec.end(), [](IPAddr & addr){
		addr = htonl(addr);
	});

	// Create vector of event handles
	std::for_each(handleVec.begin(), handleVec.end(), [](HANDLE & eventHandle){
		eventHandle = CreateEvent(NULL, NULL, false, NULL);
	});

	

	// Create Icmp handle
	HANDLE hlcmpFile = IcmpCreateFile();

	// Start pinging
	for(unsigned int i = 0; i < ipAddrVec.size(); ++i)
	{
		IcmpSendEcho2(hlcmpFile, handleVec[i], NULL, NULL, ipAddrVec[i], NULL, 0, NULL, &(replyVec[i]), replySize, 10);
	}



	// Wait for all events to be signaled
	WaitForMultipleObjects(handleVec.size(), handleVec.data(), true, 1000);




	// Parse all replies
	for(unsigned int i = 0; i < replyVec.size(); ++i)
	{
		DWORD rv = IcmpParseReplies(&(replyVec[i]), replySize);

		if(rv != 0)
		{
			struct in_addr replyAddr;
			replyAddr.S_un.S_addr = replyVec[i].Address;
			printf("IP-Address: %s \n", inet_ntoa(replyAddr));
		}
	}

	IcmpCloseHandle(hlcmpFile);

	return true;
}
int main()
{	
	IPAddr ipaddr = inet_addr("192.168.0.0");
	IPAddr subnetaddr = inet_addr("255.255.255.0");
	
	unsigned int * addresses = 0;
	unsigned int * numAddresses = 0;
	
	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
	if(!getListofIPv4Addresses(ipaddr, subnetaddr, addresses, numAddresses))
	{
		printf("fail");
	}
	std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start;
	printf("Test took %f seconds\n", duration.count());

	_CrtDumpMemoryLeaks();

	system("pause");
	return 0;
}