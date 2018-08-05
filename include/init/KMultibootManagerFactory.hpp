#ifndef __KMULTIBOOTMANAGERFACTORY_HPP__
#define __KMULTIBOOTMANAGERFACTORY_HPP__

#include <init/KMultibootManager2.hpp>
#include <init/KIMultibootManager.hpp>

#include <stdint.h>
#include <stdlib.h>

class KMultibootManagerFactory
{
private:
	KMultibootManagerFactory() {}
	~KMultibootManagerFactory() {}

public:
	static KIMultibootManager *getMultibootMgr(uint32_t magic, void* mbi_address)
	{
		switch (magic)
		{
			case MB2_BTL_MAGIC: return new KMultibootManager2(mbi_address);
			/* Add here different bootloader managers (multiboot2 tested with grub2 only) */
			default : return (KIMultibootManager *)NULL;
		}
	}
};

extern KMultibootManagerFactory kmmf;

#endif /* __KMULTIBOOTMANAGERFACTORY_HPP__ */
