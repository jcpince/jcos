#ifndef VIDEO_H
#define VIDEO_H

#include <asmf.H>

//The different colours that can be set. Only the first 7 are relevent for background
enum Colours {BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, WHITE, DARKGRAY,
	BRIGHTBLUE, BRIGHTGREEN, BRIGHTCYAN, PINK, BRIGHTMAGENTA, YELLOW, BRIGHTWHITE};

class Video	//Video class
{
	public:
		Video() ;
		~Video() ;
		void clear() ;
		void write(const char *cp) ;
		void write(char *cp) ;
		void put(char c) ;
		void gotoxy(unsigned x, unsigned y) ;
		void SetColour(Colours Text, Colours Back, bool blink);
		unsigned char GetBackColour();
		unsigned char GetTextColour();
		void SetBackColour(Colours col);
		void SetTextColour(Colours col);

	private:
		void scrollup() ;
		void setcursor(unsigned x, unsigned y);

		unsigned short *videomem ;		//pointer to video memory
		unsigned short crtc_mem;
		unsigned int ypos ;				//offset, used like a y cord
		unsigned int xpos ;				//position, used like x cord
		unsigned int scrHeight ;
		unsigned int scrWidth ;
		unsigned char colour ;			//Text colours
		unsigned char text ;
		unsigned char back ;
};


#endif
