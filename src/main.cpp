#include <core/window.h>
#include <iostream>

int main(int argc, char** argv)
{
    try
    {
        WindowFrame emulatorWindow("Chip8Emulator");
        
        bool terminateEmulator = false;
        while (!terminateEmulator) // The emulator game loop
        {
            // Handle window events
            SDL_Event event;
            while (emulatorWindow.PollEvents(event))
            {
                if (event.type == SDL_EVENT_QUIT)
                    terminateEmulator = true;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}