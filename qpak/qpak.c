/*
  PAK file utility for Quake
  
  based on: https://www.gamers.org/dEngine/quake/spec/quake-spec34/qkspec_3.htm#CPAKF
  
  2024, Tóth János
*/

#define _CRT_SECURE_NO_WARNINGS
#include "cmdlib.h"
#include <stdint.h>

typedef struct
{
	char		magic[4];
	uint32_t	diroffset;
	uint32_t	dirsize;
} pakheader_t;

typedef struct
{
	char		filename[56];
	uint32_t	offset;
	uint32_t	size;
} pakentry_t;


FILE	*fp;
char	file_base[1024];
char	file_ext[32];

pakheader_t		pak_header;
pakentry_t		pak_entries[4096];	// from qfiles.c
int				pak_entry_ctr = 0;

static void LoadPak (const char *pakfile)
{
	int			n;
	int			i, num_entries;
	pakentry_t	pak_entry;
	
	fp = fopen (pakfile, "rb");
	if (!fp)
		Error ("Unable to open %s: %s", pakfile, strerror(errno));
	
	n = fread (&pak_header, 1, sizeof(pakheader_t), fp);
	if (n != sizeof(pakheader_t))
		Error ("Unable to read header");
	
	if (memcmp(pak_header.magic, "PACK", 4) != 0)
		Error ("Not a PAK file");
	
	pak_header.diroffset = LittleLong(pak_header.diroffset);
	pak_header.dirsize = LittleLong(pak_header.dirsize);
	num_entries = pak_header.dirsize / sizeof(pakentry_t);
	
	if (num_entries >= (sizeof(pak_entries) / sizeof(pak_entries[0])))
		Error ("Too many files");
	
	ExtractFileBase (pakfile, file_base);
	ExtractFileExtension (pakfile, file_ext);
	
	if (fseek (fp, pak_header.diroffset, SEEK_SET) != 0)
		Error ("Seek error: %s", strerror(errno));
	
	for (i = 0; i < num_entries; ++i) {
		n = fread (&pak_entry, 1, sizeof(pakentry_t), fp);
		if (n != sizeof(pakentry_t))
			Error ("Unable to read entry %d", i);
		
		pak_entry.offset = LittleLong(pak_entry.offset);
		pak_entry.size = LittleLong(pak_entry.size);

		pak_entries[pak_entry_ctr++] = pak_entry;
	}
	
	fclose(fp);
}

int main (int argc, char *argv[])
{
	int			i;
	qboolean	dolist;
	char		*pakfile;
	
	dolist = false;
	
	if (argc < 2) {
		Error ("usage: qpak [options] pakfile\noptions: -list");
	}
	
	for (i=1 ; i<argc ; ++i) {
		if (argv[i][0] != '-')
			break;
		if (!strcmp (argv[i], "-list"))
			dolist = true;
		else
			Error ("Unknown option '%s'", argv[i]);
	}
	
	pakfile = argv[i];
	LoadPak (pakfile);
	
	printf ("%s.%s contains %d files.\n", file_base, file_ext, pak_entry_ctr);
	
	if (dolist)
		for (i=0 ; i<pak_entry_ctr ; ++i)
			printf ("%s - %u B\n", pak_entries[i].filename, pak_entries[i].size);
	
	return 0;
}