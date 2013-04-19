///	\brief			This header contains a function getListofIPv4Addresses that sends ICMP echo request in 
///					order to discover hosts on the local network. When all ICMP echo request are sent, 
///					discovered host are compiled into a list which is returned to the caller.
///
///	\description	getListofIPv4Addresses uses the win API in order to send ICMP requests. 
///					Given a sub-network mask and the base network address, the IP Lister sends
///					a echo to each adress in the address span. The request are done in parallel in order
///					to avoid stalling the system.  A ICMP echo can either be successfull 
///					or fail depending on whenever a echo request does or does not arive before the given timeout deadline.
///					 
///					getListofIPv4Addresses checks if the sub network mask is a valid mask. getListofIPv4Addresses makes no checks regarding 
///					if the suplied network adress + subnetwork mask is a network reachable from the calling host. Due to the design
///					its possible to send ICMP request to hosts outside the local network, however this is not recommended as the request will probably
///					be blocked and other better mechnismns exist for that purpose. Make sure that the subnetwork mask spans as few addresses as possible in
///					order to not flood the network ICMP echo requests.
///
///	\author			Christian Magnerfelt
///	\date			2013-04-19

#ifndef IP_LISTER_HPP
#define IP_LISTER_HPP

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif
 
///	\brief			Counts and returns the number of bits in a unsigned value.
///	\param	value	The value to count.
///	\return			The bit count.
unsigned int countBits(unsigned int value);

///	\brief			Constructs a sub network mask given a bit count. Ex. 24 bits would result in a unsigned 
///					int with the equivalent value of the address 255.255.255.0.
///	\param	count	The bit count.
///	\return			The sub network mask.
unsigned int constructMask(unsigned int count);

extern "C"
{
	///	\brief					Gets a list of IPv4 addresses of all hosts in network given a subnet mask. 
	///	\param	networkAddr		The base network address.
	///	\param	subnetMask		The sub network mask.
	///	\param	addrListHandle	The handle to the list of ipv4 addresses.
	DECLDIR bool getListofIPv4Addresses(unsigned int networkAddr, unsigned int subnetMask, unsigned int ** addrListHandle);

	/// \brief					Destroys and frees a address list handle.
	///	\param	addrListHandle	The handle to the adress list
	DECLDIR void destroyIPv4Addr(unsigned int ** addrListHandle);
}

#endif