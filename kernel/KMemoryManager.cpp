#include <kernel/KMemoryManager.hpp>
#include <OStream.h>
#include <stdio.h>

using std::kout;
using std::endl;

KMemoryManager kmmgr;

KMemoryManager::KMemoryManager()
{
	memoryAvailable = 0;
}

KMemoryManager::~KMemoryManager()
{

}

typedef struct SMAP_entry {
 
	uint32_t BaseL; // base address QWORD
	uint32_t BaseH;
	uint32_t LengthL; // length QWORD
	uint32_t LengthH;
	uint16_t Type; // entry Type
	uint16_t ACPI; // exteded
 
}__attribute__((packed)) SMAP_entry_t;
 
// load memory map to buffer - note: regparm(3) avoids stack issues with gcc in realmode
int32_t __attribute__((noinline)) __attribute__((regparm(3))) detectMemoryC(SMAP_entry_t* buffer, uint32_t maxentries)
{
	uint32_t contID = 0;
	uint32_t entries = 0, signature, bytes;
	
	__asm__(".code16gcc\n");
	
	do 
	{
		__asm__ __volatile__ ("int  $0x15" 
			: "=a"(signature), "=c"(bytes), "=b"(contID)
			: "a"(0xE820), "b"(contID), "c"(24), "d"(0x534D4150), "D"(buffer));
		if (signature != 0x534D4150) 
			return -1; // error
		if (bytes > 20 && (buffer->ACPI & 0x0001) == 0)
		{
			// ignore this entry
		}
		else {
			buffer++;
			entries++;
		}
	}
	while (contID != 0 && entries < maxentries);
	return entries;
}

bool KMemoryManager::detectMemory()
{
	SMAP_entry_t* smap = (SMAP_entry_t*) 0x1000;
	const uint32_t smap_size = 0x2000;
	char tmpbuf[24];
 
	int32_t entry_count = detectMemoryC(smap, smap_size / sizeof(SMAP_entry_t));
 
	if (entry_count < 0) {
		// error - halt system and/or show error message
		kout << _L("Was not able to detect the memory map...") << endl;
		return false;
	}
	for (uint32_t idx = 0 ; idx < (uint32_t)entry_count ; idx++)
	{
		kout << _L("Memory detected: slot[") << idx << _L("]:") << endl;
		kout << _L("\tBaseL: ") << sprintf(tmpbuf, "0x%x", smap[idx].BaseL) << endl;
		kout << _L("\tBaseH: ") << sprintf(tmpbuf, "0x%x", smap[idx].BaseH) << endl;
		kout << _L("\tLengthL: ") << sprintf(tmpbuf, "0x%x", smap[idx].LengthL) << endl;
		kout << _L("\tLengthH: ") << sprintf(tmpbuf, "0x%x", smap[idx].LengthH) << endl;
		kout << _L("\tType: ") << sprintf(tmpbuf, "0x%x", smap[idx].Type) << endl;
		kout << _L("\tACPI: ") << sprintf(tmpbuf, "0x%x", smap[idx].ACPI) << endl;
	}
	return true;	
}

