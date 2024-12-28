/*
  PAK file utility for Quake
  
  based on: https://www.gamers.org/dEngine/quake/spec/quake-spec34/qkspec_3.htm#CPAKF
  
  2024, Tóth János
*/

#define _CRT_SECURE_NO_WARNINGS
#include "cmdlib.h"

#ifdef WIN32
#include "dirent.h"
#else
#include <sys/stat.h>
#include <dirent.h>
#endif

#include <stdint.h>

#define	MAX_PAK_ENTRIES	4096	// from qfiles.c
#define	MAX_ENTRY_NAME	56

typedef struct
{
	char		magic[4];
	uint32_t	diroffset;
	uint32_t	dirsize;
} pakheader_t;

typedef struct
{
	char		filename[MAX_ENTRY_NAME];
	uint32_t	offset;
	uint32_t	size;
} pakentry_t;

FILE	*fp;

pakheader_t		pak_header;
pakentry_t		pak_entries[MAX_PAK_ENTRIES];
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
	
	if (num_entries >= MAX_PAK_ENTRIES)
		Error ("Too many files");
	
	SafeSeek (fp, pak_header.diroffset);
	
	for (i=0 ; i<num_entries ; ++i)
	{
		SafeRead (fp, &pak_entry, sizeof(pakentry_t));
		
		pak_entry.offset = LittleLong(pak_entry.offset);
		pak_entry.size = LittleLong(pak_entry.size);

		pak_entries[pak_entry_ctr++] = pak_entry;
	}
	
	printf ("%s contains %d files.\n", pakfile, pak_entry_ctr);
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
	
	for (i=0 ; i<pak_entry_ctr ; ++i)
	{
		len = strlen (pak_entries[i].filename);
		printf ("%s", pak_entries[i].filename);
		
		while (len++ < 56)
			putchar ('-');
		
		if (human_readable)
			printf ("%s\n", HumanReadableSize (pak_entries[i].size));
		else
			printf ("%d B\n", pak_entries[i].size);
	}
	
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

void Extract (pakentry_t *file, char *dir)
{
	/* file name is max 56 chars */
	char		file_base[64];
	char		file_ext[64];
	char		extracted_filename[2048];	/* maybe? */
	char 		path[2048];
	FILE		*efp;
	void		*buffer;
	
	buffer = malloc(file->size);
	if (!buffer)
		Error ("Unable to allocate buffer");
	
	SafeSeek (fp, file->offset);

	SafeRead (fp, buffer, file->size);
	
	if (dir) 
	{
		snprintf (extracted_filename, sizeof(extracted_filename), 
				  "%s/%s", dir, file->filename);
		
		ExtractFilePath (extracted_filename, path);
		CreatePath (path);
	}
	else
	{
		ExtractFileBase (file->filename, file_base);
		ExtractFileExtension (file->filename, file_ext);
	
		snprintf (extracted_filename, sizeof(extracted_filename), 
				  "%s.%s", file_base, file_ext);
	}
	
	efp = SafeOpenWrite (extracted_filename);
	SafeWrite (efp, buffer, file->size);
	fclose (efp);
	
	free (buffer);
	
	printf ("%s is extracted\n", extracted_filename);
}

void ExtractFile (char *filename)
{
	pakentry_t	*file;

	file = FindFile (filename);
	
	if (!file)
		Error ("%s is not found", filename);
	
	Extract (file, NULL);
	
	exit (0);
}

void ExtractAll (char *dir)
{
	int i;
	
	for (i=0 ; i<pak_entry_ctr ; ++i)
		Extract (&pak_entries[i], dir);
	
	exit (0);
}

char *RemoveFirstDirFromPath (char *path)
{
	char *src = path;
	
	while (*src != '/' && *src != '\\')
		++src;
	
	return src + 1;
}

void CopyFromDirectory(FILE *pakf, char *basepath)
{
	char			path[2048];
	struct dirent	*dp;
	DIR				*dir;
	pakentry_t		entry;
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
		
		if (dp->d_type == DT_REG)
		{
			snprintf (path, sizeof(path) - 1, "%s/%s", basepath, dp->d_name);
			
			entry_path = RemoveFirstDirFromPath (path);
			entryf = SafeOpenRead (path);
			
			strncpy (entry.filename, entry_path, MAX_ENTRY_NAME - 1);
			entry.offset = filelength (pakf);
			entry.size = filelength (entryf);
			
			buffer = malloc (entry.size);
			if (!buffer)
				Error ("Unable to alloclate buffer");
			
			SafeRead (entryf, buffer, entry.size);
			fclose (entryf);
			
			SafeSeek (pakf, entry.offset);
			SafeWrite (pakf, buffer, entry.size);
			
			free (buffer);
			
			if (pak_entry_ctr >= MAX_PAK_ENTRIES)
				Error ("Too many files");
			
			pak_entries[pak_entry_ctr++] = entry;
			
			printf ("%s added\n", entry_path);
		}
		else if (dp->d_type == DT_DIR)
		{
			strcpy (path, basepath);
			strcat (path, "/");
			strcat (path, dp->d_name);

			CopyFromDirectory (pakf, path);
		}
		else
			Error ("%s: Invalid type: %d\n", dp->d_name, dp->d_type);
	}

	closedir (dir);
}

void Create (char *pakfile, char *dir)
{
	FILE *pakf;
	
	memcpy(pak_header.magic, "PACK", 4);
	pak_header.diroffset = 0;
	pak_header.dirsize = 0;
	
	pak_entry_ctr = 0;
	
	pakf = SafeOpenWrite (pakfile);
	SafeWrite (pakf, &pak_header, sizeof(pakheader_t));
	
	CopyFromDirectory (pakf, dir);
	
	pak_header.diroffset = filelength (pakf);
	pak_header.dirsize = pak_entry_ctr * sizeof(pakentry_t);
	
	SafeSeek (pakf, pak_header.diroffset);
	SafeWrite (pakf, &pak_entries[0], pak_header.dirsize);
	
	SafeSeek (pakf, 0);
	SafeWrite (pakf, &pak_header, sizeof(pakheader_t));
	
	fclose (pakf);
	
	printf ("%s created, it contains %d files.\n", pakfile, pak_entry_ctr);
	
	exit (0);
}

char usage[] = "usage: qpak [options] pakfile\n"
			   "options:\n"
			   "-list\t\tlists the contents of pakfile\n"
			   "-listh\t\tlists the contents of pakfile with human readable sizes\n"
			   "-check file\tchecks if pakfile contains file\n"
			   "-extract file\textracts file from pakfile without a directory structure\n"
			   "-extractall dir\textracts the contents of pakfile into dir\n"
			   "               \twhile keeping the directory structure\n"
			   "-create dir\tcreates pakfile from the contents of dir\n";

int main (int argc, char *argv[])
{
	int			i;
	char		*pakfile = NULL;
	
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
			
			if(!file)
				Error ("Invalid file");
		}
		else if (!strcmp (argv[i], "-extract"))
		{
			doextract = true;
			file = argv[++i];
			
			if(!file)
				Error ("Invalid file");
		}
		else if (!strcmp (argv[i], "-extractall"))
		{
			doextractall = true;
			dir = argv[++i];
			
			if(!dir)
				Error ("Invalid directory");
		}
		else if (!strcmp (argv[i], "-create"))
		{
			docreate = true;
			dir = argv[++i];
			
			if(!dir)
				Error ("Invalid directory");
		}
		else
			Error ("Unknown option '%s'\n%s", argv[i], usage);
	}
	
	pakfile = argv[i];
	if (!pakfile)
		Error ("pakfile is missing");
	
	if (!docreate)
		LoadPak (pakfile);
	
	if (dolist)
		List (humanlist);
	else if (docheck)
		Check (file);
	else if (doextract)
		ExtractFile (file);
	else if (doextractall)
		ExtractAll (dir);
	else if (docreate)
		Create (pakfile, dir);
	
	return 0;
}
