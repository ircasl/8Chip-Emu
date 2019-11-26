#include "chip8.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

chip8::chip8()
{
	init();
}

chip8::~chip8()
{
}

uint8_t Chip8FontSet[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, //0
	0x20, 0x60, 0x20, 0x20, 0x70, //1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	0xF0, 0x10, 0x20, 0x40, 0x40, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
	0xF0, 0x90, 0xF0, 0x90, 0x90, //A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	0xE0, 0x90, 0x90, 0x90, 0xE0, //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

void chip8::init() {
	// Initialize registers and memory once
	PC = 0x200;		// Program counter starts at 0x200 (Start adress program)
	OPCode = 0;		// Reset current opcode	
	I = 0;			// Reset index register
	SP = 0;			// Reset stack pointer

	// Clear display
	memset(GFX, 0, sizeof(GFX)); //GFX[2048] 

	// Clear stack
	memset(Stack, 0, sizeof(Stack)); //Stack[16]
	memset(Key, 0, sizeof(Key)); //Key[16]
	memset(V, 0, sizeof(V)); //V[16]

	// Clear memory
	memset(Memory, 0, sizeof(Memory)); //Memory[4096]

	// Load fontset
	memcpy(Memory, Chip8FontSet, sizeof(Chip8FontSet));

	// Reset timers
	DelayTimer = 0;
	SoundTimer = 0;

	// Clear screen once
	DrawFlag = true;

	srand(time(NULL));
}

void chip8::debugRender()
{
}

void chip8::emulateCycle()
{
	// Fetch Opcode
	// Decode Opcode
	// Execute Opcode

	// Update timers
}

bool chip8::loadApplication(const char* filename)
{
	printf("Loading: %s\n", filename);

	// Open file in binary mode
	FILE* pFile;
	fopen_s(&pFile, filename, "rb");
	if (pFile == NULL)
	{
		fputs("File error", stderr);
		return false;
	}

	// Check file size
	fseek(pFile, 0, SEEK_END); // Move pointer to the end of the file
	long lSize = ftell(pFile); //Number of bytes since the beginning of the file
	rewind(pFile); //Get back to the beginning of the file
	printf("Filesize: %d\n", (int)lSize);

	//Check if 8 Chip is able to load the program
	if (lSize > WORKING_RAM_MAX_AMOUNT) {
		printf("Error: ROM too big for memory");
	}

	// Allocate memory to contain the whole file
	uint8_t* buffer = (uint8_t*)malloc(sizeof(uint8_t) * lSize);
	if (buffer == NULL)
	{
		fputs("Memory error", stderr);
		return false;
	}

	// Copy the file into the buffer
	size_t result = fread(buffer, 1, lSize, pFile);
	if (result != lSize)
	{
		fputs("Reading error", stderr);
		return false;
	}

	// Copy buffer to Chip8 memory
	memcpy(&Memory[PC], buffer, sizeof(uint8_t) * lSize); //We need to use uint8_t * lSize
														  //because buffer is a pointer to the compiler it is the same as a pointer to a single element

	// Close file, free buffer
	fclose(pFile);
	free(buffer);

	return true;
}

