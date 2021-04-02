#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "relay.h"
#include "gpio.h"
#include "thread.h"

#define VERSION_BASE	(int)1
#define VERSION_MAJOR	(int)0
#define VERSION_MINOR	(int)0

#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#define CMD_ARRAY_SIZE	6
#define RELAY_CH_NR_MAX 4

const int relayGpio[RELAY_CH_NR_MAX] =
{
	22,
	23,
	25,
	24};

static void doHelp(int argc, char *argv[]);
const CliCmdType CMD_HELP =
	{
		"-h",
		1,
		&doHelp,
		"\t-h           Display the list of command options or one command option details\n",
		"\tUsage:       4relhat -h    Display command options list\n",
		"\tUsage:       4relhat -h <param>   Display help for <param> command option\n",
		"\tExample:     4relhat -h write    Display help for \"write\" command option\n"};

static void doVersion(int argc, char *argv[]);
const CliCmdType CMD_VERSION =
{
	"-v",
	1,
	&doVersion,
	"\t-v           Display the version number\n",
	"\tUsage:       4relhat -v\n",
	"",
	"\tExample:     4relhat -v  Display the version number\n"};

static void doWarranty(int argc, char* argv[]);
const CliCmdType CMD_WAR =
{
	"-warranty",
	1,
	&doWarranty,
	"\t-warranty    Display the warranty\n",
	"\tUsage:       4relhat -warranty\n",
	"",
	"\tExample:     4relhat -warranty  Display the warranty text\n"};

static void doRelayWrite(int argc, char *argv[]);
const CliCmdType CMD_WRITE =
{
	"write",
	1,
	&doRelayWrite,
	"\twrite:       Set relays On/Off\n",
	"\tUsage:       4relhat write <channel> <on/off>\n",
	"",
	"\tExample:     4relhat write 2 1; Set Relay #2 on Board #0 On\n"};

static void doRelayRead(int argc, char *argv[]);
const CliCmdType CMD_READ =
{
	"read",
	1,
	&doRelayRead,
	"\tread:        Read relays status\n",
	"\tUsage:       4relhat read <channel>\n",
	"",
	"\tExample:     4relhat read 2; Read Status of Relay #2 on Board #0\n"};

static void doTest(int argc, char* argv[]);
const CliCmdType CMD_TEST =
{
	"test",
	1,
	&doTest,
	"\ttest:        Turn ON and OFF the relays until press a key\n",
	"\tUsage:       4relhat test\n",
	"",
	"\tExample:     4relhat test\n"};

char *warranty =
	"	       Copyright (c) 2016-2021 Sequent Microsystems\n"
		"                                                             \n"
		"		This program is free software; you can redistribute it and/or modify\n"
		"		it under the terms of the GNU Leser General Public License as published\n"
		"		by the Free Software Foundation, either version 3 of the License, or\n"
		"		(at your option) any later version.\n"
		"                                    \n"
		"		This program is distributed in the hope that it will be useful,\n"
		"		but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
		"		GNU Lesser General Public License for more details.\n"
		"			\n"
		"		You should have received a copy of the GNU Lesser General Public License\n"
		"		along with this program. If not, see <http://www.gnu.org/licenses/>.";

const CliCmdType *gCmdArray[] =
{
	&CMD_VERSION,
	&CMD_HELP,
	&CMD_WAR,
	&CMD_WRITE,
	&CMD_READ,
	&CMD_TEST,
	NULL};

static void doRelayWrite(int argc, char *argv[])
{
	int pin = 0;
	int value = 0;

	if ( (argc != 4))
	{
		printf("%s", CMD_WRITE.usage1);
		exit(1);
	}
	pin = atoi(argv[2]);
	value = atoi(argv[3]);
	if (pin < 1 || pin > 4)
	{
		printf("! Invalid pin number [1..4]\n");
		exit(1);
	}
	if (value < 0)
	{
		value = 0;
	}
	if (value > 1)
	{
		value = 1;
	}
	if (0 != GPIOWrite(relayGpio[pin - 1], value))
	{
		printf("! Fail to write\n");
		exit(1);
	}
}

static void doRelayRead(int argc, char *argv[])
{
	int pin = 0;
	int value = 0;

	if ( (argc != 3))
	{
		printf("%s", CMD_READ.usage1);
		exit(1);
	}
	pin = atoi(argv[2]);
	if (pin < 1 || pin > 4)
	{
		printf("! Invalid pin number [1..4]\n");
		exit(1);
	}
	value = GPIORead(relayGpio[pin - 1]);
	if (-1 == value)
	{
		printf("! Fail to read\n");
		exit(1);
	}
	printf("%d\n", value);
}

static void doHelp(int argc, char *argv[])
{
	int i = 0;
	if (argc == 3)
	{
		while (gCmdArray[i] != NULL) // (i = 0; i < CMD_ARRAY_SIZE; i++)
		{
			if ( (gCmdArray[i]->name != NULL))
			{
				if (strcasecmp(argv[2], gCmdArray[i]->name) == 0)
				{
					printf("%s%s%s%s", gCmdArray[i]->help, gCmdArray[i]->usage1,
						gCmdArray[i]->usage2, gCmdArray[i]->example);
					break;
				}
			}
			i++;
		}
		if (CMD_ARRAY_SIZE == i)
		{
			printf("Option \"%s\" not found\n", argv[2]);
			i = 0;
					while (gCmdArray[i] != NULL)
					{
						printf("%s%s", gCmdArray[i]->usage1, gCmdArray[i]->usage2);
						i++;
					}
		}
	}
	else
	{
		i = 0;
		while (gCmdArray[i] != NULL)
		{
			printf("%s%s", gCmdArray[i]->usage1, gCmdArray[i]->usage2);
			i++;
		}
	}
}

static void doVersion(int argc, char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
	printf("4relhat v%d.%d.%d Copyright (c) 2016 - 2021 Sequent Microsystems\n",
	VERSION_BASE, VERSION_MAJOR, VERSION_MINOR);
	printf("\nThis is free software with ABSOLUTELY NO WARRANTY.\n");
	printf("For details type: 4relhat -warranty\n");

}

static void doWarranty(int argc UNU, char* argv[] UNU)
{
	printf("%s\n", warranty);
}

/* 
 * Self test for production
 */
static void doTest(int argc, char* argv[])
{
	int i = 0;
	FILE* file = NULL;
	int relayResult = 0;
	const int relayOrder[RELAY_CH_NR_MAX] =
	{
		1,
		2,
		3,
		4};


	if (argc == 3)
	{
		file = fopen(argv[2], "w");
		if (!file)
		{
			printf("Fail to open result file\n");
			//return -1;
		}
	}
//relay test****************************
	if (strcasecmp(argv[1], "test") == 0)
	{
		
		
		printf(
			"Are all relays and LEDs turning on and off in sequence?\nPress y for Yes or any key for No....");
		startThread();
		while (relayResult == 0)
		{
			for (i = 0; i < RELAY_CH_NR_MAX; i++)
			{
				relayResult = checkThreadResult();
				if (relayResult != 0)
				{
					break;
				}

				if (0 != GPIOWrite(relayGpio[relayOrder[i] - 1], 1))
				{
					printf("! Fail to set pin");
				}

				busyWait(150);
			}
			for (i = 0; i < RELAY_CH_NR_MAX; i++)
			{
				relayResult = checkThreadResult();
				if (relayResult != 0)
				{
					break;
				}

				if (0 != GPIOWrite(relayGpio[relayOrder[i] - 1], 0))
				{
					printf("! Fail to clear pin");
				}

				busyWait(150);
			}
		}
	}
	if (relayResult == YES)
	{
		if (file)
		{
			fprintf(file, "Relay Test ............................ PASS\n");
		}
		else
		{
			printf("Relay Test ............................ PASS\n");
		}
	}
	else
	{
		if (file)
		{
			fprintf(file, "Relay Test ............................ FAIL!\n");
		}
		else
		{
			printf("Relay Test ............................ FAIL!\n");
		}
	}
	if (file)
	{
		fclose(file);
	}
	for (i = 0; i < RELAY_CH_NR_MAX; i++)
	{
		if (0 != GPIOWrite(relayGpio[relayOrder[i] - 1], 0))
		{
			printf("! Fail to clear pin");
		}
	}
}

int main(int argc, char *argv[])
{
	int i = 0;

	if (argc == 1)
	{
		i = 0;
		while (gCmdArray[i] != NULL)
		{
			printf("%s%s", gCmdArray[i]->usage1, gCmdArray[i]->usage2);
			i++;
		}
		return 1;
	}
	i = 0;
	while (gCmdArray[i] != NULL)
	{
		if ( (gCmdArray[i]->name != NULL) && (gCmdArray[i]->namePos < argc))
		{
			if (strcasecmp(argv[gCmdArray[i]->namePos], gCmdArray[i]->name) == 0)
			{
				gCmdArray[i]->pFunc(argc, argv);
				return 0;
			}
		}
		i++;
	}
	printf("! Invalid command option\n");
	i = 0;
	while (gCmdArray[i] != NULL)
	{
		printf("%s%s", gCmdArray[i]->usage1, gCmdArray[i]->usage2);
		i++;
	}

	return 1;
}
