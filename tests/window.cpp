#include <core/window.h>
#include <chrono>
#include <thread>
#include <random>
#include <ctime>

std::mt19937 mt((uint32_t)time(nullptr));

int GenerateRandomInteger(int min, int max)
{
    std::uniform_int_distribution uniformDistribution(min, max);
    return uniformDistribution(mt);
}

void CheckWindowData(const WindowFrame& window, std::string_view expectedTitle, Vector2<int> expectedResolution,
    Vector2<int> expectedPosition)
{
    if (window.GetTitle() != expectedTitle)
        throw std::exception("Unexpected title data detected");

    if (window.GetResolution() != expectedResolution)
        throw std::exception("Unexpected resolution data detected");

    if (window.GetPosition() != expectedPosition)
        throw std::exception("Unexpected position data detected");
}

int main(int argc, char** argv)
{
    try
    {
        WindowFrame testWindow("Initial Window Test");
        CheckWindowData(testWindow, "Initial Window Test", { 640, 320 }, { SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED });

        for (int testIndex = 1; testIndex <= 10; testIndex++)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(750));

            const std::string title = "Window Test " + std::to_string(testIndex);
            const Vector2<int> resolution = { GenerateRandomInteger(300, 1600), GenerateRandomInteger(300, 900) };
            const Vector2<int> position = { GenerateRandomInteger(400, 1200), GenerateRandomInteger(300, 800) };
            
            testWindow.SetTitle(title);
            testWindow.SetPosition(position);
            testWindow.SetResolution(resolution);

            CheckWindowData(testWindow, title, resolution, position);
        }
    }
    catch(const std::exception& e)
    {
        std::printf("%s\n", e.what());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}