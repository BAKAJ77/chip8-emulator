#include <core/interpreter.h>
#include <debugging.h>
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

EmulatorInterpreter::EmulatorInterpreter() 
{ 
    this->ResetSystem(); 

    // Initialize the array of opcode function pointers
    m_instructionsTable = 
    { 
        Instruction(0x00E0, std::bind(&EmulatorInterpreter::ClearDisplay, this)), 
        Instruction(0x00EE, std::bind(&EmulatorInterpreter::SubrountineReturn, this)),
        Instruction(0x1000, std::bind(&EmulatorInterpreter::JumpTo, this)),
        Instruction(0x2000, std::bind(&EmulatorInterpreter::SubroutineCall, this)),
        Instruction(0x3000, std::bind(&EmulatorInterpreter::SkipIfEqual, this)),
        Instruction(0x4000, std::bind(&EmulatorInterpreter::SkipIfNotEqual, this)),
        Instruction(0x5000, std::bind(&EmulatorInterpreter::SkipIfEqual, this)),
        Instruction(0x6000, std::bind(&EmulatorInterpreter::SetValue, this)),
        Instruction(0x7000, std::bind(&EmulatorInterpreter::AddValue, this)),
        Instruction(0x8000, std::bind(&EmulatorInterpreter::SetValue, this)),
        Instruction(0x8001, std::bind(&EmulatorInterpreter::BitwiseOR, this)),
        Instruction(0x8002, std::bind(&EmulatorInterpreter::BitwiseAND, this)),
        Instruction(0x8003, std::bind(&EmulatorInterpreter::BitwiseXOR, this)),
        Instruction(0x8004, std::bind(&EmulatorInterpreter::AddValue, this)),
        Instruction(0x8005, std::bind(&EmulatorInterpreter::SubtractValue, this)),
        Instruction(0x8006, std::bind(&EmulatorInterpreter::RightShiftBits, this)),
        Instruction(0x8007, std::bind(&EmulatorInterpreter::SubtractValue, this)),
        Instruction(0x800E, std::bind(&EmulatorInterpreter::LeftShiftBits, this)),
        Instruction(0x9000, std::bind(&EmulatorInterpreter::SkipIfNotEqual, this)),
        Instruction(0xA000, std::bind(&EmulatorInterpreter::SetAddressRegister, this)),
        Instruction(0xB000, std::bind(&EmulatorInterpreter::JumpTo, this)),
        Instruction(0xC000, std::bind(&EmulatorInterpreter::SetRandomValue, this)),
        Instruction(0xD000, std::bind(&EmulatorInterpreter::DrawSprite, this)),
        Instruction(0xE09E, std::bind(&EmulatorInterpreter::SkipIfKeyPressed, this)),
        Instruction(0xE0A1, std::bind(&EmulatorInterpreter::SkipIfKeyNotPressed, this)),
        Instruction(0xF007, std::bind(&EmulatorInterpreter::GetDelayTimer, this)),
        Instruction(0xF00A, std::bind(&EmulatorInterpreter::WaitForKeyPress, this)),
        Instruction(0xF015, std::bind(&EmulatorInterpreter::SetDelayTimer, this)),
        Instruction(0xF018, std::bind(&EmulatorInterpreter::SetSoundTimer, this)),
        Instruction(0xF01E, std::bind(&EmulatorInterpreter::SetAddressRegister, this)),
        Instruction(0xF029, std::bind(&EmulatorInterpreter::SetAddressRegister, this)),
        Instruction(0xF033, std::bind(&EmulatorInterpreter::StoreBinaryCodedDecimal, this)),
        Instruction(0xF055, std::bind(&EmulatorInterpreter::DumpRegisters, this)),
        Instruction(0xF065, std::bind(&EmulatorInterpreter::LoadRegisters, this))
    };

    // Initialize SDL mixer system
    OutputLog("[Info] Initializing audio device for playback");
    if (Mix_OpenAudio(NULL, nullptr) < 0)
        throw std::runtime_error("Failed to initialize audio device for playback (SDL_Error: " + std::string(Mix_GetError()));

    m_beepSound = Mix_LoadWAV("assets/beep.wav");
    if (!m_beepSound)
        throw std::runtime_error("Failed to load \"assets/beep.wav\" (SDL_Error: " + std::string(Mix_GetError()));
}

EmulatorInterpreter::~EmulatorInterpreter()
{
    Mix_FreeChunk(m_beepSound);
    Mix_CloseAudio();
    Mix_Quit();

#ifndef INTERPRETER_IMPL_TEST
    std::ofstream file("key_bindings.json");
    file << m_keyBindings.dump(4);
#endif
}

void EmulatorInterpreter::ResetSystem()
{
    m_addressRegister = m_currentOpcode = m_delayTimer = m_soundTimer = 0;
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

void EmulatorInterpreter::DecodeOpcode()
{
    uint16_t opcode = m_currentOpcode;
    OutputLog("[Info] Executing opcode instruction: %X\n", opcode);

    // Only keep the parts of the opcode that are useful for identifying the instruction to execute
    // The 'data' part of the given opcode (NNN, X, Y, etc.) is removed
    if ((m_currentOpcode & 0xF000) == 0x0000)
        opcode &= 0xFF;
    else if ((m_currentOpcode & 0xF000) == 0x8000)
        opcode &= 0xF00F;
    else if (((m_currentOpcode & 0xF000) == 0xE000) || ((m_currentOpcode & 0xF000) == 0xF000))
        opcode &= 0xF0FF;
    else
        opcode &= 0xF000;

    // Find the instruction in the table
    auto instruction = std::lower_bound(m_instructionsTable.begin(), m_instructionsTable.end(), opcode,
        [](const Instruction& instruction, uint16_t opcode) { return instruction.opcode < opcode; });

    instruction->func(); // Execute the instruction
}

void EmulatorInterpreter::ClearDisplay()
{
    memset(m_displayBuffer.data(), 0, sizeof(m_displayBuffer));
    m_shouldRender = true;
    m_programCounter += 2;
}

void EmulatorInterpreter::DrawSprite()
{
    const uint8_t x = m_registers[(m_currentOpcode & 0xF00) >> 8];
    const uint8_t y = m_registers[(m_currentOpcode & 0xF0) >> 4];
    const uint8_t height = m_currentOpcode & 0xF;

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

void EmulatorInterpreter::SubrountineReturn()
{
    m_programCounter = m_stack[m_stackPointer] + 2;
    m_stackPointer--;
}

void EmulatorInterpreter::JumpTo()
{
    if ((m_currentOpcode & 0xF000) == 0x1000) // 1NNN: PC = NNN
    {
        m_programCounter = (m_currentOpcode & 0xFFF);
    }
    else if ((m_currentOpcode & 0xF000) == 0xB000) // BNNN: PC = NNN + V0
    {
        m_programCounter = (m_currentOpcode & 0xFFF) + m_registers[0x0];
    }
}

void EmulatorInterpreter::SubroutineCall()
{
    m_stack[++m_stackPointer] = m_programCounter;
    m_programCounter = (m_currentOpcode & 0xFFF);
}

void EmulatorInterpreter::SkipIfEqual()
{
    if ((m_currentOpcode & 0xF000) == 0x3000) // 3XNN: Vx == NN
    {
        if (m_registers[(m_currentOpcode & 0xF00) >> 8] == (m_currentOpcode & 0xFF))
            m_programCounter += 4;
        else
            m_programCounter += 2;
    }
    else if ((m_currentOpcode & 0xF000) == 0x5000) // 5XY0: Vx == Vy
    {
        if (m_registers[(m_currentOpcode & 0xF00) >> 8] == m_registers[(m_currentOpcode & 0xF0) >> 4])
            m_programCounter += 4;
        else
            m_programCounter += 2;
    }
}

void EmulatorInterpreter::SkipIfNotEqual()
{
    if ((m_currentOpcode & 0xF000) == 0x4000) // 4XNN: Vx != NN
    {
         if (m_registers[(m_currentOpcode & 0xF00) >> 8] != (m_currentOpcode & 0xFF))
            m_programCounter += 4;
        else
            m_programCounter += 2;
    }
    else if ((m_currentOpcode & 0xF000) == 0x9000) // 9XY0: Vx != Vy
    {
        if (m_registers[(m_currentOpcode & 0xF00) >> 8] != m_registers[(m_currentOpcode & 0xF0) >> 4])
            m_programCounter += 4;
        else
            m_programCounter += 2;
    }
}

void EmulatorInterpreter::SetValue()
{
    if ((m_currentOpcode & 0xF000) == 0x6000) // 6XNN: Set Vx = NN
    {
        m_registers[(m_currentOpcode & 0xF00) >> 8] = m_currentOpcode & 0xFF;
    }
    else if ((m_currentOpcode & 0xF000) == 0x8000) // 8XY0: Set Vx = Vy
    {
        m_registers[(m_currentOpcode & 0xF00) >> 8] = m_registers[(m_currentOpcode & 0xF0) >> 4];
    }

    m_programCounter += 2;
}

void EmulatorInterpreter::SetRandomValue()
{
    m_registers[(m_currentOpcode & 0xF00) >> 8] = (uint8_t)(rand() % 0xFF) & (m_currentOpcode & 0xFF);
    m_programCounter += 2;
}

void EmulatorInterpreter::AddValue()
{
    if ((m_currentOpcode & 0xF000) == 0x7000) // 7XNN: Vx += NN
    {
        m_registers[(m_currentOpcode & 0xF00) >> 8] += m_currentOpcode & 0xFF;
    }
    else if ((m_currentOpcode & 0xF000) == 0x8000) // 8XY4: Vx += Vy
    {
        if ((uint8_t)(m_registers[(m_currentOpcode & 0xF00) >> 8] + m_registers[(m_currentOpcode & 0xF0) >> 4]) < 
            m_registers[(m_currentOpcode & 0xF00) >> 8])
        {
            m_registers[0xF] = 1;
        }
        else
            m_registers[0xF] = 0;

        m_registers[(m_currentOpcode & 0xF00) >> 8] += m_registers[(m_currentOpcode & 0xF0) >> 4];
    }

    m_programCounter += 2;
}

void EmulatorInterpreter::SubtractValue()
{
    if ((m_currentOpcode & 0xF) == 0x5) // 8XY5: Vx -= NN
    {
        if (m_registers[(m_currentOpcode & 0xF0) >> 4] > m_registers[(m_currentOpcode & 0xF00) >> 8])
            m_registers[0xF] = 0;
        else
            m_registers[0xF] = 1;

        m_registers[(m_currentOpcode & 0xF00) >> 8] -= m_registers[(m_currentOpcode & 0xF0) >> 4];
    }
    else if ((m_currentOpcode & 0xF) == 0x7) // 8XY7: Vx = Vy - Vx
    {
        if (m_registers[(m_currentOpcode & 0xF00) >> 8] > m_registers[(m_currentOpcode & 0xF0) >> 4])
            m_registers[0xF] = 0;
        else
            m_registers[0xF] = 1;

        m_registers[(m_currentOpcode & 0xF00) >> 8] = 
            m_registers[(m_currentOpcode & 0xF0) >> 4] - m_registers[(m_currentOpcode & 0xF00) >> 8];
    }

    m_programCounter += 2;
}

void EmulatorInterpreter::BitwiseOR()
{
    m_registers[(m_currentOpcode & 0xF00) >> 8] |= m_registers[(m_currentOpcode & 0xF0) >> 4];
    m_programCounter += 2;
}

void EmulatorInterpreter::BitwiseAND()
{
    m_registers[(m_currentOpcode & 0xF00) >> 8] &= m_registers[(m_currentOpcode & 0xF0) >> 4];
    m_programCounter += 2;
}

void EmulatorInterpreter::BitwiseXOR()
{
    m_registers[(m_currentOpcode & 0xF00) >> 8] ^= m_registers[(m_currentOpcode & 0xF0) >> 4];
    m_programCounter += 2;
}

void EmulatorInterpreter::LeftShiftBits()
{
    m_registers[0xF] = m_registers[(m_currentOpcode & 0xF00) >> 8] >> 7;
    m_registers[(m_currentOpcode & 0xF00) >> 8] <<= 1;
    m_programCounter += 2;
}

void EmulatorInterpreter::RightShiftBits()
{
    m_registers[0xF] = m_registers[(m_currentOpcode & 0xF00) >> 8] & 0x1;
    m_registers[(m_currentOpcode & 0xF00) >> 8] >>= 1;
    m_programCounter += 2;
}

void EmulatorInterpreter::SetAddressRegister()
{
    if ((m_currentOpcode & 0xF000) == 0xA000) // ANNN: I = NNN
    {
        m_addressRegister = m_currentOpcode & 0xFFF;
    }
    else if ((m_currentOpcode & 0xF0FF) == 0xF01E) // FX1E: I += Vx
    {
        m_addressRegister += m_registers[(m_currentOpcode & 0xF00) >> 8];
    }
    else if ((m_currentOpcode & 0xF0FF) == 0xF029) // FX29: I = font glyph address
    {
        m_addressRegister = m_registers[(m_currentOpcode & 0xF00) >> 8] * 5;
    }

    m_programCounter += 2;
}

void EmulatorInterpreter::StoreBinaryCodedDecimal()
{
    m_memory[m_addressRegister] = m_registers[(m_currentOpcode & 0xF00) >> 8] / 100;
    m_memory[m_addressRegister + 1] = (m_registers[(m_currentOpcode & 0xF00) >> 8] / 10) % 10;
    m_memory[m_addressRegister + 2] = (m_registers[(m_currentOpcode & 0xF00) >> 8] % 100) % 10;
    m_programCounter += 2;
}

void EmulatorInterpreter::DumpRegisters()
{
    for (int i = 0; i <= (m_currentOpcode & 0xF00) >> 8; i++)
        m_memory[m_addressRegister + i] = m_registers[i];

    m_programCounter += 2;
}

void EmulatorInterpreter::LoadRegisters()
{
    for (int i = 0; i <= (m_currentOpcode & 0xF00) >> 8; i++)
        m_registers[i] = m_memory[m_addressRegister + i];

    m_programCounter += 2;
}

void EmulatorInterpreter::SkipIfKeyPressed()
{
    if (m_keys[m_registers[(m_currentOpcode & 0xF00) >> 8]])
        m_programCounter += 4;
    else
        m_programCounter += 2;
}

void EmulatorInterpreter::SkipIfKeyNotPressed()
{
    if (!m_keys[m_registers[(m_currentOpcode & 0xF00) >> 8]])
        m_programCounter += 4;
    else
        m_programCounter += 2;
}

void EmulatorInterpreter::WaitForKeyPress()
{
    bool wasKeyPressed = false;
    for (int i = 0; i < 0xF; i++)
    {
        if (m_keys[i])
        {
            m_registers[(m_currentOpcode & 0xF00) >> 8] = (uint8_t)i;
            wasKeyPressed = true;
        }
    }

    if (wasKeyPressed)
        m_programCounter += 2;
}

void EmulatorInterpreter::SetDelayTimer()
{
    m_delayTimer = m_registers[(m_currentOpcode & 0xF00) >> 8];
    m_programCounter += 2;
}

void EmulatorInterpreter::SetSoundTimer()
{
    m_soundTimer = m_registers[(m_currentOpcode & 0xF00) >> 8];
    m_programCounter += 2;
}

void EmulatorInterpreter::GetDelayTimer()
{
    m_registers[(m_currentOpcode & 0xF00) >> 8] = m_delayTimer;
    m_programCounter += 2;
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
    m_currentOpcode = (uint16_t)((m_memory[m_programCounter] << 8) | m_memory[m_programCounter + 1]);
    this->DecodeOpcode();

    // Update timers
    if (m_delayTimer > 0)
        m_delayTimer--;

    if (m_soundTimer > 0)
    {
        if (m_soundTimer == 1)
            Mix_PlayChannel(-1, m_beepSound, 0);

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