#include <core/interpreter.h>
#include <iostream>

int main(int argc, char** argv)
{
    try
    {
        WindowFrame emulatorWindow("Chip-8 Emulator");
        GraphicsRenderer& renderer = emulatorWindow.GetRenderer();
        
        EmulatorInterpreter interpreter;
        interpreter.LoadProgram("tetris.c8");

        while (!interpreter.ShouldTerminate()) // The emulator game loop
        {
            interpreter.Update(emulatorWindow);
            interpreter.Render(renderer);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}