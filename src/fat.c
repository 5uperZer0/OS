#include "./fat.h"
#include "./fdc.h"

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
    fat1 = (fat_t *) (startAddress+sizeof(fat_t)); // Put FAT at 0x21200
    floppy_read(0, 10, (void *)fat1, sizeof(fat_t));

    // Read the root directory (Drive 0, Cluster 19, 512 bytes * 14 clusters)
    directory->startingAddress = (uint8 *) (startAddress+(sizeof(fat_t)*2)); // Put ROOT at 0x22400
    floppy_read(0, 19, (void *)directory->startingAddress, 512 * 14);

    directory->entry.filename[0] = 'R';
    directory->entry.filename[1] = 'O';
    directory->entry.filename[2] = 'O';
    directory->entry.filename[3] = 'T';
}

void closeFile(file_t *file)
{
    int index = 0;
    
    //Need to use floppy_write to move the write_byte changes to the actual floppy disk
    floppy_write(0, file->entry.startingCluster + 31, file->startingAddress, 512-1);

    // use filesize to determine how much to write.

    int cluster = (file->entry.startingCluster + 31, file->startingAddress, 512-1);  
    int remaining = file->entry.fileSize;

    while (remaining > 0) 
    {
        floppy_write(0, cluster, file->startingAddress, 512-1);
        remaining -= 512;
        if(remaining > 0)
        {
            if(fat0->entries[cluster] != 0xFFFF)
            {
            cluster = fat0->entries[cluster];
            }
            else
            {
                for(int entry = 2; entry < 2304; entry++)
                {
                    if(fat0->entries[entry] == 0 && fat1->entries[entry] == 0)
                    {
                        fat0->entries[cluster] = entry;
                        cluster = entry;
                        break;
                    }   
                }
            }
        }
    }  

    // Further, need to have code to detect if bytes written exceed one sector to add more sectors

    // Writebyte writes to continuous memory, closeFile breaks memory into sectors and writes to floppy

    file->isOpened = 0;
    return;
}

int openFile(file_t *file)
{
    int first_cluster = file->entry.startingCluster;
    if (first_cluster >= 2 && fat0->entries[first_cluster] == fat1->entries[first_cluster] ) {
        
        file->isOpened = 1;
        return 0;
    }
    printf("Error: File cluster invalid or corrupted!");
    return -1;
}

// Creates an empty file and writes it to the floppy disk
// You can see the effects of calling this function by viewing your OS image in a hex editor
// Your OS image will be updated to contain the following:
// FAT Tables changes:
// 0x0266 - 0x0267: 0xFFFF (FAT entry) (First FAT table)
// 0x1466 - 0x1467: 0xFFFF (FAT entry) (Second FAT table)
// Root Directory changes:
// 0x2620 - 0x263F: Directory entry for our new file (contains filename, extension, startingCluster, filesize, etc.)
// File changes:
// 0xA400 - 0xA5FF: Our file (1 sector) contains either 0's or "Hello World!\n"
int createFile(file_t *file, directory_t *parent)
{
    // Point to the first directory entry in our directory
    directory_entry_t *emptyEntry = (directory_entry_t *)parent->startingAddress;
    
    // If the first directory entry is not empty, loop until we find an empty directory entry
    uint8 entryCount = 0;
    while(emptyEntry->filename[0] != 0)
    {
        // Incrementing emptyEntry points to the next 32 byte directory entry
        emptyEntry++;

        // For now, we are assuming that our directories can hold a max of 16 directory entries
        // If we hit 16 without finding an empty entry, return with error
        entryCount++;
        if(entryCount >= 16)
        {
            printf("Error: Directory is full!\n");
            return -1;
        }
    }

    // Start at the beginning of the FAT table (entry 2) and loop until the end
    for(int entry = 2; entry < 2304; entry++)
    {
        // Find the first FAT entry that is 0 for both copies of the FAT table
        // If the FAT entry is 0 that means the sector is free and this is where we will place our file
        if(fat0->entries[entry] == 0 && fat1->entries[entry] == 0)
        {
            // Below we update the directory entry for our file we are about to create
            // Without this information, our root directory would not have the info necessary for us to find our file later

            // Update the filename for our new directory entry
            for(int i = 0; i < 8; i++)
            {
                emptyEntry->filename[i] = file->entry.filename[i];
            }

            // Update the extension for our new directory entry
            for(int i = 0; i < 3; i++)
            {
                emptyEntry->extension[i] = file->entry.extension[i];
            }

            // The starting cluster is this FAT table entry
            emptyEntry->startingCluster = entry;

            // Our file starts empty but must take up at least 1 sector
            emptyEntry->fileSize = 512;

            // 0x30000 is selected as where our file will exist in memory for the time being
            // This can be anything, but we are choosing this because it is far enough away so we don't overwrite anything else in memory
            file->startingAddress = (uint8 *)0x30000;

            // Indicate we have opened our file
            file->isOpened = 1;

            // Write 512 bytes worth of 0's into memory at the start of our file
            for(int i = 0; i < 512; i++)
            {
                writeByte(file, 0, i);
            }
            
            // NOTE: Uncomment the following section if you want to see "Hello World!\n" in your file for debugging purposes
            // Write "Hello World!\n" to the file
            // char string[] = "Hello World!\n";
            // for(int i = 0; string[i] != 0; i++)
            // {
                // writeByte(file, (uint8) string[i], i);
            // }

            // Update this FAT tables to indicate this is the end of our file
            // Our file is empty and takes up 1 sector
            // We update the FAT table entry to 0xFFFF to show this is the last sector for the file
            fat0->entries[entry] = 0xFFFF;
            fat1->entries[entry] = 0xFFFF;

            // Our FAT entry must be converted to a data sector
            // Our FAT entires start at 2
            // Our data sectors start at 33
            // sector = entry + 33 - 2 = entry + 31
            uint32 fileSector = entry + 31;

            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            // NOTE:    For some reason, floppy_write requires (count - 1) bytes                +
            //          Example: To write 1 sector you should use: count = 512-1                +
            //          Without this, the floppy_write writes invalid data to the floppy disk   +
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            // Write both copies of our FAT tables to the floppy disk
            floppy_write(0, 1, (void *)fat0, sizeof(fat_t)-1);
            floppy_write(0, 10,(void *)fat1, sizeof(fat_t)-1);

            // Write our file to the floppy disk
            floppy_write(0, fileSector, (void *)file->startingAddress,      512-1);

            // Check if the parent is the root directory
            // Since we added a file to the parent directory, we have to save it to the floppy disk too!
            if( parent->entry.filename[0] == 'R' && 
                parent->entry.filename[1] == 'O' && 
                parent->entry.filename[2] == 'O' && 
                parent->entry.filename[3] == 'T')
            {
                // If this is the root directory, update physical sector 19 (the location of the root directory on the floppy)
                // This is a special case because we cannot reference physical sector 19 using FAT table entries!
                floppy_write(0, 19, (void *)parent->startingAddress, 512-1);
            }
            else
            {
                // If this is NOT the root directory, update the physical sector of the directory
                // Again, we are assuming our directories can only hold 16 files/directories (1 sector = 512 bytes / 32 bytes)
                uint32 directorySector = parent->entry.startingCluster + 31;
                floppy_write(0, directorySector, (void *)parent->startingAddress, 512-1);
            }

            // Indicate the file is now closed
            file->isOpened = 0;

            // Return success!
            return 0;
        }
    }

    // If we searched the entire FAT table and did not find a 0, our disk is full!
    printf("Error: Floppy disk is full!\n");
    return -1;
}

uint8 readByte(file_t *file, uint32 index)
{        
    if(file->isOpened && file->startingAddress != 0){
        uint8 byte = floppy_read(0, file->entry.startingCluster, file->startingAddress[index], 1);
        return byte;
    } 
    printf("Error: File was not opened or pointed to NULL!\n");
    return -1;
}

// Writes a byte to a file that is currently loaded into memory
// This does NOT modify the floppy disk
// To write this to the floppy disk, we have to call floppy_write()
int writeByte(file_t *file, uint8 byte, uint32 index)
{
    // Check if the file is opened and is not a NULL pointer
    if(file->isOpened && file->startingAddress != 0)
    {
        // Place the byte at the address + index
        file->startingAddress[index] = byte;
        // Increase the file size
        file->entry.fileSize++;
        return 0;
    }

    // If the file was not opened, or was a NULL pointer, return error
    printf("Error: File was not opened or pointed to NULL!\n");
    return -1;
}

void deleteFile(file_t *file, directory_t *parent)
{
    uint16 fatEntry = file->entry.startingCluster;
    uint16 next;

    while(fatEntry != 0xFFFF)
    {
        next = fat0->entries[fatEntry];

        fat0->entries[fatEntry] = 0;
        fat1->entries[fatEntry] = 0;

        fatEntry = next;
    }

    directory_entry_t *entry = (directory_entry_t *)parent.startingAddress;

    while(entry->filename[0] != 0)
    {
        if (stringcompare((char *)entry->filename, file->filename, 8) && stringcompare((char *)entry->extension, file->extension, 3))
        {
            for(int i = 0; i < sizeof(directory_entry_t); i++)
            {
                writeByte(file, 0, i);
            }
        }       
        entry++;
    }

    if( parent->entry.filename[0] == 'R' && 
        parent->entry.filename[1] == 'O' && 
        parent->entry.filename[2] == 'O' && 
        parent->entry.filename[3] == 'T')
    {
        floppy_write(0, 19, (void *)parent->startingAddress, 512-1);
    }
    else
    {
        uint32 directorySector = parent->entry.startingCluster + 31;
        floppy_write(0, directorySector, (void *)parent->startingAddress, 512-1);
    }

}