#include "./hal.h"
#include "./types.h"

// Define a keymap to convert keyboard scancodes to ASCII
char keymap[128];

// Track the current cursor's row and column
int cursorCol = 0;
int cursorRow = 0;

int setCursor(int x, int y) 
{
	cursorCol = x % SCREEN_WIDTH;
	cursorRow = ((int)(x/SCREEN_WIDTH) + y) % SCREEN_HEIGHT;
	return 0;	
}

int getCursor()
{
	return ((cursorRow * SCREEN_WIDTH * 2) + cursorCol * 2);
}

int putchar(char character)
{
	char *videomem = (char*) VIDEO_MEM;
	if (character == 0x0A) {
		setCursor(0, cursorRow + 1);
	} else {
		videomem[getCursor()] = character;
		videomem[getCursor() + 1] = TEXT_COLOR;
		setCursor(cursorCol + 1, cursorRow);
	}

	return character;
}

// Print the character array (string) using putchar()
// Print until we find a NULL terminator (0)
int printf(char str[]) 
{
	// Use character count to track how many characters we print
	int characterCount = 0;

	if (cursorRow > 23) {
		clearscreen();
		setCursor(0, 0);
	}

	while (str[characterCount])
	{
		putchar(str[characterCount]);
		characterCount++;
	}

	return characterCount;
}

void clearscreen()
{
	setCursor(0, 0);
	for (int i; i<2000;i++)
	{
		putchar(' ');
	}
	return;
}

char getchar()
{
	while(1)
	{
		if (inb(0x64) & 0b00000001)
		{
			if (keymap[inb(0x60)]) 
			{
				return keymap[inb(0x60)];
			}
		}
	}
}

void scanf(char address[])
{
	
	char new_char = getchar();

	int counter = 0;

	while (new_char) 
	{	
		if (counter == 98)
		{
			printf("\nToo many characters! Returning...");
			address[counter] = new_char;
			address[++counter] = 0x00;
			return;
		}

		if (new_char == 0x0A)
		{
			address[counter] = 0x00;
			return;
		}
		
		putchar(new_char);
		address[counter] = new_char;
		new_char = getchar();
		counter++;
	}

}

// outb (out byte) - write an 8-bit value to an I/O port address (16-bit)
void outb(uint16 port, uint8 value)
{
    asm("outb %1, %0" : : "dN" (port), "a" (value));
	return;
}

// outw (out word) - write an 16-bit value to an I/O port address (16-bit)
void outw(uint16 port, uint16 value)
{
    asm ("outw %1, %0" : : "dN" (port), "a" (value));
	return;
}

// inb (in byte) - read an 8-bit value from an I/O port address (16-bit)
uint8 inb(uint16 port)
{
   uint8 ret;
   asm ("inb %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}

// inw (in word) - read an 16-bit value from an I/O port address (16-bit)
uint16 inw(uint16 port)
{
   uint16 ret;
   asm ("inw %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}

// Initializes the keyboard characters
// Each index in the array "keymap" is the scancode
// Each value in the array is the corresponding ASCII character
// To fill out the rest of the characters refer to this chart:
// https://wiki.osdev.org/PS/2_Keyboard#Scan_Code_Set_1
void initkeymap()
{
    keymap[0x02] = '1';
    keymap[0x03] = '2';
	keymap[0x04] = '3';
	keymap[0x05] = '4';
	keymap[0x06] = '5';
	keymap[0x07] = '6';
	keymap[0x08] = '7';
	keymap[0x09] = '8';
	keymap[0x0A] = '9';
	keymap[0x0B] = '0';
	keymap[0x10] = 'q';
	keymap[0x11] = 'w';
	keymap[0x12] = 'e';
	keymap[0x13] = 'r';
	keymap[0x14] = 't';
	keymap[0x15] = 'y';
	keymap[0x16] = 'u';
	keymap[0x17] = 'i';
	keymap[0x18] = 'o';
	keymap[0x19] = 'p';
	keymap[0x1E] = 'a';
	keymap[0x1F] = 's';
	keymap[0x20] = 'd';
	keymap[0x21] = 'f';
	keymap[0x22] = 'g';
	keymap[0x23] = 'h';
	keymap[0x24] = 'j';
	keymap[0x25] = 'k';
	keymap[0x26] = 'l';
	keymap[0x2C] = 'z';
	keymap[0x2D] = 'x';
	keymap[0x2E] = 'c';
	keymap[0x2F] = 'v';
	keymap[0x30] = 'b';
	keymap[0x31] = 'n';
	keymap[0x32] = 'm';
	keymap[0x39] = ' ';
	keymap[0x1C] = 0x0A;

	return;
}