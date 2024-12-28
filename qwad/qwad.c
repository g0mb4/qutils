/*
  WAD file utility for Quake
  
  based on: https://www.gamers.org/dEngine/quake/spec/quake-spec34/qkspec_7.htm#CWADF
  
  2024, Tóth János
*/

#define _CRT_SECURE_NO_WARNINGS
#include "cmdlib.h"

#include <stdint.h>

#define	MAX_WAD_ENTRIES	4096
#define	MAX_ENTRY_NAME	16

#define WAD_TYPE_COLOR_PALETTE	0x40
#define WAD_TYPE_STATUS_BAR		0x42
#define WAD_TYPE_MIP_TEXTURE	0x44
#define WAD_TYPE_CONSOLE_PIC	0x45

typedef struct
{
	char		magic[4];
	uint32_t	numentries;
	uint32_t	diroffset;
} wadhead_t;

typedef struct
{
	uint32_t offset;
	uint32_t dsize;
	uint32_t size;
	uint8_t type;
	uint8_t is_compressed;
	uint16_t dummy;
	char name[MAX_ENTRY_NAME];
} wadentry_t;

FILE	*fp;

wadhead_t		wad_header;
wadentry_t		wad_entries[MAX_WAD_ENTRIES];
int				wad_entry_ctr = 0;

void LoadWad (char *wadfile)
{
	uint32_t	i;
	wadentry_t	wad_entry;
	
	fp = SafeOpenRead (wadfile);
	SafeRead (fp, &wad_header, sizeof(wadentry_t));
	
	if (memcmp(wad_header.magic, "WAD2", 4) != 0)
		Error ("Not a WAD file");
	
	wad_header.numentries = LittleLong(wad_header.numentries);
	wad_header.diroffset = LittleLong(wad_header.diroffset);
	
	if (wad_header.numentries >= MAX_WAD_ENTRIES)
		Error ("Too many files");
	
	SafeSeek (fp, wad_header.diroffset);
	
	for (i=0 ; i<wad_header.numentries ; ++i)
	{
		SafeRead (fp, &wad_entry, sizeof(wadentry_t));
		
		wad_entry.offset = LittleLong(wad_entry.offset);
		wad_entry.dsize = LittleLong(wad_entry.dsize);
		wad_entry.size = LittleLong(wad_entry.size);

		wad_entries[wad_entry_ctr++] = wad_entry;
	}
	
	printf ("%s contains %d files.\n", wadfile, wad_header.numentries);
}

char *HumanReadableSize (uint32_t size)
{
	static char buffer[64];
	static const char* units[] = {"B", "kB", "MB", "GB"};
	double fsize = size;
	int i = 0;
	
	while (fsize > 1024)
	{
		fsize /= 1024;
		i++;
	}
	
	snprintf(buffer, sizeof(buffer) - 1, "%.2f %s", fsize, units[i]);
	return &buffer[0];
}

void List (qboolean human_readable)
{
	int i;
	int len;
	char ext[5];
	
	for (i=0 ; i<wad_entry_ctr ; ++i)
	{
		switch (wad_entries[i].type)
		{
			case WAD_TYPE_COLOR_PALETTE:	memcpy (ext, "PAL", 3); break;
			case WAD_TYPE_STATUS_BAR:		memcpy (ext, "STB", 3); break;
			case WAD_TYPE_MIP_TEXTURE:		memcpy (ext, "MTX", 3); break;
			case WAD_TYPE_CONSOLE_PIC:		memcpy (ext, "PIC", 3); break;
			default:						sprintf (ext, "%02XH", wad_entries[i].type);
		}
		
		if (wad_entries[i].is_compressed)
		{
			ext[3] = 'C';
			ext[4] = 0;
		}
		else
			ext[3] = 0;
		
		len = strlen (wad_entries[i].name);
		printf ("%-4s %s", ext, wad_entries[i].name);
		
		while (len++ < 53)
			putchar ('-');
		
		if (human_readable)
			printf ("%s\n", HumanReadableSize (wad_entries[i].dsize));
		else
			printf ("%d B\n", wad_entries[i].dsize);
	}
	
	exit (0);
}

char usage[] = "usage: qwad [options] wadfile\n"
			   "options:\n"
			   "-list\t\tlists the contents of wadfile\n"
			   "-listh\t\tlists the contents of wadfile with human readable sizes\n";

int main (int argc, char *argv[])
{
	int			i;
	char		*wadfile = NULL;
	
	qboolean	dolist = false;
	qboolean	humanlist = false;

	if (argc < 2) {
		Error (usage); 
	}
	
	for (i=1 ; i<argc ; ++i) {
		if (argv[i][0] != '-')
			break;
		else if (!strcmp (argv[i], "-list"))
			dolist = true;
		else if (!strcmp (argv[i], "-listh"))
		{
			dolist = true;
			humanlist = true;
		}
		else
			Error ("Unknown option '%s'\n%s", argv[i], usage);
	}
	
	wadfile = argv[i];
	if (!wadfile)
		Error ("wadfile is missing");
	
	LoadWad (wadfile);
	
	if (dolist)
		List (humanlist);
	
	return 0;
}
