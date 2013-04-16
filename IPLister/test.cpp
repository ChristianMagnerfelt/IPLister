#include <winsock2.h>
#include <iphlpapi.h>
#include <IcmpAPI.h>
#include <stdio.h>

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
	char sendData[2] = "1";
    DWORD replySize = (sizeof(ICMP_ECHO_REPLY) + sizeof(sendData));
    LPVOID replyBuffer = (VOID*) malloc(replySize);

	// Create Icmp handle
	HANDLE hlcmpFile = IcmpCreateFile();

	// Start pinging
	for(unsigned int i = 0; i < numAddrToPing; ++i)
	{
		ipaddr = ntohl(ipaddr);
		ipaddr += 1;
		ipaddr = htonl(ipaddr);

		struct in_addr replyAddr;
		replyAddr.S_un.S_addr = ipaddr;
		printf("IP-Address: %s ", inet_ntoa(replyAddr));

		DWORD rv = IcmpSendEcho(hlcmpFile, ipaddr, NULL, 0, NULL, replyBuffer, replySize, 1);
		if(rv != 0)
		{
			printf("OK");
			//PICMP_ECHO_REPLY echoReply = (PICMP_ECHO_REPLY)replyBuffer;
			//printf("Number of replies: %d\n", rv);
			//printf("---\n");
			
			//printf("Status: %ld\n", echoReply->Status);
			//printf("Roundtrip time: %ld ms\n", echoReply->RoundTripTime);
		}
		else
		{
			
			//DWORD error = GetLastError();
			//printf("Error: %d\n", error);
		}
		printf("\n");
	}

	free(replyBuffer);
	IcmpCloseHandle(hlcmpFile);

	return true;
}
int main()
{
	IPAddr ipaddr = inet_addr("192.168.0.0");
	IPAddr subnetaddr = inet_addr("255.255.255.0");
	
	unsigned int * addresses = 0;
	unsigned int * numAddresses = 0;
	if(!getListofIPv4Addresses(ipaddr, subnetaddr, addresses, numAddresses))
	{
		printf("fail");
	}

	system("pause");
	return 0;
}