#include <core/interpreter.h>
#include <config.h>
#include <random>
#include <ctime>

int GenerateRandomInt(int min, int max);
void LoadProgram_Test();
void DecodeOpcodes_Test();

EmulatorInterpreter interpreter;

int main(int argc, char** argv)
{
    srand((uint32_t)time(nullptr));

    try
    {
        LoadProgram_Test();

        for (int i = 0; i < 10; i++)
        {
            interpreter.ResetSystem();
            DecodeOpcodes_Test();
        }
    }
    catch (const std::exception& e)
    {
        std::printf("%s\n", e.what());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

int GenerateRandomInt(int min, int max) 
{ 
    return (rand() % (max - min)) + min; 
}

/**
 * This test aims to verify whether or not the program data from the binary file was loaded into memory correctly
 * The test c8 file is not functional and is only being used as a means to test the program loading functionality.
 */
void LoadProgram_Test()
{
    const std::array<uint8_t, 20> expectedData = { 0x00, 0xE0, 0xD1, 0x2A, 0x37, 0xFA, 0x89, 0xB0, 0xA0, 0x80, 0xF3, 0x07, 
        0xF9, 0x15, 0xC7, 0xA1, 0xDD, 0xE0, 0x0E, 0x00 };

    interpreter.LoadProgram(TESTS_DIR_PATH + std::string("load_test.c8"));
    for (int i = 0; i < 20; i++)
    {
        const uint8_t& byte = interpreter.m_memory[0x200 + i];
        if (byte != expectedData[i])
            throw std::exception("LoadProgram_Test: Unexpected byte in interpreter memory");
    }
}

/**
 * This test aims to verify that the execution of each opcode gives the expected result outlined by the CHIP-8 specification. 
 * This specification can be found on the CHIP-8 wikipedia page.
 */
void DecodeOpcodes_Test()
{
    // Generate random values for testing
    const uint16_t addressNNN = GenerateRandomInt(0x200, 0xFFF);
    const uint8_t constantNN = GenerateRandomInt(1, 0xFF);
    const uint8_t constantN = GenerateRandomInt(1, 0xF);

    const int registerX = GenerateRandomInt(0, 14);
    int registerY = GenerateRandomInt(0, 14);
    while (registerX == registerY) // Ensure two different registers are picked
        registerY = GenerateRandomInt(0, 14);

    // 00EO opcode instruction test
    memset(interpreter.m_displayBuffer.data(), 1, sizeof(interpreter.m_displayBuffer)); // Set all pixels to 1 (aka visible)
    interpreter.m_currentOpcode = 0x00E0;
    interpreter.DecodeOpcode();

    for (const uint8_t& pixel : interpreter.m_displayBuffer)
    {
        if (pixel != 0)
            throw std::exception("00E0 Instruction_Test: Unexpected display pixel value");
    }

    interpreter.m_shouldRender = false;

    // 00EE opcode instruction test
    interpreter.m_stack[0] = addressNNN;
    interpreter.m_stackPointer++;
    interpreter.m_currentOpcode = 0x00EE;
    interpreter.DecodeOpcode();

    if (interpreter.m_programCounter != (addressNNN + 2))
        throw std::exception("00EE Instruction_Test: Unexpected program counter value");

    // 1NNN opcode instruction test
    interpreter.m_currentOpcode = 0x1000 | addressNNN;
    interpreter.DecodeOpcode();
    if (interpreter.m_programCounter != addressNNN)
        throw std::exception("1NNN Instruction_Test: Unexpected program counter value");

    // 2NNN opcode instruction test
    interpreter.m_programCounter = 0x200;
    interpreter.m_currentOpcode = 0x2000 | addressNNN;
    interpreter.DecodeOpcode();

    if (interpreter.m_stack[0] != 0x200)
        throw std::exception("2NNN Instruction_Test: Unexpected call stack value");

    if (interpreter.m_programCounter != addressNNN)
        throw std::exception("2NNN Instruction_Test_2: Unexpected program counter value");

    // 3XNN opcode instruction test
    interpreter.m_programCounter = 0x200;
    interpreter.m_registers[registerX] = constantNN;
    interpreter.m_currentOpcode = 0x3000 | (registerX << 8) | constantNN;
    interpreter.DecodeOpcode();

    if (interpreter.m_programCounter != 0x204) // Values are equal, hence the next instruction should've been skipped
        throw std::exception("3XNN Instruction_Test: Unexpected program counter value");

    interpreter.m_registers[registerX] = 0;
    interpreter.m_currentOpcode = 0x3000 | (registerX << 8) | constantNN;
    interpreter.DecodeOpcode();
    
    if (interpreter.m_programCounter != 0x206) // Values aren't equal, hence the next instruction shouldn't have been skipped
        throw std::exception("3XNN Instruction_Test_2: Unexpected program counter value");

    // 4XNN opcode instruction test
    interpreter.m_programCounter = 0x200;
    interpreter.m_currentOpcode = 0x4000 | (registerX << 8) | constantNN;
    interpreter.DecodeOpcode();

    if (interpreter.m_programCounter != 0x204) // Values aren't equal, hence the next instruction should've been skipped
        throw std::exception("4XNN Instruction_Test: Unexpected program counter value");

    interpreter.m_registers[registerX] = constantNN;
    interpreter.m_currentOpcode = 0x4000 | (registerX << 8) | constantNN;
    interpreter.DecodeOpcode();

    if (interpreter.m_programCounter != 0x206) // Values are equal, hence the next instruction shouldn't have been skipped
        throw std::exception("4XNN Instruction_Test_2: Unexpected program counter value");

    // 5XY0 opcode instruction test
    interpreter.m_programCounter = 0x200;
    interpreter.m_registers[registerX] = interpreter.m_registers[registerY] = constantNN;
    interpreter.m_currentOpcode = 0x5000 | (registerX << 8) | (registerY << 4);
    interpreter.DecodeOpcode();
    
    if (interpreter.m_programCounter != 0x204) // Values are equal, hence the next instruction should've been skipped
        throw std::exception("5XY0 Instruction_Test: Unexpected program counter value");

    interpreter.m_registers[registerX] = 0;
    interpreter.m_currentOpcode = 0x5000 | (registerX << 8) | (registerY << 4);
    interpreter.DecodeOpcode();

    if (interpreter.m_programCounter != 0x206) // Values aren't equal, hence the next instruction shouldn't have been skipped
        throw std::exception("5XY0 Instruction_Test_2: Unexpected program counter value");

    // 6XNN opcode instruction test
    interpreter.m_currentOpcode = 0x6000 | (registerX << 8) | constantNN;
    interpreter.DecodeOpcode();
    if (interpreter.m_registers[registerX] != constantNN)
        throw std::exception("6XNN Instruction_Test: Unexpected register value");

    // 7XNN opcode instruction test
    interpreter.m_registers[registerX] = constantN;
    interpreter.m_currentOpcode = 0x7000 | (registerX << 8) | constantNN;
    interpreter.DecodeOpcode();

    if (interpreter.m_registers[registerX] != (uint8_t)(constantN + constantNN))
        throw std::exception("7XNN Instruction_Test: Unexpected register value");

    // 8XY0 opcode instruction test
    interpreter.m_registers[registerX] = constantN;
    interpreter.m_registers[registerY] = constantNN;
    interpreter.m_currentOpcode = 0x8000 | (registerX << 8) | (registerY << 4);
    interpreter.DecodeOpcode();
    
    if (interpreter.m_registers[registerX] != interpreter.m_registers[registerY])
        throw std::exception("8XY0 Instruction_Test: Unexpected register value");

    // 8XY1 opcode instruction test
    interpreter.m_registers[registerX] = constantN;
    interpreter.m_registers[registerY] = constantNN;
    interpreter.m_currentOpcode = 0x8001 | (registerX << 8) | (registerY << 4);
    interpreter.DecodeOpcode();

    if (interpreter.m_registers[registerX] != (uint8_t)(constantN | interpreter.m_registers[registerY]))
        throw std::exception("8XY1 Instruction_Test: Unexpected register value");

    // 8XY2 opcode instruction test
    interpreter.m_registers[registerX] = constantN;
    interpreter.m_registers[registerY] = constantNN;
    interpreter.m_currentOpcode = 0x8002 | (registerX << 8) | (registerY << 4);
    interpreter.DecodeOpcode();

    if (interpreter.m_registers[registerX] != (uint8_t)(constantN & interpreter.m_registers[registerY]))
        throw std::exception("8XY2 Instruction_Test: Unexpected register value");

    // 8XY3 opcode instruction test
    interpreter.m_registers[registerX] = constantN;
    interpreter.m_registers[registerY] = constantNN;
    interpreter.m_currentOpcode = 0x8003 | (registerX << 8) | (registerY << 4);
    interpreter.DecodeOpcode();

    if (interpreter.m_registers[registerX] != (uint8_t)(constantN ^ interpreter.m_registers[registerY]))
        throw std::exception("8XY3 Instruction_Test: Unexpected register value");

    // 8XY4 opcode instruction test
    interpreter.m_registers[registerX] = constantNN;
    interpreter.m_registers[registerY] = 0xFF - constantNN;
    interpreter.m_currentOpcode = 0x8004 | (registerX << 8) | (registerY << 4);
    interpreter.DecodeOpcode();

    if (interpreter.m_registers[registerX] != (uint8_t)(constantNN + interpreter.m_registers[registerY]))
        throw std::exception("8XY4 Instruction_Test: Unexpected register value");

    if (interpreter.m_registers[0xF] != 0)
        throw std::exception("8XY4 Instruction_Test: Unexpected carry flag value");

    interpreter.m_currentOpcode = 0x8004 | (registerX << 8) | (registerY << 4);
    interpreter.DecodeOpcode();
    if (interpreter.m_registers[0xF] != 1)
        throw std::exception("8XY4 Instruction_Test_2: Unexpected carry flag value");

    // 8XY5 opcode instruction test
    interpreter.m_registers[registerX] = interpreter.m_registers[registerY] = constantNN;
    interpreter.m_currentOpcode = 0x8005 | (registerX << 8) | (registerY << 4);
    interpreter.DecodeOpcode();

    if (interpreter.m_registers[registerX] != 0)
        throw std::exception("8XY5 Instruction_Test: Unexpected register value");

    if (interpreter.m_registers[0xF] != 1)
        throw std::exception("8XY5 Instruction_Test: Unexpected underflow flag value");

    interpreter.m_currentOpcode = 0x8005 | (registerX << 8) | (registerY << 4);
    interpreter.DecodeOpcode();
    if (interpreter.m_registers[0xF] != 0)
        throw std::exception("8XY5 Instruction_Test_2: Unexpected underflow flag value");

    // 8XY6 opcode instruction test (technically 8X06 since register Y is ignored in this implementation)
    interpreter.m_registers[registerX] = constantNN;
    interpreter.m_currentOpcode = 0x8006 | (registerX << 8);
    interpreter.DecodeOpcode();

    if (interpreter.m_registers[registerX] != (uint8_t)(constantNN >> 1))
        throw std::exception("8XY6 Instruction_Test: Unexpected register value");

    if (interpreter.m_registers[0xF] != (constantNN & 0x1))
        throw std::exception("8XY6 Instruction_Test: Unexpected carry bit value");

    // 8XY7 opcode instruction test
    interpreter.m_registers[registerX] = interpreter.m_registers[registerY] = constantNN;
    interpreter.m_currentOpcode = 0x8007 | (registerX << 8) | (registerY << 4);
    interpreter.DecodeOpcode();

    if (interpreter.m_registers[registerX] != 0)
        throw std::exception("8XY7 Instruction_Test: Unexpected register value");

    if (interpreter.m_registers[0xF] != 1)
        throw std::exception("8XY7 Instruction_Test: Unexpected underflow flag value");

    interpreter.m_registers[registerX] = constantNN;
    interpreter.m_registers[registerY] = 0;
    interpreter.m_currentOpcode = 0x8007 | (registerX << 8) | (registerY << 4);
    interpreter.DecodeOpcode();

    if (interpreter.m_registers[0xF] != 0)
        throw std::exception("8XY7 Instruction_Test_2: Unexpected underflow flag value");

    // 8XYE opcode instruction test (technically 8X06 since register Y is ignored in this implementation)
    interpreter.m_registers[registerX] = constantNN;
    interpreter.m_currentOpcode = 0x800E | (registerX << 8);
    interpreter.DecodeOpcode();

    if (interpreter.m_registers[registerX] != (uint8_t)(constantNN << 1))
        throw std::exception("8XYE Instruction_Test: Unexpected register value");

    if (interpreter.m_registers[0xF] != (uint8_t)(constantNN >> 7))
        throw std::exception("8XYE Instruction_Test: Unexpected carry bit value");

    // 9XY0 opcode instruction test
    interpreter.m_programCounter = 0x200;
    interpreter.m_registers[registerX] = constantNN;
    interpreter.m_registers[registerY] = 0;
    interpreter.m_currentOpcode = 0x9000 | (registerX << 8) | (registerY << 4);
    interpreter.DecodeOpcode();
    
    if (interpreter.m_programCounter != 0x204) // Values aren't equal, hence the next instruction should've been skipped
        throw std::exception("9XY0 Instruction_Test: Unexpected program counter value");

    interpreter.m_registers[registerY] = constantNN;
    interpreter.m_currentOpcode = 0x9000 | (registerX << 8) | (registerY << 4);
    interpreter.DecodeOpcode();

    if (interpreter.m_programCounter != 0x206) // Values are equal, hence the next instruction shouldn't have been skipped
        throw std::exception("9XY0 Instruction_Test_2: Unexpected program counter value");

    // ANNN opcode instruction test
    interpreter.m_currentOpcode = 0xA000 | addressNNN;
    interpreter.DecodeOpcode();
    if (interpreter.m_addressRegister != addressNNN)
        throw std::exception("ANNN Instruction_Test: Unexpected address register value");

    // BNNN opcode instruction test
    interpreter.m_registers[0] = constantNN;
    interpreter.m_currentOpcode = 0xB000 | addressNNN;
    interpreter.DecodeOpcode();

    if (interpreter.m_programCounter != interpreter.m_registers[0] + addressNNN)
        throw std::exception("BNNN Instruction_Test: Unexpected program counter value");

    // Skipped the CXNN opcode since its output is randomised hence being difficult to predict
    // DXYN opcode instruction test
    const int xPos = interpreter.m_registers[registerX] = (uint8_t)GenerateRandomInt(0, DISPLAY_WIDTH);
    const int yPos = interpreter.m_registers[registerY] = (uint8_t)GenerateRandomInt(0, DISPLAY_HEIGHT);
    interpreter.m_memory[0x200] = interpreter.m_memory[0x201] = 0xC0; // 11000000 (1 being a set pixel, and 0 being unset)
    interpreter.m_addressRegister = 0x200;

    interpreter.m_currentOpcode = 0xD000 | (registerX << 8) | (registerY << 4) | 0x2;
    interpreter.DecodeOpcode();

    for (int y = 0; y < DISPLAY_HEIGHT; y++)
    {
        for (int x = 0; x < DISPLAY_WIDTH; x++)
        {
            const uint8_t pixel = interpreter.m_displayBuffer[x + (y * DISPLAY_WIDTH)];
            if ((x == xPos || x == ((xPos + 1) % DISPLAY_WIDTH)) && (y == yPos || y == ((yPos + 1) % DISPLAY_HEIGHT)))
            {  
                if (pixel != 1)
                    throw std::exception("DXYN Instruction_Test: Unexpected pixel value");
            }
            else
            {
                if (pixel != 0)
                    throw std::exception("DXYN Instruction_Test: Unexpected pixel value");
            }
        }
    }

    // EX9E opcode instruction test
    interpreter.m_programCounter = 0x200;
    interpreter.m_registers[registerX] = constantN; // Store keycode N
    interpreter.m_keys[constantN] = true;

    interpreter.m_currentOpcode = 0xE09E | (registerX << 8);
    interpreter.DecodeOpcode();

    if (interpreter.m_programCounter != 0x204) // The key state is set as pressed, so the next instruction should've been skipped
        throw std::exception("EX9E Instruction_Test: Unexpected program counter value");

    interpreter.m_registers[registerX] = 0; // Store keycode 0x0
    interpreter.m_currentOpcode = 0xE09E | (registerX << 8);
    interpreter.DecodeOpcode();

    if (interpreter.m_programCounter != 0x206) // The key state is set as not pressed, so the next instruction should've not been skipped
        throw std::exception("EX9E Instruction_Test_2: Unexpected program counter value");

    // EXA1 opcode instruction test
    interpreter.m_programCounter = 0x200;
    interpreter.m_registers[registerX] = constantN; // Store keycode N
    interpreter.m_keys[constantN] = false;
    interpreter.m_currentOpcode = 0xE0A1 | (registerX << 8);
    interpreter.DecodeOpcode();

    if (interpreter.m_programCounter != 0x204) // The key state is set as not pressed, so the next instruction should've been skipped
        throw std::exception("EXA1 Instruction_Test: Unexpected program counter value");

    interpreter.m_registers[registerX] = 0; // Store keycode 0x0
    interpreter.m_keys[0x0] = true;
    interpreter.m_currentOpcode = 0xE0A1 | (registerX << 8);
    interpreter.DecodeOpcode();

    if (interpreter.m_programCounter != 0x206) // The key state is set as pressed, so the next instruction should've not been skipped
        throw std::exception("EXA1 Instruction_Test_2: Unexpected program counter value");

    // FX07 opcode instruction test
    interpreter.m_delayTimer = constantN;
    interpreter.m_currentOpcode = 0xF007 | (registerX << 8);
    interpreter.DecodeOpcode();

    if (interpreter.m_registers[registerX] != constantN)
        throw std::exception("FX07 Instruction_Test: Unexpected register value");

    // FX0A opcode instruction test
    interpreter.m_programCounter = 0x200;
    interpreter.m_registers[registerX] = 0;
    memset(interpreter.m_keys.data(), 0, sizeof(interpreter.m_keys));

    interpreter.m_currentOpcode = 0xF00A | (registerX << 8);
    interpreter.DecodeOpcode();

    if (interpreter.m_programCounter != 0x200)
        throw std::exception("FX0A Instruction_Test: Unexpected program counter value");

    if (interpreter.m_registers[registerX] != 0)
        throw std::exception("FX0A Instruction_Test: Unexpected register value");

    interpreter.m_keys[constantN] = true;
    interpreter.m_currentOpcode = 0xF00A | (registerX << 8);
    interpreter.DecodeOpcode();

    if (interpreter.m_programCounter != 0x202)
        throw std::exception("FX0A Instruction_Test_2: Unexpected program counter value");

    if (interpreter.m_registers[registerX] != constantN)
        throw std::exception("FX0A Instruction_Test_2: Unexpected register value");

    // FX15 opcode instruction test
    interpreter.m_registers[registerX] = constantNN;
    interpreter.m_currentOpcode = 0xF015 | (registerX << 8);
    interpreter.DecodeOpcode();

    if (interpreter.m_delayTimer != constantNN)
        throw std::exception("FX15 Instruction_Test: Unexpected delay timer value");

    // FX18 opcode instruction test
    interpreter.m_registers[registerX] = constantNN;
    interpreter.m_currentOpcode = 0xF018 | (registerX << 8);
    interpreter.DecodeOpcode();

    if (interpreter.m_soundTimer != constantNN)
        throw std::exception("FX18 Instruction_Test: Unexpected sound timer value");

    // FX1E opcode instruction test
    interpreter.m_registers[registerX] = constantN;
    interpreter.m_addressRegister = constantNN;
    interpreter.m_currentOpcode = 0xF01E | (registerX << 8);
    interpreter.DecodeOpcode();

    if (interpreter.m_addressRegister != constantNN + constantN)
        throw std::exception("FX1E Instruction_Test: Unexpected address register value");

    // FX29 opcode instruction test
    interpreter.m_registers[registerX] = constantN;
    interpreter.m_currentOpcode = 0xF029 | (registerX << 8);
    interpreter.DecodeOpcode();

    if (interpreter.m_addressRegister != (constantN * 5))
        throw std::exception("FX29 Instruction_Test: Unexpected address register value");

    // FX33 opcode instruction test
    interpreter.m_registers[registerX] = constantNN;
    interpreter.m_addressRegister = 0x200;
    interpreter.m_currentOpcode = 0xF033 | (registerX << 8);
    interpreter.DecodeOpcode();
    
    if (interpreter.m_memory[0x200] != (constantNN / 100))
        throw std::exception("FX33 Instruction_Test: Unexpected value at memory location 0x200");

    if (interpreter.m_memory[0x201] != ((constantNN % 100) / 10))
        throw std::exception("FX33 Instruction_Test: Unexpected value at memory location 0x201");

    if (interpreter.m_memory[0x202] != ((constantNN % 100) % 10))
        throw std::exception("FX33 Instruction_Test: Unexpected value at memory location 0x202");

    // FX55 opcode instruction test
    interpreter.m_addressRegister = 0x200;
    for (uint8_t i = 0; i <= registerX; i++)
        interpreter.m_registers[i] = (uint8_t)GenerateRandomInt(0, 255);

    interpreter.m_currentOpcode = 0xF055 | (registerX << 8);
    interpreter.DecodeOpcode();
    for (uint8_t i = 0; i <= registerX; i++)
    {
        if (interpreter.m_memory[interpreter.m_addressRegister + i] != interpreter.m_registers[i])
        {
            throw std::exception(("FX55 Instruction_Test: Unexpected value at memory location " + 
                std::to_string(interpreter.m_addressRegister + i)).c_str());
        }
    }

    // FX65 opcode instruction test
    interpreter.m_addressRegister = 0x200;
    for (uint8_t i = 0; i <= registerX; i++)
        interpreter.m_memory[interpreter.m_addressRegister + i] = (uint8_t)GenerateRandomInt(0, 255);

    interpreter.m_currentOpcode = 0xF065 | (registerX << 8);
    interpreter.DecodeOpcode();
    for (uint8_t i = 0; i <= registerX; i++)
    {
        if (interpreter.m_memory[interpreter.m_addressRegister + i] != interpreter.m_registers[i])
            throw std::exception("FX55 Instruction_Test: Unexpected register value");
    }
}