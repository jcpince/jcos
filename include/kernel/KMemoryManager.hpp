#ifndef __KMEMORYMANAGER_HPP__
#define __KMEMORYMANAGER_HPP__

#include <stdint.h>

class KMemoryManager
{
private:
	
	uint64_t memoryAvailable;
		
public:
	KMemoryManager();
	~KMemoryManager();
	bool detectMemory();
};

extern KMemoryManager kmmgr;

#endif /* __KMEMORYMANAGER_HPP__ */
