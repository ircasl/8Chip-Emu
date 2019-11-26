#pragma once
#include <cstdint>
// Memory map of the 8 bit chip
// 0x000 - 0x1FF - Chip 8 interpreter(contains font set in emu)
// 0x050 - 0x0A0 - Used for the built in 4x5 pixel font set(0 - F)
// 0x200 - 0xFFF - Program ROM and work RAM

#define WORKING_RAM_MAX_AMOUNT 3584

class chip8
{
	public:
		chip8();
		~chip8();

		bool DrawFlag;

		void debugRender();
		void emulateCycle();
		bool loadApplication(const char* filename);

		// Chip8
		uint16_t  GFX[64 * 32];	// Total amount of pixels: 2048 //VRAM
		uint16_t  Key[16];

	private:
		uint16_t PC;		// Program counter
		uint16_t OPCode;	// Current opcode
		uint16_t I;		// Index register
		uint16_t SP;		// Stack pointer

		uint8_t  V[16];			// V-regs (V0-VF)
		uint16_t Stack[16];		// Stack (16 levels)
		uint8_t  Memory[4096];	// Memory (size = 4k)		

		uint8_t  DelayTimer;	// Delay timer
		uint8_t  SoundTimer;	// Sound timer		

		void init();
};
