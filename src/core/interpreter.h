#ifndef INTERPRETER_H
#define INTERPRETER_H

#ifndef INTERPRETER_IMPL_TEST
    #include <core/window.h>
    #include <core/renderer.h>
    #include <nlohmann/json.hpp>
#endif

#include <string>
#include <array>
#include <chrono>
#include <ctime>

constexpr int DISPLAY_WIDTH = 64, DISPLAY_HEIGHT = 32;

class EmulatorInterpreter
{
public:
    /**
     * @brief The default constructor of the class which automatically invokes the `ResetSystems()` member function.
     */
    EmulatorInterpreter();

    ~EmulatorInterpreter();

    /**
     * @brief Completely hard resets the interpreter system.
     * The interpreter's memory, registers, call stack, key states, timers, and pointers are reset.
     * The interpreter's random engine is re-initialized with a new seed, and the built-in CHIP-8 fontset is reloaded back 
     * into memory.
     */
    void ResetSystem();

    /**
     * @brief Loads the CHIP-8 program contained in the specified binary file. The loaded program is stored in the 
     * interpreter's memory and is immediately executed.
     * 
     * @param[in] filePath The path to the CHIP-8 program binary file.
     */
    void LoadProgram(std::string_view filePath);

#ifndef INTERPRETER_IMPL_TEST
    /**
     * @brief Runs a cycle of the intepreter's execution and handles pending events, such as window, input, etc.
     * @param[in] window The window being used by the emulator.
     */
    void Update(WindowFrame& window);

    /**
     * @brief Renders and displays the current scene.
     * @param[in] renderer The graphics renderer context bound to the emulator's window.
     */
    void Render(GraphicsRenderer& renderer);

    /**
     * @brief Gets whether or not the emulator should terminate.
     * @return `True` if the emulator should terminate execution, otherwise `False` is returned.
     */
    bool ShouldTerminate() const;
private:
    /**
     * @brief Loads the key binding configurations from the file at the specifed path.
     * @param[in] filePath The path to the key bindings configuration file
     */
    void LoadKeyBindingConfig(std::string_view filePath);

    /**
     * @brief Emulates a cycle of the interpreter's execution.
     */
    void ExecuteCycle();
#endif

    /**
     * @brief Decodes and executes the specified opcode.
     * @param[in] opcode The opcode to be decoded and executed.
     */
    void DecodeOpcode(uint16_t opcode);
#ifndef INTERPRETER_IMPL_TEST
private:
    nlohmann::json m_keyBindings;
#endif
    std::array<uint8_t, DISPLAY_WIDTH * DISPLAY_HEIGHT> m_displayBuffer;
    std::array<uint8_t, 4096> m_memory;
    std::array<uint8_t, 16> m_registers;
    std::array<uint16_t, 16> m_stack;
    std::array<bool, 16> m_keys;

    uint16_t m_programCounter, m_addressRegister;
    uint8_t m_delayTimer, m_soundTimer;
    size_t m_stackPointer;
    bool m_shouldRender, m_terminateEmulator;

    std::chrono::steady_clock::time_point m_lastExecuteTime;
};

#endif