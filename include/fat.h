#include "./types.h"

typedef struct
{
    // FAT12 Bios Parameter Block
    uint8   jumpInstruction[3];
    uint8   oem[8];
    uint16  bytesPerSector;
    uint8   sectorsPerCluster;
    uint16  ReservedSectors;
    uint8   fatCount;
    uint16  rootDirectoryEntries;
    uint16  sectorCount;
    uint8   mediaDescriptorType;
    uint16  sectorsPerFat;
    uint16  sectorsPerTrack;
    uint16  headCount;
    uint32  hiddenSectorCount;
    uint32  largeSectorCount;

    // Extended Boot Record
    uint8   driveNumber;
    uint8   reserved;
    uint8   signature;
    uint32  volumeID;
    uint8   volumeLabel[11];
    uint8   systemID[8];

} __attribute__((packed)) boot_sector_t;

typedef struct
{
    // File Allocation Table (FAT)
    // We will use 16-bits for our entries
    uint16 entries[2304];

} __attribute__((packed)) fat_t;

typedef struct
{
    // Directory entry contents
    uint8   filename[8];    
    uint8   extension[3];   
    uint8   attributes;     
    uint16  reserved;       
    uint16  creationTime;   
    uint16  creationDate;    
    uint16  lastAccessDate; 
    uint16  ignored;        
    uint16  lastWriteTime;  
    uint16  lastWriteDate;  
    uint16  startingCluster;
    uint32  fileSize;       

} __attribute__((packed)) directory_entry_t;

typedef struct
{
    uint8 *startingAddress;

    // Set to 0 if not opened
    // Set to non-zero if opened
    int isOpened;

    // The directory entry for the file, containing all its metadata
    directory_entry_t entry;

} __attribute__((packed)) file_t;

typedef struct
{   
    uint8 *startingAddress;

    // Set to 0 if not opened
    // Set to non-zero if opened
    int isOpened;

    // The directory entry for the directory, containing all its metadata
    directory_entry_t entry;

} __attribute__((packed)) directory_t;

void init_fs(directory_t *directory);
int openFile(file_t *file);
void closeFile(file_t *file);
int createFile(file_t *file, directory_t *parent);
void deleteFile(directory_entry_t *file, directory_t *parent);
uint8 readByte(file_t *file, uint32 index);
int writeByte(file_t *file, uint8 byte, uint32 index);
int stringcompare(char *string0, char *string1, int length);