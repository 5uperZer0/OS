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

	// Schedule the next process

	printf(message0);
	yield();

	printf(message1);
	yield();

	printf(message2);
	exit();
}

// The user processes

void proca()
{
	char message0[] = "Starting User Process A\n";
	char message1[] = "Resuming User Process A\n";
	char message2[] = "Exiting User Process A\n";

	printf(message0);
	yield();

	printf(message1);
	yield();
	
	printf(message2);
	yield();
}