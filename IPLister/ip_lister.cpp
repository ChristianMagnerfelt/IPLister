#include <winsock2.h>
#include <iphlpapi.h>
#include <IcmpAPI.h>

#include <vector>
#include <algorithm>

#define DLL_EXPORT
#include "ip_lister.hpp"

// Count the number of bits in a integer type
unsigned int countBits(unsigned int value)
{     
	unsigned int count = 0;
	for (; value != 0; ++count) 
		value &= value - 1;
	return count;
}
// Constructs a mask from a bit count
unsigned int constructMask(unsigned int numBits)
{
	unsigned int value = 0; 
	for(; numBits > 0; --numBits)
		value |= (1 << (numBits - 1));
	return value;
}
extern "C"
{
	// Gets a list of ipv4 addresses
	DECLDIR bool getListofIPv4Addresses(unsigned int networkAddr, unsigned int subnetMask, unsigned int ** addrListHandle)
	{
		// Count the bits in the subnet mask
		unsigned int count = countBits(subnetMask);

		// Check that the subnet mask is valid, if we reconstruct it, it should be the same as the first supplied.
		if(constructMask(count) != subnetMask)
			return false;

		// Calculate the number of addresses to ping
		unsigned int numAddrToPing = (0xFFFFFFFF ^ (subnetMask << (32 - count))) - 2;

		IPAddr ipaddr = networkAddr;
		DWORD replySize = sizeof(ICMP_ECHO_REPLY);

		std::vector<IPAddr> ipAddrVec(numAddrToPing);			// Contains ipv4 addresses to ping
		std::vector<ICMP_ECHO_REPLY> replyVec(numAddrToPing);	// Stores a reply for each ping
		std::vector<HANDLE> handleVec(numAddrToPing);			// Event handle to each ping
		*addrListHandle = new unsigned int[numAddrToPing + 1];			// List for which contains addresses of reachable hosts on the network

		// Change to host byte order so we can increment the addresses
		ipaddr = ntohl(ipaddr);

		// Increment and store each address
		std::generate(ipAddrVec.begin(), ipAddrVec.end(), [&ipaddr](){
			return ++ipaddr;
		});
		// Convert back all addresses to network order
		std::for_each(ipAddrVec.begin(), ipAddrVec.end(), [](IPAddr & addr){
			addr = htonl(addr);
		});

		// Create vector of event handles
		std::for_each(handleVec.begin(), handleVec.end(), [](HANDLE & eventHandle){
			eventHandle = CreateEvent(NULL, NULL, false, NULL);
		});
	
		// Create Icmp handle
		HANDLE hlcmpFile = IcmpCreateFile();

		// Start pinging addresses
		for(unsigned int i = 0; i < ipAddrVec.size(); ++i)
		{
			IcmpSendEcho2(hlcmpFile, handleVec[i], NULL, NULL, ipAddrVec[i], NULL, 0, NULL, &(replyVec[i]), replySize, 25);
		}

		// Wait for all events to be signaled
		WaitForMultipleObjects(handleVec.size(), handleVec.data(), true, 1000);
	
		unsigned int * p = (*addrListHandle);			
		unsigned int idx = 1;	// reply vector offset, first value contains number of results
		// Parse all replies
		for(unsigned int i = 0; i < replyVec.size(); ++i)
		{
			DWORD rv = IcmpParseReplies(&(replyVec[i]), replySize);

			// If the returned value is non zero, we got a successfull reply.
			if(rv != 0)
			{
				p[idx] = replyVec[i].Address;
				++idx;
			}
		}
		// Set first element to number of address entries
		p[0] = idx;

		IcmpCloseHandle(hlcmpFile);
		return true;
	}
	// Destroys and frees memory of a address handle.
	DECLDIR void destroyIPv4Addr(unsigned int ** addr)
	{
		if(addr != 0)
		{
			if(*addr != 0)
			{
				delete [] *addr;
				*addr = 0;
			}
		}
	}
}
