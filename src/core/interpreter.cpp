#include <core/interpreter.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>

constexpr uint8_t CHIP_8_FONTSET[80] = 
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

EmulatorInterpreter::EmulatorInterpreter() { this->ResetSystem(); }

EmulatorInterpreter::~EmulatorInterpreter()
{
#ifndef INTERPRETER_IMPL_TEST
    std::ofstream file("key_bindings.json");
    file << m_keyBindings.dump(4);
#endif
}

void EmulatorInterpreter::ResetSystem()
{
    m_addressRegister = m_delayTimer = m_soundTimer = 0;
    m_programCounter = 0x200;
    m_stackPointer = -1;
    m_shouldRender = false;

#ifndef INTERPRETER_IMPL_TEST
    m_terminateEmulator = false;
    this->LoadKeyBindingConfig("key_bindings.json");
#endif
    
    memset(m_memory.data(), 0, sizeof(m_memory));
    memset(m_registers.data(), 0, sizeof(m_registers));
    memset(m_keys.data(), 0, sizeof(m_keys));
    memset(m_displayBuffer.data(), 0, sizeof(m_displayBuffer));
    memset(m_stack.data(), 0, sizeof(m_stack));

    std::srand((uint32_t)time(nullptr)); // Initialize random engine seed
    memcpy(m_memory.data(), CHIP_8_FONTSET, sizeof(CHIP_8_FONTSET)); // Load fontset into memory
}

void EmulatorInterpreter::LoadProgram(std::string_view filePath)
{
    std::ifstream programFile(filePath.data(), std::ios::binary); // Open the file in binary mode
    if (programFile.fail())
        throw std::runtime_error("Failed to open CHIP-8 program file");

    // Get the size of the file
    programFile.seekg(0, std::ios::end);
    const int fileSize = (int)programFile.tellg();
    programFile.seekg(0);

    // Read the file contents into an array
    std::vector<uint8_t> buffer(fileSize);
    programFile.read((char*)buffer.data(), fileSize);

    for (size_t i = 0; i < buffer.size(); i++)
        m_memory[0x200 + i] = buffer[i];
}

void EmulatorInterpreter::DecodeOpcode(uint16_t opcode)
{
    const int registerX = (opcode & 0x0F00) >> 8;
    const int registerY = (opcode & 0x00F0) >> 4;
    const uint8_t constantNN = opcode & 0x00FF;
    const uint16_t addressNNN = opcode & 0x0FFF;

    switch (opcode & 0xF000)
    {
    case 0x0000:
        switch (opcode & 0x00FF)
        {
        case 0x00E0: 
            // This opcode clears the display
            memset(m_displayBuffer.data(), 0, sizeof(m_displayBuffer));
            m_shouldRender = true;
            m_programCounter += 2;
            break;
        case 0x00EE: 
            // This opcode returns from a subroutine
            m_programCounter = m_stack[m_stackPointer];
            m_stackPointer--;
            m_programCounter += 2;
            break;
        }
        break;
    case 0x1000: 
        // This opcode jumps to specified address NNN
        m_programCounter = addressNNN;
        break;
    case 0x2000: 
        // This opcode calls a subroutine at the specified address NNN
        m_stack[++m_stackPointer] = m_programCounter;
        m_programCounter = addressNNN;
        break;
    case 0x3000: 
        // This opcode skips the next instruction if the value of register X is equal to the specified constant NN
        if (m_registers[registerX] == constantNN)
            m_programCounter += 4;
        else
            m_programCounter += 2;
        break;
    case 0x4000: 
        // This opcode skips the next instruction if the value of register X is not equal to the specified constant NN
        if (m_registers[registerX] != constantNN)
            m_programCounter += 4;
        else
            m_programCounter += 2;
        break;
    case 0x5000:
        // This opcode skips the next instruction if the values of registers X and Y are equal
        if (m_registers[registerX] == m_registers[registerY])
            m_programCounter += 4;
        else
            m_programCounter += 2;
        break;
    case 0x6000:
        // This opcode sets the value of register X to the specified constant NN
        m_registers[registerX] = constantNN;
        m_programCounter += 2;
        break;
    case 0x7000:
        // This opcode adds the specified constant NN onto the value of register X (the value of register F is not affected 
        // by this operation)
        m_registers[registerX] += constantNN;
        m_programCounter += 2;
        break;
    case 0x8000:
        switch (opcode & 0x000F)
        {
        case 0x0000:
            // This opcode sets the value of register X to the value of register Y
            m_registers[registerX] = m_registers[registerY];
            break;
        case 0x0001:
            // This opcode sets the value of register X to the bitwise [OR] of register X and Y
            m_registers[registerX] |= m_registers[registerY];
            break;
        case 0x0002:
            // This opcode sets the value of register X to the bitwise [AND] of register X and Y
            m_registers[registerX] &= m_registers[registerY];
            break;
        case 0x0003:
            // This opcode sets the value of register X to the bitwise [XOR] of register X and Y
            m_registers[registerX] ^= m_registers[registerY];
            break;
        case 0x0004:
            // This opcode adds the value of register Y onto the value of register X (the value of register F is set to 1 
            // if there is an overflow, and to 0 if there is not)
            if ((uint8_t)(m_registers[registerX] + m_registers[registerY]) < m_registers[registerX])
                m_registers[0xF] = 1;
            else
                m_registers[0xF] = 0;

            m_registers[registerX] += m_registers[registerY];
            break;
        case 0x0005:
            // This opcode subtracts the value of register Y from the value of register X (the value of register F is 
            // set to 0 if there is an underflow, and to 1 if there is not)
            if (m_registers[registerY] > m_registers[registerX])
                m_registers[0xF] = 0;
            else
                m_registers[0xF] = 1;

            m_registers[registerX] -= m_registers[registerY];
            break;
        case 0x0006:
            // This opcode shifts the value of register X to the right by 1 (the least significant bit prior to the 
            // operation is stored in register F)
            {
                const uint8_t leastSignifBit = m_registers[registerX] & 0x1;
                m_registers[registerX] >>= 1;
                m_registers[0xF] = leastSignifBit;
            }
            break;
        case 0x0007:
            // This opcode sets the value of register X to the result of register Y subtract register X (the value of 
            // register F is set to 0 if there is an underflow, and to 1 if there is not)
            if (m_registers[registerX] > m_registers[registerY])
                m_registers[0xF] = 0;
            else
                m_registers[0xF] = 1;

            m_registers[registerX] = m_registers[registerY] - m_registers[registerX];
            break;
        case 0x000E:
            // This operand shifts the value of register X to the left by 1 (the most significant bit prior to the 
            // operation is stored in register F)
            {
                const uint8_t mostSignifBit = m_registers[registerX] >> 7;
                m_registers[registerX] <<= 1;
                m_registers[0xF] = mostSignifBit;
            }
            break;
        }

        m_programCounter += 2;
        break;
    case 0x9000:
        // This opcode skips the next instruction if the values of registers X and Y are not equal
        if (m_registers[registerX] != m_registers[registerY])
            m_programCounter += 4;
        else
            m_programCounter += 2;
        break;
    case 0xA000:
        // This opcode skips the next instruction if the values of registers X and Y are not equal
        m_addressRegister = addressNNN;
        m_programCounter += 2;
        break;
    case 0xB000:
        // This opcode jumps to the specifed address NNN plus the value of register 0
        m_programCounter = addressNNN + m_registers[0x0];
        break;
    case 0xC000:
        // This opcode sets the value of register X to the bitwise [AND] of a random integer (from 0 to 255) and the
        // specified constant NN
        m_registers[registerX] = (uint8_t)(rand() % 0xFF) & constantNN;
        m_programCounter += 2;
        break;
    case 0xD000:
        // This operand draws a sprite at the coordinates stored in registers X and Y that has a fixed width of 8 pixels and
        // a height of specified constant N pixels. Each row of the sprite (8 pixels) is read as bit-coded from the memory
        // location stored in the address register. If any screen pixels are flipped from 1 to 0 then register F is set to 1
        // otherwise it is set to 0.
        {
            const uint8_t x = m_registers[registerX];
            const uint8_t y = m_registers[registerY];
            const uint8_t height = opcode & 0x000F;

            m_registers[0xF] = 0;
            for (int row = 0; row < height; row++)
            {
                const uint8_t spriteRow = m_memory[m_addressRegister + row];
                for (int column = 0; column < 8; column++)
                {
                    if ((spriteRow & (0x80 >> column)) != 0)
                    {
                        const int pixelPosX = (x + column) % DISPLAY_WIDTH;
                        const int pixelPosY = (y + row) % DISPLAY_HEIGHT;

                        uint8_t& pixel = m_displayBuffer[pixelPosX + (pixelPosY * DISPLAY_WIDTH)];
                        if (pixel == 1)
                            m_registers[0xF] = 1;

                        pixel ^= 1;
                    }
                }
            }

            m_shouldRender = true;
            m_programCounter += 2;
        }
        break;
    case 0xE000:
        switch (opcode & 0x00FF)
        {
        case 0x009E:
            // This opcode skips the next instruction if the key specified by the value of register X was pressed.
            {
                const uint8_t keycode = m_registers[registerX];
                if (m_keys[keycode])
                    m_programCounter += 4;
                else
                    m_programCounter += 2;
            }
            break;
        case 0x00A1:
            // This opcode skips the next instruction if the key specified by the value of register X was not pressed.
            {
                const uint8_t keycode = m_registers[registerX];
                if (!m_keys[keycode])
                    m_programCounter += 4;
                else
                    m_programCounter += 2;
            }
            break;
        }
        break;
    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x0007:
            // This opcode sets the value of register X to the current value of the delay timer.
            m_registers[registerX] = m_delayTimer;
            m_programCounter += 2;
            break;
        case 0x000A:
            // This opcode waits for a key to be pressed then stores the key in register X (this operation blocks execution
            // until a key is pressed)
            {
                bool wasKeyPressed = false;
                for (int i = 0; i < 0xF; i++)
                {
                    if (m_keys[i])
                    {
                        m_registers[registerX] = (uint8_t)i;
                        wasKeyPressed = true;
                    }
                }

                if (wasKeyPressed)
                        m_programCounter += 2;
            }
            break;
        case 0x0015:
            // This operand sets the value of the delay timer to the value of register X
            m_delayTimer = m_registers[registerX];
            m_programCounter += 2;
            break;
        case 0x0018:
            // This operand sets the value of the sound timer to the value of register X
            m_soundTimer = m_registers[registerX];
            m_programCounter += 2;
            break;
        case 0x001E:
            // This operand adds the value of register X onto the value of the address register.
            m_addressRegister += m_registers[registerX];
            m_programCounter += 2;
            break;
        case 0x0029:
            // This operand sets the value of the address register to the location of sprite for the character
            // (0-F in hexadecimal) stored in register X.
            {
                const uint8_t character = m_registers[registerX];
                m_addressRegister = character * 5;
                m_programCounter += 2;
            }
            break;
        case 0x0033:
            // This opcode stores the binary-coded decimal representation of the value of register X in memory, with the 
            // hundreds digit being stored at the memory location stored in the address register, 
            // the tens digit at location + 1, and the ones digit at location + 2.
            m_memory[m_addressRegister] = m_registers[registerX] / 100;
            m_memory[m_addressRegister + 1] = (m_registers[registerX] / 10) % 10;
            m_memory[m_addressRegister + 2] = (m_registers[registerX] % 100) % 10;
            m_programCounter += 2;
            break;
        case 0x0055:
            // This opcode stores the values in registers 0 to X in memory, starting at the memory location stored in the 
            // address register.
            for (int i = 0; i <= registerX; i++)
                m_memory[m_addressRegister + i] = m_registers[i];

            m_programCounter += 2;
            break;
        case 0x0065:
            // This opcode fills the registers 0 to X with values loaded from memory, starting at the location stored in the 
            // address register.
            for (int i = 0; i <= registerX; i++)
                m_registers[i] = m_memory[m_addressRegister + i];

            m_programCounter += 2;
            break;
        }
        break;
    default:
        m_terminateEmulator = true;
        break;
    }
}

#ifndef INTERPRETER_IMPL_TEST

void EmulatorInterpreter::LoadKeyBindingConfig(std::string_view filePath)
{
    std::ifstream file(filePath.data());
    if(file.fail())
    {
        m_keyBindings = 
        {
            { "1", SDLK_1 },
            { "2", SDLK_2 },
            { "3", SDLK_3 },
            { "4", SDLK_Q },
            { "5", SDLK_W },
            { "6", SDLK_E },
            { "7", SDLK_A },
            { "8", SDLK_S },
            { "9", SDLK_D },
            { "A", SDLK_Z },
            { "B", SDLK_C },
            { "C", SDLK_4 },
            { "D", SDLK_R },
            { "E", SDLK_F },
            { "F", SDLK_V }
        };

        std::printf("Warning: Key bindings config file not found, using default instead\n");
    }
    else
        m_keyBindings = nlohmann::json::parse(file);   
}

void EmulatorInterpreter::ExecuteCycle()
{
    const uint16_t currentOpcode = (m_memory[m_programCounter] << 8) | m_memory[m_programCounter + 1];
    this->DecodeOpcode(currentOpcode);

    // Update timers
    if (m_delayTimer > 0)
        m_delayTimer--;

    if (m_soundTimer > 0)
    {
        if (m_soundTimer == 1)
            std::printf("[Temporary] BEEP!!!\n"); 

        m_soundTimer--;
    }
}

void EmulatorInterpreter::Update(WindowFrame& window)
{
    constexpr int CLOCK_SPEED_HZ = 60; // The execution speed of the emulator (in Hertz)

    // Limit the amount of opcode instructions executed per second
    // This limit is defined via the constant integer CLOCK_SPEED_HZ
    const std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
    const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_lastExecuteTime).count();
    if (elapsedTime >= 1000 / CLOCK_SPEED_HZ)
    {
        this->ExecuteCycle();

        // Handle emulator window events
        SDL_Event event;
        while (window.PollEvents(event))
        {
            if (event.type == SDL_EVENT_KEY_DOWN) // Check if any bound keys are pressed
            {
                for (int hexKey = 0; hexKey < 0xF; hexKey++)
                {
                    if (event.key.key == m_keyBindings[std::string(1, hexKey < 10 ? '0' + hexKey : 'A' + (hexKey - 10))])
                    {
                        m_keys[hexKey] = true;
                        break;
                    }
                }
            }
            else if (event.type == SDL_EVENT_KEY_UP) // Check if any bound keys are released
            {
                for (int hexKey = 0; hexKey < 0xF; hexKey++)
                {
                    if (event.key.key == m_keyBindings[std::string(1, hexKey < 10 ? '0' + hexKey : 'A' + (hexKey - 10))])
                    {
                        m_keys[hexKey] = false;
                        break;
                    }
                }
            }
            else if (event.type == SDL_EVENT_QUIT) // Check if user wants to close the window
                m_terminateEmulator = true;
        }

        m_lastExecuteTime = currentTime;
    }
}

void EmulatorInterpreter::Render(GraphicsRenderer& renderer)
{
    if (m_shouldRender)
    {
        renderer.Clear();

        for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++)
        {
            if (m_displayBuffer[i] == 1)
                renderer.DrawRect({ (i % DISPLAY_WIDTH) * 10, (int)(i / DISPLAY_WIDTH) * 10 }, { 10, 10 });
        }

        renderer.Update();
        m_shouldRender = false;
    }
}

bool EmulatorInterpreter::ShouldTerminate() const { return m_terminateEmulator; }

#endif