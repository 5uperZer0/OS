#include "./hal.h"
#include "./multitasking.h"
#include "./irq.h"
#include "./isr.h"
#include "./fat.h"

void prockernel();
void fileproc();
void inputWithPadding(char *string, int length);
void printFileName(directory_entry_t *entry);
int findFile(char *filename, char* ext, directory_t directory, directory_entry_t *foundEntry);
int stringcompare(char *string0, char *string1, int length);

int main() 
{
	// Clear the screen
	clearscreen();

	// Initialize our keyboard
	initkeymap();
	
	idt_install();
	isrs_install();
	irq_install();

	startkernel(prockernel);

	return 0;
}

void prockernel()
{
	createproc(fileproc, (void *) 0x10000);

	int userprocs = users_ready();

	while(userprocs > 0)
	{
		yield();
		userprocs = users_ready()
	}
	
	printf("OS Shutting Down...\n")
}

// The user processes

void fileproc()
{
	directory_t root;
	init_fs(&root);

	printf("Make a selection (c, d, r, w, q): ");
	char input = getchar();
	putchar(input);
	putchar('\n');

	while(input != 'q')
	{
		char filename[8];
		char ext[3];
		directory_entry_t *foundEntry = 0;

		printf("Enter filename: ");
		inputWithPadding(filename, 8);
		putchar('\n');

		printf("Enter extension: ");
		inputWithPadding(ext, 3);
		putchar('\n');

		int fileExists = findFile(filename, ext, root, foundEntry);

		if(fileExists)
		{
			if(input == 'c')
			{
				printf("Error: Tried to create a file that already exists!\n");
			}
			else if(input == 'd')
			{
				printf("Deleting File...\n");
				// Delete the file
				deleteFile()
			}
			else if(input == 'r')
			{
				printf("Reading File...\n");
			}
			else if(input == 'w')
			{
				printf("Writing File...\n");
			}
			else
			{
				printf("Error: Invalid input!\n");
			}
		}
		else
		{
			if(input == 'c')
			{
				printf("Creating File...\n");
				file_t file;

				file.entry.filename[0] = filename[0];
				file.entry.filename[1] = filename[1];
				file.entry.filename[2] = filename[2];
				file.entry.filename[3] = filename[3];
				file.entry.filename[4] = filename[4];
				file.entry.filename[5] = filename[5];
				file.entry.filename[6] = filename[6];
				file.entry.filename[7] = filename[7];
				
				file.entry.extension[0] = ext[0];
				file.entry.extension[1] = ext[1];
				file.entry.extension[2] = ext[2];

				createFile(&file, &root);
			}
			else if(input == 'd')
			{
				printf("Error: Tried deleting a file that doesn't exist!\n");
			}
			else if(input == 'r')
			{
				printf("Error: Tried reading a file that doesn't exist!\n");
			}
			else if(input == 'w')
			{
				printf("Error: Tried writing to a file that doesn't exist!\n");
			}
			else
			{
				printf("Error: Invalid input!\n");
			}
		}	

		printf("Make a selection (c, d, r, w, q): ");
		input = getchar();
		putchar(input);
		putchar('\n');
	}

	exit();
}

void inputWithPadding(char *string, int length)
{
	char nextChar = '-';
	for(int i = 0; i < length; i++)
	{
		if(nextChar != '\n' && nextChar != ' ')
		{
			nextChar = getchar();
			
		}

		if(nextChar == '\n')
		{
			nextChar = ' ';
		}

		putchar(nextChar);
		
		string[i] = nextChar;
	}
}

int findFile(char *filename, char* ext, directory_t directory, directory_entry_t *foundEntry)
{
	directory_entry_t *entry = (directory_entry_t *)directory.startingAddress;

	while(entry->filename[0] != 0)
	{
		int fileExists = stringcompare((char *)entry->filename, filename, 8) && stringcompare((char *)entry->extension, ext, 3);
		
		if(fileExists)
		{
			foundEntry = entry;
			return 1;
		}
		else
		{
			foundEntry = 0;
		}

		entry++;
	}

	return 0;
}

int stringcompare(char *string0, char *string1, int length)
{
	for(int i = 0; i < length; i++)
	{
		if(string0[i] != string1[i])
		{
			return 0;
		}
	}

	return 1;
}

void printFileName(directory_entry_t *entry)
{
	putchar((char)entry->filename[0]);
	putchar((char)entry->filename[1]);
	putchar((char)entry->filename[2]);
	putchar((char)entry->filename[3]);
	putchar((char)entry->filename[4]);
	putchar((char)entry->filename[5]);
	putchar((char)entry->filename[6]);
	putchar((char)entry->filename[7]);
	putchar('.');
	putchar((char)entry->extension[0]);
	putchar((char)entry->extension[1]);
	putchar((char)entry->extension[2]);
}