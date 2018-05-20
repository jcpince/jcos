void cppfunc0(void)
{
	unsigned char *videoram = (unsigned char *) 0xb8000;
	videoram[10] = 'C'; /* character 'A' */
	videoram[11] = 0x07; /* light grey (7) on black (0). */
	videoram[12] = 'C'; /* character 'A' */
	videoram[13] = 0x07; /* light grey (7) on black (0). */
	videoram[14] = 'C'; /* character 'A' */
	videoram[15] = 0x07; /* light grey (7) on black (0). */
	videoram[16] = 'C'; /* character 'A' */
	videoram[17] = 0x07; /* light grey (7) on black (0). */
	videoram[18] = 'C'; /* character 'A' */
	videoram[19] = 0x07; /* light grey (7) on black (0). */
}

