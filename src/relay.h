#ifndef __RELAY_H__
#define __RELAY_H__

typedef struct
{
	const char *name;
	const int namePos;
	void(*pFunc)(int, char**);
	const char *help;
	const char *usage1;
	const char *usage2;
	const char *example;
} CliCmdType;


#endif //__RELAY_H__