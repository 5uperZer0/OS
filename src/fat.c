#include "./fat.h"
#include "./fdc.h"


int openFile(file_t *file);
uint8 readByte(file_t *file, uint32 index);
int writeByte(file_t *file, uint8 byte, uint32 index);
void closeFile(file_t *file);
int createFile(file_t *file, directory_t *parent);
void deleteFile(file_t *file, directory_t *parent);

// FAT Copies
// First copy is fat0 stored at 
// Second copy is fat1
// There were issues declaring the FATs as non-pointers
// When they would get read from floppy, it would overwrite wrong areas of memory
fat_t *fat0;
fat_t *fat1;
void *startAddress = (void *) 0x20000;

// Initialize the file system
// Loads the FATs and root directory
void init_fs(directory_t *directory)
{
    // The FATs and directory are loaded into 0x20000, 0x21200, and 0x22400
    // These addresses were chosen because they are far enough away from the kernel (0x01000 - 0x07000)

    // Read the first copy of the FAT (Drive 0, Cluster 1, 512 bytes * 9 clusters)
    fat0 = (fat_t *) startAddress; // Put FAT at 0x20000
    floppy_read(0, 1,  (void *)fat0, sizeof(fat_t));

    // Read the second copy of the FAT (Drive 0, Cluster 10, 512 bytes * 9 clusters)
    fat1 = (fat_t *) startAddress+sizeof(fat_t); // Put FAT at 0x21200
    floppy_read(0, 10, (void *)fat1, sizeof(fat_t));

    // Read the root directory (Drive 0, Cluster 19, 512 bytes * 14 clusters)
    directory->startingAddress = (uint8 *) startAddress+(sizeof(fat_t)*2); // Put ROOT at 0x22400
    floppy_read(0, 19, (void *)directory->startingAddress, 512 * 14);

    directory->entry.filename[0] = 'R';
    directory->entry.filename[1] = 'O';
    directory->entry.filename[2] = 'O';
    directory->entry.filename[3] = 'T';
}

int openFile(file_t *file)
{

}

uint8 readByte(file_t *file, uint32 index)
{

}

int writeByte(file_t *file, uint8 byte, uint32 index)
{

}

void closeFile(file_t *file)
{

}

int createFile(file_t *file, directory_t *parent)
{

}

void deleteFile(file_t *file, directory_t *parent)
{
    
}