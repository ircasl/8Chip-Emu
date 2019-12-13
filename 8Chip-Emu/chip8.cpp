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
	// Draw from VRAM
	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			if (GFX[(y * 64) + x] == 0)
				printf("O");
			else
				printf(" ");
		}
		printf("\n");
	}
	printf("\n");
}

void chip8::emulateCycle()
{
	// Fetch Opcode
	OPCode = Memory[PC] << 8 | Memory[PC + 1];
	// Bitwise operation works something like this
	// Memory[PC] << 8 shift the memory value 8 bits to the left ???? ???? 0000 0000 
	// And after the result value | Memory[PC + 1] witch takes the most right 8 bits of the result that are all 0 and change them to Memory[PC + 1] value.
	// Using this operations we get the 2 bytes we wanted from the program

	// Decode Opcode
	// Execute Opcode
	switch (OPCode & 0xF000) //Let's distinguish OPCodes by the most significant value first and inside the multiple cases if needed create more switch cases
	{
		case 0x2000:// 0x2NNN: Calls subroutine at NNN.
			Stack[SP] = PC; // Let's save the current adress to the stack
			SP++; // Increment the stack pointer
			PC = OPCode & 0x0FFF; // Let's save the only the NNN values to the PC
			break;
		case 0x6000:// 6XNN: Sets VX to NN.
			V[(OPCode & 0x0F00) >> 8] = OPCode & 0x00FF; //Don't forget to shift the value 8 bitsso that it represents the value that we want
			PC += 2;
			break;
		case 0xA000: // ANNN: Sets I to the address NNN.
			I = OPCode & 0x0FFF;
			PC += 2;
			break;
		case 0xD000: // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
					 // Each row of 8 pixels is read as bit-coded starting from memory location I;
					 // I value doesn�t change after the execution of this instruction.
					 // As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn�t happen 
			// Get the VX and VY coordinate
			unsigned short x = V[(OPCode & 0x0F00) >> 8]; 
			unsigned short y = V[(OPCode & 0x00F0) >> 4];
			//Get height
			unsigned short height = OPCode & 0x000F;
			//Pixel that we get from memory
			unsigned short pixel;

			//Let's assume that VF won't be changed
			V[0xF] = 0;

			for (int yLine = 0; yLine < height; yLine++)
			{
				pixel = Memory[I + yLine]; //Get the byte representing the pixel
				for (int xPos = 0; xPos < 8; xPos++)
				{
					if ((pixel & (0x80 >> xPos)) != 0) // Test values from left to right using 0x80 and a bitshift to the right
					{
						if (GFX[(x + xPos + ((y + yLine) * 64))] == 1)
						{
							V[0xF] = 1; //A pixel as been changed from set to unset
						}
						GFX[x + xPos + ((y + yLine) * 64)] ^= 1; //XOR operation on the current value inside the VRAM
					}
				}
			}

			DrawFlag = true;
			PC += 2;
			break;
		default:
			printf("Unknown opcode: 0x%X\n", OPCode);
	}


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
	long fileByteSize = ftell(pFile); //Number of bytes since the beginning of the file
	rewind(pFile); //Get back to the beginning of the file
	printf("Filesize: %d\n", (int)fileByteSize);

	//Check if 8 Chip is able to load the program
	if (fileByteSize > WORKING_RAM_MAX_AMOUNT) {
		printf("Error: ROM too big for memory");
	}

	// Allocate memory to contain the whole file
	uint8_t* buffer = (uint8_t*)malloc(sizeof(uint8_t) * fileByteSize);
	if (buffer == NULL)
	{
		fputs("Memory error", stderr);
		return false;
	}

	// Copy the file into the buffer
	size_t result = fread(buffer, 1, fileByteSize, pFile);
	if (result != fileByteSize)
	{
		fputs("Reading error", stderr);
		return false;
	}

	// Copy buffer to Chip8 memory
	memcpy(&Memory[PC], buffer, sizeof(uint8_t) * fileByteSize); //We need to use uint8_t * fileByteSize
																 //because buffer is a pointer to the compiler it is the same as a pointer to a single element	
	
	// Close file, free buffer
	fclose(pFile);
	free(buffer);

	return true;
}

