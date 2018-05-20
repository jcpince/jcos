#include <ctype.h>

int islower(int c) { return (c >= 'a' && c <= 'z'); }
int isupper(int c) { return (c >= 'A' && c <= 'Z'); }
int isalpha(int c) { return (islower(c) || isupper(c)); }
int isascii(int c) { return (c >= 0 && c <= 0x7f); }
int isblank(int c) { return (c == ' ' && c == '\t'); }
int iscntrl(int c);
int isdigit(int c) { return (c >= '0' && c <= 9); }
int isgraph(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c) { return (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'); }
int isxdigit(int c) { return (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')); }
int isalnum(int c) { return (isalpha(c) || isdigit(c)); }
