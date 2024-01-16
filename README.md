
![logo](/assets/logo.png)

![C99](https://img.shields.io/badge/C99-blue?style=for-the-badge&logo=C&logoColor=white)
![Raylib](https://img.shields.io/badge/raylib-green?style=for-the-badge&logo=raylib&logoColor=white)<br>
![Linux Status](https://img.shields.io/github/actions/workflow/status/ezri-roberts/chip8/linux.yml?style=for-the-badge&logo=github&label=linux)
![Windows Status](https://img.shields.io/github/actions/workflow/status/ezri-roberts/chip8/windows.yml?style=for-the-badge&logo=github&label=windows)

# CHIP-8 Emulator

A [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) emulator written in C.<br>
Graphics done with the [Raylib](https://www.raylib.com/) library.<br>

## Usage

Run the executable and specify a path to a ROM file.

`chip8 [ROM file]`

Some sample ROMS are included.<br>
Try running:

`chip8 roms/MAZE.ch8`

## Building

Clone the repository.

`git clone git@github.com:ezri-roberts/chip8.git`

The project can be built using make on both Windows and Linux.<br>
To build simply run:<br>

`make all` 

To remove built files:

`make clean`

## Examples

**Pong**
![pong](/assets/pong.png)
**Breakout**
![breakout](/assets/breakout.png)
**IBM Logo**
![ibm](/assets/ibm.png)
