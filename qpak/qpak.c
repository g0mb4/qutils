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

pakheader_t		pak_header;
pakentry_t		pak_entries[4096];	// from qfiles.c
int				pak_entry_ctr = 0;

void LoadPak (char *pakfile)
{
	int			i;
	int			num_entries;
	pakentry_t	pak_entry;
	
	fp = SafeOpenRead (pakfile);
	SafeRead (fp, &pak_header, sizeof(pakheader_t));
	
	if (memcmp(pak_header.magic, "PACK", 4) != 0)
		Error ("Not a PAK file");
	
	pak_header.diroffset = LittleLong(pak_header.diroffset);
	pak_header.dirsize = LittleLong(pak_header.dirsize);
	num_entries = pak_header.dirsize / sizeof(pakentry_t);
	
	if (num_entries >= (sizeof(pak_entries) / sizeof(pak_entries[0])))
		Error ("Too many files");
	
	if (fseek (fp, pak_header.diroffset, SEEK_SET) != 0)
		Error ("Seek error: %s", strerror(errno));
	
	for (i = 0; i < num_entries; ++i)
	{
		SafeRead (fp, &pak_entry, sizeof(pakentry_t));
		
		pak_entry.offset = LittleLong(pak_entry.offset);
		pak_entry.size = LittleLong(pak_entry.size);

		pak_entries[pak_entry_ctr++] = pak_entry;
	}
}

void List (void)
{
	int i;
	
	for (i=0 ; i<pak_entry_ctr ; ++i)
		printf ("%s - %u B\n", pak_entries[i].filename, pak_entries[i].size);
	
	exit (0);
}

pakentry_t *FindFile (char *filename)
{
	int i;
	
	for (i=0 ; i<pak_entry_ctr ; ++i)
	{
		if (!strcmp (pak_entries[i].filename, filename))
		{
			return &pak_entries[i];
		}
	}
	
	return NULL;
}

void Check (char *filename)
{
	pakentry_t	*file;
	
	file = FindFile (filename);
	
	if (!file)
		Error ("%s is not found\n", filename);
	
	printf ("%s is found\n", filename);
	exit (0);
}

void Extract (char *filename)
{
	pakentry_t	*file;
	/* file name is max 56 chars */
	char		file_base[64];
	char		file_ext[64];
	char		extracted_filename[64];
	FILE		*efp;
	void		*buffer;
	
	file = FindFile (filename);
	
	if (!file)
		Error ("%s is not found", filename);
	
	buffer = malloc(file->size);
	if (!buffer)
		Error ("Unable to allocate buffer");
	
	if (fseek (fp, file->offset, SEEK_SET) != 0)
		Error ("Seek error: %s", strerror(errno));
	
	SafeRead (fp, buffer, file->size);
	
	ExtractFileBase (filename, file_base);
	ExtractFileExtension (filename, file_ext);
	
	snprintf (extracted_filename, sizeof(extracted_filename), 
			  "%s.%s", file_base, file_ext);
			  
	efp = SafeOpenWrite (extracted_filename);
	SafeWrite (efp, buffer, file->size);
	fclose (efp);
	
	printf ("%s is extracted\n", extracted_filename);
	
	exit (0);
}

char usage[] = "usage: qpak [options] pakfile\n"
			   "options:\n"
			   "-list\t\tlists the content of pakfile\n"
			   "-check file\t\tchecks if pakfile contains file\n"
			   "-extract file\textracts file without directory\n";

int main (int argc, char *argv[])
{
	int			i;
	char		*pakfile;
	
	qboolean	dolist;
	qboolean	docheck;
	qboolean	doextract;
	char		*file;
	
	dolist = false;
	docheck = false;
	doextract = false;

	if (argc < 2) {
		Error (usage); 
	}
	
	for (i=1 ; i<argc ; ++i) {
		if (argv[i][0] != '-')
			break;
		else if (!strcmp (argv[i], "-list"))
			dolist = true;
		else if (!strcmp (argv[i], "-check"))
		{
			docheck = true;
			file = argv[++i];
		}
		else if (!strcmp (argv[i], "-extract"))
		{
			doextract = true;
			file = argv[++i];
		}
		else
			Error ("Unknown option '%s'\n%s", argv[i], usage);
	}
	
	pakfile = argv[i];
	LoadPak (pakfile);
	
	printf ("%s contains %d files.\n", pakfile, pak_entry_ctr);
	
	if (dolist)
		List ();
	else if (docheck)
		Check (file);
	else if (doextract)
		Extract (file);
	
	return 0;
}