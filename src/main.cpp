#include <window.h>
#include <iostream>

int main(int argc, char** argv)
{
    try
    {
        Window emulatorWindow("Chip8Emulator");

        while (!emulatorWindow.WasRequestedToClose())
        {
            emulatorWindow.PollEvents();
            emulatorWindow.SwapRenderBuffers();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}