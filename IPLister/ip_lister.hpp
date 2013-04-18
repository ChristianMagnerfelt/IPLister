#ifndef IP_LISTER_HPP
#define IP_LISTER_HPP

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif
 
unsigned int countBits(unsigned int value);
unsigned int constructMask(unsigned int value);

extern "C"
{
   DECLDIR bool getListofIPv4Addresses(unsigned int networkAddr, unsigned int subnetMask, unsigned int ** addr);
}

#endif