#include <myclass.hpp>

Myclass::Myclass()
{
	mypriv_data = 0;
	myvideoidx = 0;
}

Myclass::~Myclass()
{
	mypriv_data--;
}
	
void Myclass::test(char c)
{
	//unsigned int idx;
	unsigned char *videoram = (unsigned char *) 0xb8000;
	
	mypriv_data++;
	/*for (idx = 0 ; idx < 0xffffff ; idx++)
		mypriv_data++;*/
	
		
	videoram[myvideoidx++] = c; /* character 'A' */
	videoram[myvideoidx++] = 0x07; /* light grey (7) on black (0). */
	videoram[myvideoidx++] = c; /* character 'A' */
	videoram[myvideoidx++] = 0x07; /* light grey (7) on black (0). */
	videoram[myvideoidx++] = c; /* character 'A' */
	videoram[myvideoidx++] = 0x07; /* light grey (7) on black (0). */
	videoram[myvideoidx++] = c; /* character 'A' */
	videoram[myvideoidx++] = 0x07; /* light grey (7) on black (0). */
	videoram[myvideoidx++] = c; /* character 'A' */
	videoram[myvideoidx++] = 0x07; /* light grey (7) on black (0). */
}
	
void Myclass::test_throw()
{
	mypriv_data++;
	throw 0;
}

void Myclass::priv_test()
{
	mypriv_data+=2;
}

