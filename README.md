# 8Chip-Emu

A simple 8-Chip emulator writen in C++ using OpenGL for rendering and GLFW for window management.
Based on the work of Laurence Muller.
<p align="center">
      <img src="Docs/Images/Maze.png">
</p>

## Compiling and Running

### Windows
Use Visual Studio 2019 for compilation.
After compilation add glfw3.dll to the same folder as the executable.

Usage:
```
8Chip-Emu.exe ROM
```
### Other OS
The code is platform agnostic so you should be able to use it to build the app for Linux or MacOS too.

## Key Mapping 
Original Keypad:

123C

456D

789G

A0BF

Keyboard Mapping:

1234

qwer

asdf

zxcv

## References
Helpful resources used when writing this emulator:

- http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
- http://en.wikipedia.org/wiki/CHIP-8
- http://www.multigesture.net/wp-content/uploads/mirror/goldroad/chip8.shtml
- http://mattmik.com/files/chip8/mastering/chip8.html
