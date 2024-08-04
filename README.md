# CHIP-8 Emulator

## About
This is a basic CHIP-8 emulator that I have implemented for learning purposes; this is my first attempt at implementing an emulator, so no better place to start than with a emulator simulating a basic CHIP-8 interpreted 
system. This emulator was implemented in C++ and should be able to run all, if not, most CHIP-8 ROMs.

## Requirements
All you will need to build this project is:
- CMake, of version `3.0.0` or higher
- A C++ compiler (preferrably MSVC but shouldn't matter hopefully)

If you don't have CMake installed on your computer, you can download the latest version from their official website [here](https://cmake.org/download/).

## Building
#### For Windows-based systems:
```
git clone --recursive https://github.com/BAKAJ77/chip8-emulator.git
md build
cd build
cmake ..
```

This should generate the project files required for you to be able to build the project yourself, though you can make CMake build the project for you by running the following command: </br>
```
cmake --build .
```

The above command builds a debug version of the application; to build a release version, you simply need to do:
```
cmake --build . --config Release
```

## How To Use
To run a CHIP-8 ROM on the emulator, you can either just drag and drop the ROM file onto the emulator executable file, or you can run the following CMD command:
```
Chip8Emulator.exe <path_to_rom>
```

## Keybindings
The default keybindings is the following:
```
 Emulator Keypad                  Keyboard
+---+---+---+---+             +---+---+---+---+
| 1 | 2 | 3 | C |             | 1 | 2 | 3 | 4 |
+---+---+---+---+             +---+---+---+---+
| 4 | 5 | 6 | D |             | Q | W | E | R |
+---+---+---+---+      =>     +---+---+---+---+
| 7 | 8 | 9 | E |             | A | S | D | F |
+---+---+---+---+             +---+---+---+---+
| A | 0 | B | F |             | Z | X | C | V |
+---+---+---+---+             +---+---+---+---+
```

These keybindings can be customised by editing the `key_bindings.json` file. The json data is formatted as such:
```
{
  // Hex Key : SDL Keycode (the bound key)
  "0": 120, // SDLK_X
  "1": 49,  // SDLK_1
  "2": 50,  // SDLK_2

  ...

  "F": 118  // SDLK_V
}
```

## License
This project is released under the terms of the MIT license. See [LICENSE](LICENSE) for more information or see https://opensource.org/licenses/MIT.
