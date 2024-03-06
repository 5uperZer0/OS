#include "./hal.h"
#include "./multitasking.h"

void prockernel();
void proca();

int main() 
{
	// A string in C is an array of characters
	char userInput[100];
	char helloWorld[] = "Hello World!\n";
	char message[] = "Please type something: ";
	char response[] = "\nYou typed: ";
	char newLine[] = "\n";
	
	// Clear the screen
	clearscreen();

	// Initialize our keyboard
	initkeymap();

	startkernel(prockernel);

	// Print our string
	// printf(helloWorld);

	// Run an infinite loop
	// for(;;)
	// {
	// 	// Ask the user to type something
	// 	printf(message);

	// 	// Get the complete string from the user's keyboard
	// 	scanf(userInput);

	// 	// Print the string back to the user
	// 	printf(response);
	// 	printf(userInput);
	// 	printf(newLine);
	// }

	return 0;
}

void prockernel()
{
	char message0[] = "Starting Kernel Process\n";
	char message1[] = "Resuming Kernel Process\n";
	char message2[] = "Exiting Kernel Process\n";

	// Create the user processes

	createproc(proca, (void *) 0x3000);
	createproc(procb, (void *) 0x3100);
	createproc(procc, (void *) 0x3200);
	createproc(procd, (void *) 0x3300);
	createproc(proce, (void *) 0x3400);

	// while loop -- while any process is ready, yield

	exit();

}

// The user processes

void proca()
{
	char letter = 'A';
	putchar(&letter);
	exit();
}

void procb()
{
	char letter = 'B';
	putchar(&letter);
	yield();

	putchar(&letter);
	exit();
}

void procc()
{
	char letter = 'C';
	putchar(&letter);
	yield();

	putchar(&letter);
	yield();

	putchar(&letter);
	yield();

	putchar(&letter);
	exit();
}


void procd()
{
	char letter = 'D';
	putchar(&letter);
	yield();

	putchar(&letter);
	yield();

	putchar(&letter);
	exit();
}


void proce()
{
	char letter = 'E';
	putchar(&letter);
	yield();

	putchar(&letter);
	exit();
}