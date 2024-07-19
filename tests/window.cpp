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

void CheckWindowData(const WindowFrame& window, std::string_view expectedTitle, int expectedWidth, int expectedHeight,
    int expectedPosX, int expectedPosY)
{
    if (window.GetTitle() != expectedTitle)
        throw std::exception("Unexpected title data detected");

    if (window.GetWidth() != expectedWidth)
        throw std::exception("Unexpected width data detected");

    if (window.GetHeight() != expectedHeight)
        throw std::exception("Unexpected height data detected");

    if (window.GetPositionX() != expectedPosX)
        throw std::exception("Unexpected X position data detected");

    if (window.GetPositionY() != expectedPosY)
        throw std::exception("Unexpected Y position data detected");
}

int main(int argc, char** argv)
{
    try
    {
        WindowFrame testWindow("Initial Window Test");
        CheckWindowData(testWindow, "Initial Window Test", 640, 320, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

        for (int testIndex = 1; testIndex <= 10; testIndex++)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));

            const std::string title = "Window Test " + std::to_string(testIndex);
            const int width = GenerateRandomInteger(300, 1600);
            const int height = GenerateRandomInteger(300, 900);
            const int xPos = GenerateRandomInteger(400, 1200);
            const int yPos = GenerateRandomInteger(300, 800);

            testWindow.SetTitle(title);
            testWindow.SetPosition(xPos, yPos);
            testWindow.SetSize(width, height);

            CheckWindowData(testWindow, title, width, height, xPos, yPos);
        }
    }
    catch(const std::exception& e)
    {
        std::printf("%s\n", e.what());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}