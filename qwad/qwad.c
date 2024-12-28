/*
  WAD file utility for Quake
  
  based on: https://www.gamers.org/dEngine/quake/spec/quake-spec34/qkspec_7.htm#CWADF
  and wadlib
  
  2024, Tóth János
*/

#define _CRT_SECURE_NO_WARNINGS
#include "cmdlib.h"

#ifdef WIN32
#include "dirent.h"
#else
#include <dirent.h>
#endif

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
	uint32_t disksize;
	uint32_t size;		// uncompressed
	uint8_t type;
	uint8_t is_compressed;
	uint16_t pad;
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
		wad_entry.disksize = LittleLong(wad_entry.disksize);
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

char *GetExtension (wadentry_t *wadentry)
{
	static char ext[5];
	
	switch (wadentry->type)
	{
		case WAD_TYPE_COLOR_PALETTE:	memcpy (ext, "PAL", 3); break;
		case WAD_TYPE_STATUS_BAR:		memcpy (ext, "STB", 3); break;
		case WAD_TYPE_MIP_TEXTURE:		memcpy (ext, "TEX", 3); break;
		case WAD_TYPE_CONSOLE_PIC:		memcpy (ext, "CON", 3); break;
		default:						sprintf (ext, "%02XH", wadentry->type);
	}
	
	if (wadentry->is_compressed)
	{
		ext[3] = 'C';
		ext[4] = 0;
	}
	else
		ext[3] = 0;
	
	return &ext[0];
}

void List (qboolean human_readable)
{
	int i;
	int len;
	
	for (i=0 ; i<wad_entry_ctr ; ++i)
	{
		len = strlen (wad_entries[i].name);
		printf ("%-4s %s", GetExtension (&wad_entries[i]), wad_entries[i].name);
		
		while (len++ < 53)
			putchar ('-');
		
		if (human_readable)
			printf ("%s\n", HumanReadableSize (wad_entries[i].disksize));
		else
			printf ("%d B\n", wad_entries[i].disksize);
	}
	
	exit (0);
}

wadentry_t *FindFile (char *filename)
{
	int i;
	
	for (i=0 ; i<wad_entry_ctr ; ++i)
	{
		if (!strcmp (wad_entries[i].name, filename))
		{
			return &wad_entries[i];
		}
	}
	
	return NULL;
}

void Check (char *filename)
{
	wadentry_t	*file;
	
	file = FindFile (filename);
	if (!file)
		Error ("%s is not found\n", filename);
	
	printf ("%s is found\n", filename);
	exit (0);
}

char *SafeFileName (char *filename)
{
	static char buffer[1024];
	int		i, n;
	int		len;
	
	n = 0;
	len = strlen(filename);
	for (i=0; i<len && filename[i] != 0; ++i)
	{
		char c = filename[i];
		if (isalnum (c) || c == '_')
			buffer[n++] = c;
		else
			n += snprintf(buffer + n, sizeof(buffer) - n, "!%02X", c);
		
	}
	buffer[n] = 0;
	
	return &buffer[0];
}

void Extract (wadentry_t *file, char *dir)
{
	char		*file_ext;
	char		extracted_filename[2048];	/* maybe? */
	char 		path[2048];
	FILE		*efp;
	void		*buffer;
	
	buffer = malloc(file->disksize);
	if (!buffer)
		Error ("Unable to allocate buffer");
	
	SafeSeek (fp, file->offset);

	SafeRead (fp, buffer, file->disksize);
	
	file_ext = GetExtension (file);
	
	if (dir) 
	{
		snprintf (extracted_filename, sizeof(extracted_filename), 
				  "%s/%s.%s", dir, SafeFileName (file->name), file_ext);
		
		ExtractFilePath (extracted_filename, path);
		CreatePath (path);
	}
	else
	{
		snprintf (extracted_filename, sizeof(extracted_filename), 
				  "%s.%s", SafeFileName (file->name), file_ext);
	}
	
	efp = SafeOpenWrite (extracted_filename);
	SafeWrite (efp, buffer, file->disksize);
	fclose (efp);
	
	free (buffer);
	
	printf ("%s is extracted\n", extracted_filename);
}

void ExtractFile (char *filename)
{
	wadentry_t	*file;

	file = FindFile (filename);
	if (!file)
		Error ("%s is not found", filename);
	
	Extract (file, NULL);
	
	exit (0);
}

void ExtractAll (char *dir)
{
	int i;
	
	for (i=0 ; i<wad_entry_ctr ; ++i)
		Extract (&wad_entries[i], dir);
	
	exit (0);
}

char CharFromNibble(char nibble)
{
	if (nibble >= '0' && nibble <= '9')
		return nibble - '0';
	else if (nibble >= 'A' && nibble <= 'F')
		return nibble - 'A' + 10;
	else 
	{
		Error ("Invalid nibble: %c", nibble);
		return 0;	/* to pacify the compiler */
	}
}

char CharFromHex(char high, char low)
{
	return CharFromNibble(high) << 4 | CharFromNibble(low);
}

void EntryFromFilename (wadentry_t *file, char *filename)
{
	char	buffer[MAX_ENTRY_NAME];
	int		i;
	int		n;
	int		len;
	char	*ext;
	
	len = strlen(filename);
	n = 0;
	for (i=0; i<len && filename[i] != '.' && i<(MAX_ENTRY_NAME - 1) ; )
	{
		if (filename[i] == '!')
		{
			buffer[n++] = CharFromHex(filename[i + 1], filename[i + 2]);
			i += 3;
		}
		else
		{
			buffer[n++] = filename[i];
			i += 1;
		}
	}
	buffer[n] = 0;
	
	memcpy(file->name, buffer, i);
	file->name[i] = 0;
	
	ext = &filename[i + 1];
	if (!strncmp(ext, "PAL", 3))
		file->type = WAD_TYPE_COLOR_PALETTE;
	else if (!strncmp(ext, "STB", 3))
		file->type = WAD_TYPE_STATUS_BAR;
	else if (!strncmp(ext, "TEX", 3))
		file->type = WAD_TYPE_MIP_TEXTURE;
	else if (!strncmp(ext, "CON", 3))
		file->type = WAD_TYPE_CONSOLE_PIC;
	else if (ext[2] == 'H')
		file->type = CharFromHex(ext[0], ext[1]);
	else
		Error ("%s: Unknown wad file extension", filename);
	
	if (ext[3] == 'C') {
		Error ("Compressed files are not supported");
	}
}

void CopyFromDirectory(FILE *wadf, char *basepath)
{
	char			path[2048];
	struct dirent	*dp;
	DIR				*dir;
	wadentry_t		entry;
	FILE			*entryf;
	char			*entry_path;
	void			*buffer;

	dir = opendir (basepath);
	if (!dir)
		Error ("Unable to open %s", basepath);

	while ((dp = readdir (dir)) != NULL)
	{
		if (!strcmp (dp->d_name, ".") || !strcmp (dp->d_name, ".."))
			continue;
		
		if (dp->d_type != DT_REG)
			Error ("%s: Invalid type: %d\n", dp->d_name, dp->d_type);
		
		snprintf (path, sizeof(path) - 1, "%s/%s", basepath, dp->d_name);
		
		entry_path = RemoveFirstDirFromPath (path);
		entryf = SafeOpenRead (path);
		
		EntryFromFilename (&entry, entry_path);
		
		entry.offset = filelength (wadf);
		entry.disksize = filelength (entryf);
		entry.size = entry.disksize;
		entry.is_compressed = 0;	/* NOTE(gmb): Compressed files are not supported */
		
		buffer = malloc (entry.size);
		if (!buffer)
			Error ("Unable to alloclate buffer");
		
		SafeRead (entryf, buffer, entry.size);
		fclose (entryf);
		
		SafeSeek (wadf, entry.offset);
		SafeWrite (wadf, buffer, entry.size);
		
		free (buffer);
		
		if (wad_entry_ctr >= MAX_WAD_ENTRIES)
			Error ("Too many files");
		
		wad_entries[wad_entry_ctr++] = entry;
		
		printf ("%s added\n", entry.name);
	}

	closedir (dir);
}

void Create (char *wadfile, char *dir)
{
	FILE *wadf;
	
	memcpy(wad_header.magic, "WAD2", 4);
	wad_header.diroffset = 0;
	wad_header.numentries = 0;
	
	wad_entry_ctr = 0;
	
	wadf = SafeOpenWrite (wadfile);
	SafeWrite (wadf, &wad_header, sizeof(wadhead_t));
	
	CopyFromDirectory (wadf, dir);
	
	wad_header.diroffset = filelength (wadf);
	wad_header.numentries = wad_entry_ctr;
	
	SafeSeek (wadf, wad_header.diroffset);
	SafeWrite (wadf, &wad_entries[0], wad_entry_ctr * sizeof(wadentry_t));
	
	SafeSeek (wadf, 0);
	SafeWrite (wadf, &wad_header, sizeof(wadhead_t));
	
	fclose (wadf);
	
	printf ("%s created, it contains %d files.\n", wadfile, wad_entry_ctr);
	
	exit (0);
}

char usage[] = "usage: qwad [options] wadfile\n"
			   "options:\n"
			   "-list\t\tlists the contents of wadfile\n"
			   "-listh\t\tlists the contents of wadfile with human readable sizes\n"
			   "-check file\tchecks if wadfile contains file\n"
			   "-extract file\textracts file from wadfile\n"
			   "-extractall dir\textracts the contents of wadfile into dir\n"
			   "-create dir\tcreates wadfile from the contents of dir\n";

int main (int argc, char *argv[])
{
	int			i;
	char		*wadfile = NULL;
	
	qboolean	dolist = false;
	qboolean	humanlist = false;
	qboolean	docheck = false;
	qboolean	doextract = false;
	qboolean	doextractall = false;
	qboolean	docreate = false;
	
	char		*file = NULL;
	char		*dir = NULL;

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
		else if (!strcmp (argv[i], "-check"))
		{
			docheck = true;
			file = argv[++i];
			
			if (!file)
				Error ("Invalid file");
		}
		else if (!strcmp (argv[i], "-extract"))
		{
			doextract = true;
			file = argv[++i];
			
			if (!file)
				Error ("Invalid file");
		}
		else if (!strcmp (argv[i], "-extractall"))
		{
			doextractall = true;
			dir = argv[++i];
			
			if (!dir)
				Error ("Invalid directory");
		}
		else if (!strcmp (argv[i], "-create"))
		{
			docreate = true;
			dir = argv[++i];
			
			if (!dir)
				Error ("Invalid directory");
		}
		else
			Error ("Unknown option '%s'\n%s", argv[i], usage);
	}
	
	wadfile = argv[i];
	if (!wadfile)
		Error ("wadfile is missing");
	
	if (!docreate)
		LoadWad (wadfile);
	
	if (dolist)
		List (humanlist);
	else if (docheck)
		Check (file);
	else if (doextract)
		ExtractFile (file);
	else if (doextractall)
		ExtractAll (dir);
	else if (docreate)
		Create (wadfile, dir);
	
	return 0;
}
