//gb2gba, a command line alternative

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

#include "goomba.h"
#include "goombacolor.h"


enum RomType {
	ROM_TYPE_NULL,
	ROM_TYPE_GB,
	ROM_TYPE_GBC,
	ROM_TYPE_SGB
};

enum Emulator {
	EMULATOR_NULL,
	EMULATOR_GOOMBA,
	EMULATOR_GOOMBA_COLOR
};


static const char* ROM_STRINGS[] = {
	"NULL",
	"gb",
	"gbc",
	"sgb"
};

static const char* EMU_STRINGS[] = {
	"NULL",
	"Goomba",
	"Goomba Color",
	"Goomba Color"
};


static RomType getRomType(char* fname);
static Emulator getEmulator(RomType romType);

static bool writeEmulator(FILE* f, Emulator e);
static bool writeRom(FILE* fout, FILE* fin);

static bool getFileName(char* in, char* out);
static bool getOutPath(char* in, char* out);


int main(int argc, char* argv[])
{	
	//No arguments specified
	if (argc <= 1)
		printf("Drag and drop a rom file onto gb2gba.\n");
	
	else
	{
		//Repeat for each argument
		int arg = 1;
		for (; arg < argc; arg++)
		{
			printf("------------------------------\n");

			FILE* inFile = NULL;
			FILE* outFile = NULL;
			
			//Get full file path
			char* filePath = argv[arg];

			//Get file name from path
			char fileName[512];
			getFileName(filePath, fileName);

			printf("Converting: %s\n", fileName);
			
			//Check if file is supported
			RomType romType = getRomType(fileName);
			Emulator emulator = getEmulator(romType);

			//Print rom and emu type
			printf("Rom Type: %s\n", ROM_STRINGS[romType]);
			printf("Emulator: %s\n", EMU_STRINGS[emulator]);

			//Error: unsupported file type
			if (emulator == EMULATOR_NULL)
			{
				printf("Error - Use a .gb, .gbc or .sgb file.\n");
				continue;
			}
			
			//Open rom file
			inFile = fopen(filePath, "rb");

			if (!inFile)
			{
				printf("Error - Could not open %s\n", filePath);
			}			
			else
			{
				//Create output file
				{
					char outPath[512];
					getOutPath(filePath, outPath);

					outFile = fopen(outPath, "wb");
				}

				//
				if (!outFile)
				{
					printf("Error - Could not create output file.\n");
				}				
				else
				{
					if (writeEmulator(outFile, emulator) == false)
					{
						printf("Error - Could not write emulator data.\n");
					}
					else
					{
						if (writeRom(outFile, inFile) == false)
						{
							printf("Error - Could not write rom data.\n");
						}
						else
						{
							printf("Success!\n");
						}						
					}
				}
			}
			
			fclose(outFile);
			fclose(inFile);
		}
	}
	
	return 0;
}


static bool strToLower(char* in, char* out)
{
	if (in == NULL || out == NULL)
		return false;
	
	int len = strlen(in);
	for (int i = 0; i < len; i++)
	{
		out[i] = tolower(in[i]);
	}

	return true;
}

static RomType getRomType(char* fname)
{
	char lowPath[512];
	strToLower(fname, lowPath);

	RomType romType = ROM_TYPE_NULL;
	char* periodPtr = strrchr(lowPath, '.');
	
	if (periodPtr != NULL)
	{
		int periodPos = periodPtr - lowPath;
		
		if (strstr(lowPath, ".gbc") != NULL)
			romType = ROM_TYPE_GBC;

		else if (strstr(lowPath, ".gb") != NULL)
			romType = ROM_TYPE_GB;
		
		else if (strstr(lowPath, ".sgb") != NULL)
			romType = ROM_TYPE_SGB;
	}
	
	return romType;
}

static Emulator getEmulator(RomType romType)
{
	switch (romType)
	{
		case ROM_TYPE_GB:
			return EMULATOR_GOOMBA;
			break;

		case ROM_TYPE_GBC:
		case ROM_TYPE_SGB:
			return EMULATOR_GOOMBA_COLOR;
			break;
	}
	
	return EMULATOR_NULL;
}

static bool writeEmulator(FILE* f, Emulator e)
{
	unsigned char* emuData = NULL;
	int emuDataSize = 0;
					
	switch (e)
	{
		case EMULATOR_GOOMBA:
			emuData = goombaData;
			emuDataSize = GOOMBA_SIZE;
			break;

		case EMULATOR_GOOMBA_COLOR:
			emuData = goombaColorData;
			emuDataSize = GOOMBACOLOR_SIZE;
			break;

		default:
			return false;
	}
	
	if (fwrite(emuData, emuDataSize, 1, f) != 1)
		return false;

	return true;
}

static bool writeRom(FILE* fout, FILE* fin)
{
	bool result = true;

	const int BUF_SIZE = (1024*1024*4);
	unsigned char* buffer = (unsigned char*)malloc(BUF_SIZE);
	int bytesRead = 0;

	fseek(fin, 0, SEEK_SET);

	do
	{
		bytesRead = fread(buffer, 1, BUF_SIZE, fin);

		if (fwrite(buffer, bytesRead, 1, fout) != 1)
		{
			//Error
			result = false;
			break;
		}
	}
	while (bytesRead == BUF_SIZE);

	free(buffer);

	return result;
}

static bool getFileName(char* in, char* out)
{	
	if (in == NULL || out == NULL)
		return false;

	//Get last slash
	char* slashPtr = strrchr(in, '\\');
	int slashPos = 0;
	
	if (slashPtr != NULL)	
		slashPos = slashPtr + 1 - in;

	int fnameLength = strlen(in) - slashPos;
	out[fnameLength] = '\0';

	for (int i = 0; i < fnameLength; i++)
		out[i] = in[slashPos + i];
	
//	printf("getFileName(%s) -> %s\n", in, out);
	return true;
}

static bool getOutPath(char* in, char* out)
{
	if (in == NULL || out == NULL)
		return false;

//	sprintf(out, "./%s", in);

	int extentionPos = strchr(in, '.') - in;
					
	for (int i = 0; i < extentionPos + 1; i++)
	{
		out[i] = in[i];
	}
					
	char newExtension[] = "gba\0";
	memcpy(out + extentionPos + 1, newExtension, strlen(newExtension) + 1);

	return true;
}