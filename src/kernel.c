#include "./hal.h"
#include "./multitasking.h"

void prockernel();
void proca();
void procb();
void procc();
void procd();
void proce();

int main() 
{
	// Clear the screen
	clearscreen();

	// Initialize our keyboard
	initkeymap();

	startkernel(prockernel);

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
	printf(message0);
	while(users_ready())
	{
		yield();
	}
	putchar('\n');
	printf(message2);


	exit();

	return;
}

// The user processes

void proca()
{
	putchar('A');
	exit();
}

void procb()
{
	putchar('B');
	yield();

	putchar('B');
	exit();
}

void procc()
{
	putchar('C');
	yield();

	putchar('C');
	yield();

	putchar('C');
	yield();

	putchar('C');
	exit();
}


void procd()
{
	putchar('D');
	yield();

	putchar('D');
	yield();

	putchar('D');
	exit();
}


void proce()
{
	putchar('E');
	yield();

	putchar('E');
	exit();
}