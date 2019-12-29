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
		case 0x0000:
			switch (OPCode & 0x00FF)
			{
				case 0x00EE: // 00EE: Returns from a subroutine.
					SP--; // 16 levels of stack, decrease stack pointer to prevent overwrite
					PC = Stack[SP]; //Set the program counter to the saved value on the stack
					PC += 2; //Skip to the next instruction
					break;
				case 0x00E0: // 00E0: Clears the screen.
					memset(GFX, 0, sizeof(GFX)); //GFX[2048] 
					DrawFlag = true; //Let's set this so that the draw logic knows that it needs to redraw
					PC += 2;
					break;
				default:
					// TODO: Add 0NNN
					printf("Unknown opcode: 0x%X\n", OPCode);
			}
			break;
		case 0x1000:// 1NNN: Jumps to address NNN.
			PC = OPCode & 0x0FFF;
			break;
		case 0x2000:// 0x2NNN: Calls subroutine at NNN.
			Stack[SP] = PC; // Let's save the current adress to the stack
			SP++; // Increment the stack pointer
			PC = OPCode & 0x0FFF; // Let's save the only the NNN value to the PC
			break;
		case 0x3000:// 3XNN: Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
			if (V[(OPCode & 0x0F00) >> 8] == (OPCode & 0x00FF))
				PC += 4;
			else // We still need to read the next if instruction even if VX != NN
				PC += 2;
			break;
		case 0x4000:// 4XNN: Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
			if (V[(OPCode & 0x0F00) >> 8] != (OPCode & 0x00FF))
				PC += 4;
			else // We still need to read the next if instruction even if VX == NN
				PC += 2;
			break;
		case 0x5000:// 5XY0: Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
			if (V[(OPCode & 0x0F00) >> 8] == V[(OPCode & 0x00F0) >> 4])
				PC += 4;
			else // We still need to read the next if instruction even if VX != VY
				PC += 2;
			break;
		case 0x6000:// 6XNN: Sets VX to NN.
			V[(OPCode & 0x0F00) >> 8] = OPCode & 0x00FF; //Don't forget to shift the value 8 bitsso that it represents the value that we want
			PC += 2;
			break;
		case 0x7000:// 7XNN: Adds NN to VX. (Carry flag is not changed)
			V[(OPCode & 0x0F00) >> 8] += OPCode & 0x00FF;
			PC += 2;
			break;
		case 0x8000:
			switch (OPCode & 0x000F)
			{
			case 0x0000: // 8XY0: Sets VX to the value of VY.
				V[(OPCode & 0x0F00) >> 8] = V[(OPCode & 0x00F0) >> 4];
				PC += 2; //Skip to the next instruction
				break;
			case 0x0001: // 8XY1: Sets VX to VX or VY. (Bitwise OR operation)
				V[(OPCode & 0x0F00) >> 8] = (V[(OPCode & 0x0F00) >> 8] | V[(OPCode & 0x00F0) >> 4]);
				PC += 2;
				break;
			case 0x0002: // 8XY2: Sets VX to VX and VY. (Bitwise AND operation)
				V[(OPCode & 0x0F00) >> 8] = (V[(OPCode & 0x0F00) >> 8] & V[(OPCode & 0x00F0) >> 4]);
				PC += 2;
				break;
			case 0x0003:// 8XY3: Sets VX to VX xor VY.
				V[(OPCode & 0x0F00) >> 8] = (V[(OPCode & 0x0F00) >> 8] ^ V[(OPCode & 0x00F0) >> 4]);
				PC += 2;
				break;
			case 0x0004:// 8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
				if (V[(OPCode & 0x00F0) >> 4] > (0xFF - V[(OPCode & 0x0F00) >> 8])) //If VY > 0xFF - VX then VX+VY will have a carry
					V[0xF] = 1; //carry
				else
					V[0xF] = 0;
				V[(OPCode & 0x0F00) >> 8] = (V[(OPCode & 0x0F00) >> 8] + V[(OPCode & 0x00F0) >> 4]);
				PC += 2;
				break;
			case 0x0005:// 8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
				if (V[(OPCode & 0x00F0) >> 4] > V[(OPCode & 0x0F00) >> 8]) //If VY > 0xFF - VX then VX+VY will have a carry
					V[0xF] = 0; //borrow
				else
					V[0xF] = 1;
				V[(OPCode & 0x0F00) >> 8] = (V[(OPCode & 0x0F00) >> 8] - V[(OPCode & 0x00F0) >> 4]);
				PC += 2;
				break;
			case 0x0006:// 8XY6: Stores the least significant bit of VX in VF and then shifts VX to the right by 1
				V[0xF] = V[(OPCode & 0x0F00) >> 8] & 0x01;
				V[(OPCode & 0x0F00) >> 8] = (V[(OPCode & 0x0F00) >> 8] >> 1); //shift to right by one
				PC += 2;
				break;
			case 0x0007:// 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
				if (V[(OPCode & 0x0F00) >> 8] > V[(OPCode & 0x00F0) >> 4]) //If VY > 0xFF - VX then VX+VY will have a carry
					V[0xF] = 0; //borrow
				else
					V[0xF] = 1;
				V[(OPCode & 0x0F00) >> 8] = (V[(OPCode & 0x00F0) >> 4] - V[(OPCode & 0x0F00) >> 8]);
				PC += 2;
				break;
			case 0x000E:// 8XYE: Stores the most significant bit of VX in VF and then shifts VX to the left by 1.
				V[0xF] = V[(OPCode & 0x0F00) >> 8] >> 7; //Shift 7 bits to the right to get the most significant bit only
				V[(OPCode & 0x0F00) >> 8] = (V[(OPCode & 0x0F00) >> 8] << 1); //shift to right by one
				PC += 2;
				break;
			default:
				printf("Unknown opcode: 0x%X\n", OPCode);
			}
			break;
		case 0x9000:// 9XY0: Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
			if (V[(OPCode & 0x0F00) >> 8] != V[(OPCode & 0x00F0) >> 4])
				PC += 4;
			else // We still need to read the next if instruction even if VX == VY
				PC += 2;
			break;
		case 0xA000: // ANNN: Sets I to the address NNN.
			I = OPCode & 0x0FFF;
			PC += 2;
			break;
		case 0xB000:// BNNN: Jumps to the address NNN plus V0.
			PC = (OPCode & 0x0FFF) + V[0];
			break;
		case 0xC000:// CXNN: Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
			V[OPCode & 0x0F00 >> 8] = (OPCode & 0x00FF) & (rand() % 0x00FF);
			break;
		case 0xD000: // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
					 // Each row of 8 pixels is read as bit-coded starting from memory location I;
					 // I value doesn’t change after the execution of this instruction.
					 // As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen 
			{
				// Get the VX and VY coordinate
				uint16_t x = V[(OPCode & 0x0F00) >> 8];
				uint16_t y = V[(OPCode & 0x00F0) >> 4];
				//Get height
				uint16_t height = OPCode & 0x000F;
				//Pixel that we get from memory
				uint16_t pixel;

				//Let's assume that VF won't be changed
				V[0xF] = 0;

				for (int yLine = 0; yLine < height; yLine++)
				{
					pixel = Memory[I + yLine]; // Get the byte representing the pixel
												// the pixel is enconded for example if screen is clear 11110000 will give ****EEEE, E being blank
					for (int xLine = 0; xLine < 8; xLine++)
					{
						if ((pixel & (0x80 >> xLine)) != 0) // Test values from left to right by using 0x80 and a bitshift to the right
						{
							if (GFX[(x + xLine + ((y + yLine) * 64))] == 1)
							{
								V[0xF] = 1; //A pixel as been changed from set to unset
							}
							GFX[x + xLine + ((y + yLine) * 64)] ^= 1; //XOR operation on the current value inside the VRAM
						}
					}
				}

				DrawFlag = true;
				PC += 2;
			}
			break;
		case 0xE000:
			switch (OPCode & 0x00FF)
			{
			case 0x009E:// EX9E: Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
				if (Key[V[(OPCode & 0x0F00) >> 8]] == 1)
					PC += 4;
				else
					PC += 2;
				break;
			case 0x00A1:// EXA1: Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)
				if (Key[V[(OPCode & 0x0F00) >> 8]] == 0)
					PC += 4;
				else
					PC += 2;
				break;
			default:
				printf("Unknown opcode: 0x%X\n", OPCode);
			}
			break;
		case 0xF000:
			switch (OPCode & 0x00FF)
			{
				case 0x0007:// FX07: Sets VX to the value of the delay timer.
					V[(OPCode & 0x0F00) >> 8] = DelayTimer;
					PC += 2;
					break;
				case 0x000A:// FX0A: A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
					{
					bool keyPressed = false;

					for (int i = 0; i < 16; ++i) //Maybe add a or keyPressed inside the for condition
					{
						if (Key[i] != 0)
						{
							V[(OPCode & 0x0F00) >> 8] = i;
							keyPressed = true;
						}
					}

					// If we didn't received a keypress, skip this cycle and try again.
					if (!keyPressed)
						return;

					PC += 2;
					}
					break;
				case 0x0015:// FX15: Sets the delay timer to VX.
					DelayTimer = V[(OPCode & 0x0F00) >> 8];
					PC += 2;
					break;
				case 0x0018:// FX18: Sets the sound timer to VX.
					SoundTimer = V[(OPCode & 0x0F00) >> 8];
					PC += 2;
					break;
				case 0x001E:// FX1E: Adds VX to I. VF is set to 1 when there is a range overflow (I+VX>0xFFF), and to 0 when there isn't.
					if (I + V[(OPCode & 0x0F00) >> 8] > 0x0FFF)
						V[0xF] = 1;
					else
						V[0xF] = 0;

					I = I + V[(OPCode & 0x0F00) >> 8];
					PC += 2;
					break;
				case 0x0029:// FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
					I = V[((OPCode & 0x0F00) >> 8)] * 0x5; //The sprites are 5 bytes long therefore we need to implement a 0x5 offset to find each char
					PC += 2;
					break;
				case 0x0033: // FX33: Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1,
							 // and the least significant digit at I plus 2. 
							 //(In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I,
							 //the tens digit at location I+1, and the ones digit at location I+2.)
					Memory[I] = V[(OPCode & 0x0F00) >> 8] / 100; //Get the most significant digit
					Memory[I + 1] = (V[(OPCode & 0x0F00) >> 8] / 10) % 10;   //Get the middle digit
					Memory[I + 2] = (V[(OPCode & 0x0F00) >> 8] % 100) % 10;   //Get the least significant digit
					PC += 2;
					break;
				case 0x0055:// FX55: Stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.
					for (int i = 0; i <= ((OPCode & 0x0F00) >> 8); ++i)
						Memory[I + i] = V[i];
					// On the original CHIP-8 and CHIP-48, when the operation is done, I = I + X + 1.
					I += ((OPCode & 0x0F00) >> 8) + 1;
					PC += 2;
					break;
				case 0x0065: // FX65: Fills V0 to VX (including VX) with values from memory starting at address I.
					for (int i = 0; i < ((OPCode & 0x0F00) >> 8); i++) {
						V[i] = Memory[I + i];
					}
					// On the original CHIP-8 and CHIP-48, when the operation is done, I = I + X + 1.
					I += ((OPCode & 0x0F00) >> 8) + 1;
					PC += 2;
					break;
				default:
					printf("Unknown opcode: 0x%X\n", OPCode);
			}
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

