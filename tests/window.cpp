#include <window.h>
#include <glfw/glfw3.h>
#include <stdexcept>

int main(int argc, char** argv)
{
    try
    {
        Window window("Window Test");
        
        if (window.GetWidth() != 640 || window.GetHeight() != 320)
            throw std::exception("Returned invalid window size data");

        window.SetTitle("Title Test");
        window.SetSize(800, 600);

        if (window.GetTitle() != "Title Test" || window.GetWidth() != 800 || window.GetHeight() != 600)
            throw std::exception("Returned invalid window title and size data");

        if (window.WasRequestedToClose())
            throw std::exception("Flag should be false, no request has been made");

        window.PollEvents();
        window.SwapRenderBuffers();

        const char* errorDesc = nullptr;
        if (glfwGetError(&errorDesc) != GLFW_NO_ERROR)
            throw std::exception(errorDesc);
    }
    catch(std::exception e)
    {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}