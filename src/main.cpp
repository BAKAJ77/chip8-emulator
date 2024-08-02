#include <core/interpreter.h>
#include <debugging.h>
#include <iostream>

int main(int argc, char** argv)
{
    try
    {
        // Get the specified file path of the CHIP-8 program
        if (argc < 2)
            throw std::runtime_error("No CHIP-8 program file was specified\n");

        const std::string filePath = argv[1];

        // Initialize the emulator window and renderer
        OutputLog("[Info] Initializing emulator window\n");
        WindowFrame emulatorWindow("Chip-8 Emulator");

        OutputLog("[Info] Initializing emulator renderer\n");
        GraphicsRenderer& renderer = emulatorWindow.GetRenderer();
        
        // Initialize the emulator interpreter and load the CHIP-8 program
        OutputLog("[Info] Initializing emulator interpreter\n");
        EmulatorInterpreter interpreter;

        OutputLog("[Info] Loading the CHIP-8 program: %s\n", filePath.c_str());
        interpreter.LoadProgram(filePath);

        // The emulator game loop
        while (!interpreter.ShouldTerminate())
        {
            interpreter.Update(emulatorWindow);
            interpreter.Render(renderer);
        }
    }
    catch (const std::exception& e)
    {
        OutputLog("[Error] %s\n", e.what());

#ifdef DEBUG_MODE
        std::cin.get(); // Pause termination so that the debugging console can be read by the user before exiting
#endif

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}