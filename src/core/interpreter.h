#ifndef INTERPRETER_H
#define INTERPRETER_H

#ifndef INTERPRETER_IMPL_TEST
    #include <core/window.h>
    #include <core/renderer.h>
    #include <nlohmann/json.hpp>
    #include <SDL3_mixer/SDL_mixer.h>
#endif

#include <string>
#include <array>
#include <chrono>
#include <ctime>
#include <functional>

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
     * @brief Executes the current opcode instruction.
     */
    void DecodeOpcode();

    ////////////////////////////////////// Opcode Functions //////////////////////////////////////

    // Display Operations

    /**
     * @brief This function is executed by opcode `00E0`.
     * 
     * This instruction clears the display buffer, resetting all pixel values to zero.
     */
    void ClearDisplay();

    /**
     * @brief This function is executed by opcode `DXYN`.
     * 
     * This instruction draws a sprite at the coordinates stored in registers `X` and `Y`; sprites has a fixed width of 
     * 8 pixels and their height is defined via the constant `N`. Each row of the sprite (8 pixels) is read as bit-coded from 
     * the memory location stored in the address register. Also, if any screen pixels are flipped from 1 to 0 then 
     * register `F` is set to 1, otherwise it is set to 0.
     */
    void DrawSprite();

    // Flow Operations

    /**
     * @brief This function is executed by opcode `00EE`.
     * 
     * This instruction returns from the current subroutine.
     */
    void SubrountineReturn();

    /**
     * @brief This function is executed by the following opcodes: `1NNN` and `BNNN`.
     * 
     * - For opcode `1NNN`, this instruction jumps to the addresss `NNN`.
     * 
     * - For opcode `BNNN`, this instruction jumps to the address resulting from the addition of `NNN` and the value stored in 
     *   register `0`.
     */
    void JumpTo();

    /**
     * @brief This function is executed by opcode `2NNN`.
     * 
     * This instruction calls the subroutine located at address `NNN`.
     */
    void SubroutineCall();

    /**
     * @brief This function is executed by the following opcodes: `3XNN` and `5XY0`.
     * 
     * - For opcode `3XNN`, this instruction skips the next instruction if the value in register `X` is equal to constant `NN`.
     * 
     * - For opcode `5XY0`, this instruction skips the next instruction if the value in register `X` is equal to the value in
     *   register `Y`.
     */
    void SkipIfEqual();

    /**
     * @brief This function is executed by the following opcodes: `4XNN` and `9XY0`.
     * 
     * - For opcode `4XNN`, this instruction skips the next instruction if the value in register `X` is not equal to constant `NN`.
     * 
     * - For opcode `9XY0`, this instruction skips the next instruction if the value in register `X` is not equal to the value 
     *   in register `Y`.
     */
    void SkipIfNotEqual();
    
    // Arithmetic Operations

    /**
     * @brief This function is executed by the following opcodes: `6XNN` and `8XY0`.
     * 
     * - For opcode `6XNN`, this instruction sets the value of register `X` to constant `NN`.
     * 
     * - For opcode `8XY0`, this instruction sets the value of register `X` to the value of register `Y`.
     */
    void SetValue();

    /**
     * @brief This function is executed by opcode `CXNN`.
     * 
     * This instruction sets the value of register `X` to the bitwise AND of constant `NN` and a random 8-bit integer.
     */
    void SetRandomValue();

    /**
     * @brief This function is executed by the following opcodes: `7XNN` and `8XY4`.
     * 
     * - For opcode `7XNN`, this instruction adds constant `NN` onto the value stored in register `X`.
     * 
     * - For opcode `8XY4`, this instruction adds the value of register `Y` onto the value stored in register `X`.
     * 
     * The value of register F will be set to 1 if an overflow occurs, or to 0 if no overflow occurs; this only applies
     * to opcode instruction `8XY4`.
     */
    void AddValue();

    /**
     * @brief This function is executed by the following opcodes: `8XY5` and `8XY7`.
     * 
     * - For opcode `8XY5`, this instruction subtracts constant `NN` from the value stored in register `X`.

     * - For opcode `8XY7`, this instruction sets the value of register `X` to the result of the value of register `Y` subtract 
     *   the value of register `X`.
     * 
     * The value of register `F` will be set to 0 if an underflow occurs, or to 1 if no underflow occurs; this applies to
     * both opcode instructions.
     */
    void SubtractValue();

    /**
     * @brief This function is executed by opcode `8XY1`.
     * 
     * This instruction sets the value of register `X` to the bitwise OR of the values stored in register `X` and register `Y`.
     */
    void BitwiseOR();

    /**
     * @brief This function is executed by opcode `8XY2`.
     * 
     * This instruction sets the value of register `X` to the bitwise AND of the values stored in register `X` and register `Y`.
     */
    void BitwiseAND();

    /**
     * @brief This function is executed by opcode `8XY3`.
     * 
     * This instruction sets the value of register `X` to the bitwise XOR of the values stored in register `X` and register `Y`.
     */
    void BitwiseXOR();

    /**
     * @brief This function is executed by opcode `8XYE`.
     * 
     * This instruction shifts the value of register `X` to the left by 1. The most significant bit prior to the 
     * operation is stored in register `F`.
     */
    void LeftShiftBits();

    /**
     * @brief This function is executed by opcode `8XY6`.
     * 
     * This instruction shifts the value of register `X` to the right by 1. The least significant bit prior to the 
     * operation is stored in register `F`.
     */
    void RightShiftBits();

    // Memory Operations

    /**
     * @brief This function is executed by the following opcodes: `ANNN`, `FX1E`, and `FX29`.
     * 
     * - For opcode `ANNN`, this instruction sets the value of the address register to address `NNN`.
     * 
     * - For opcode `FX1E`, this instruction adds the value of register `X` onto the value stored in the address register.
     * 
     * - For opcode `FX29`, this instruction sets the value of the address register to the location of font glyph for the 
     *   character (ranges from 0 to F) stored in register X.
     */
    void SetAddressRegister();

    /**
     * @brief This function is executed by opcode `ANNN`.
     * 
     * This instruction stores the binary-coded decimal representation of the value of register `X` in memory. The hundreds 
     * digit is stored at the memory location stored in the address register, the tens digit at memory location + 1, and the 
     * ones digit at memory location + 2.
     */
    void StoreBinaryCodedDecimal();

    /**
     * @brief This function is executed by opcode `FX55`.
     * 
     * This instruction stores the values of registers `0` to `X` in memory, starting at the memory location stored in the 
     * address register.
     */
    void DumpRegisters();

    /**
     * @brief This function is executed by opcode `FX65`.
     * 
     * This instruction fills the registers `0` to `X` with values loaded from memory, starting at the location stored in the 
     * address register.
     */
    void LoadRegisters();

    // Input Operations

    /**
     * @brief This function is executed by opcode `EX9E`.
     * 
     * This instruction skips the next instruction if the key, specified by the value stored in register X, was pressed.
     */
    void SkipIfKeyPressed();

    /**
     * @brief This function is executed by opcode `EXA1`.
     * 
     * This instruction skips the next instruction if the key, specified by the value stored in register X, was not pressed.
     */
    void SkipIfKeyNotPressed();

    /**
     * @brief This function is executed by opcode `FX0A`.
     * 
     * This instruction waits for a key to be pressed then stores the value of the pressed key in register `X`. This 
     * operation blocks execution until a key is pressed.
     */
    void WaitForKeyPress();

    // Timer Operations

    /**
     * @brief This function is executed by opcode `FX15`.
     * 
     * This instruction sets the value of the delay timer to the value of register `X`.
     */
    void SetDelayTimer();

    /**
     * @brief This function is executed by opcode `FX18`.
     * 
     * This instruction sets the value of the delay timer to the value of register `X`.
     */
    void SetSoundTimer();

    /**
     * @brief This function is executed by opcode `FX07`.
     * 
     * This instruction sets the value of register `X` to the current value of the delay timer.
     */
    void GetDelayTimer();

    //////////////////////////////////////////////////////////////////////////////////////////////
#ifndef INTERPRETER_IMPL_TEST
private:
    nlohmann::json m_keyBindings;
    Mix_Chunk* m_beepSound;
#endif
    struct Instruction
    {
        Instruction() = default;
        Instruction(uint16_t opcode, std::function<void()> func) :
            opcode(opcode), func(func)
        {}

        uint16_t opcode;
        std::function<void()> func;
    };

    std::array<Instruction, 34> m_instructionsTable;

    std::array<uint8_t, DISPLAY_WIDTH * DISPLAY_HEIGHT> m_displayBuffer;
    std::array<uint8_t, 4096> m_memory;
    std::array<uint8_t, 16> m_registers;
    std::array<uint16_t, 16> m_stack;
    std::array<bool, 16> m_keys;

    uint16_t m_programCounter, m_addressRegister, m_currentOpcode;
    uint8_t m_delayTimer, m_soundTimer;
    size_t m_stackPointer;
    bool m_shouldRender, m_terminateEmulator;

    std::chrono::steady_clock::time_point m_lastExecuteTime;
};

#endif