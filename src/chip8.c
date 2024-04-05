#include "chip8.h"
#include "font.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int quit = 0;

// Ops
void op_00E0(chip8 * const);
void op_00EE(chip8 * const);
void op_1nnn(chip8 * const, word);
void op_2nnn(chip8 * const, word);
void op_3xkk(chip8 * const, word);
void op_4xkk(chip8 * const, word);
void op_5xy0(chip8 * const, word);
void op_6xkk(chip8 * const, word);
void op_7xkk(chip8 * const, word);
void op_8xy0(chip8 * const, word);
void op_8xy1(chip8 * const, word);
void op_8xy2(chip8 * const, word);
void op_8xy3(chip8 * const, word);
void op_8xy4(chip8 * const, word);
void op_8xy5(chip8 * const, word);
void op_8xy6(chip8 * const, word);
void op_8xy7(chip8 * const, word);
void op_8xyE(chip8 * const, word);
void op_9xy0(chip8 * const, word);
void op_Annn(chip8 * const, word);
void op_Bnnn(chip8 * const, word);
void op_Cxkk(chip8 * const, word);
void op_Dxyn(chip8 * const, word);
void op_Ex9E(chip8 * const, word);
void op_ExA1(chip8 * const, word);
void op_Fx07(chip8 * const, word);
void op_Fx0A(chip8 * const, word);
void op_Fx15(chip8 * const, word);
void op_Fx18(chip8 * const, word);
void op_Fx1E(chip8 * const, word);
void op_Fx29(chip8 * const, word);
void op_Fx33(chip8 * const, word);
void op_Fx55(chip8 * const, word);
void op_Fx65(chip8 * const, word);

// Helper functions
byte rand_byte();

void chip8_init(chip8 * const emulator)
{
    srand(time(NULL));

    memset(emulator->memory, 0, MEMORY_SIZE);
    memcpy(emulator->memory+FONT_START_ADDR, FONT, FONT_NUM_SPRITES*FONT_SPRITE_SIZE);

    emulator->pc = ROM_START_ADDR;
    emulator->sp = 0;
}

void chip8_load_rom(chip8 * const emulator, const char * const rom_file_name)
{
    FILE * fp = fopen(rom_file_name, "rb");

    if (fp == NULL)
    {  
        printf("[CHIP8] %s: Unable to load rom: %s\n", __func__, rom_file_name);
        exit(-1);
    }

    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    rewind(fp);
    fread(&emulator->memory[ROM_START_ADDR], sizeof(emulator->memory[0]), file_size, fp);

    fclose(fp);
}

void chip8_memory_dump(const chip8 * const emulator)
{
    printf("[");

    for (int i = 0; i < MEMORY_SIZE-1; i++)
    {
        printf("0x%X, ", emulator->memory[i]);
    }

    printf("0x%X]\n", emulator->memory[MEMORY_SIZE-1]);
}

// CLS - Clear video screen
void op_00E0(chip8 * const emulator)
{
    memset(emulator->video, 0, sizeof(emulator->video));
}

// RET - Return from subroutine
void op_00EE(chip8 * const emulator)
{
    emulator->sp--;
    emulator->pc = emulator->stack[emulator->sp];
}

// JP addr - Jump to specified address
void op_1nnn(chip8 * const emulator, word opcode)
{
    word address = opcode & 0x0FFFu;

    emulator->pc = address;
}

// CALL addr - Call subroutine at specified address
void op_2nnn(chip8 * const emulator , word opcode)
{
    word address = opcode & 0x0FFFu;

    emulator->stack[emulator->sp] = emulator->pc;
    emulator->sp++;
    emulator->pc = address;
}

// SE Vx, byte - Skip next instruction if Vx = kk
void op_3xkk(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;
    word value = opcode & 0x00FFu;

    if (emulator->variable_regs[reg] == value)
        emulator->pc += 2;
}

// SNE Vx, byte - Skip next instruction if Vx != kk
void op_4xkk(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;
    word value = opcode & 0x00FFu;

    if (emulator->variable_regs[reg] != value)
        emulator->pc += 2;
}

// SE Vx, Vy - Skip next instruction if Vx = Vy
void op_5xy0(chip8 * const emulator , word opcode)
{
    byte regx = (opcode & 0x0F00u) >> 8;
    byte regy = (opcode & 0x00F0u) >> 4;

    if (emulator->variable_regs[regx] == emulator->variable_regs[regy])
        emulator->pc += 2;
}

// LD Vx, byte - Load value kk into Vx
void op_6xkk(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;
    word value = opcode & 0x00FFu;

    emulator->variable_regs[reg] = value;
}

// ADD Vx, byte - Add value kk to the Vx
void op_7xkk(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;
    word value = opcode & 0x00FFu;

    emulator->variable_regs[reg] += value;
}

// LD Vx, Vy - Load value in Vy into Vx
void op_8xy0(chip8 * const emulator , word opcode)
{
    byte regx = (opcode & 0x0F00u) >> 8;
    byte regy = (opcode & 0x00F0u) >> 4;

    emulator->variable_regs[regx] = emulator->variable_regs[regy];
}

// OR Vx, Vy - Set Vx equal to the result of or-ing Vx and Vy
void op_8xy1(chip8 * const emulator , word opcode)
{
    byte regx = (opcode & 0x0F00u) >> 8;
    byte regy = (opcode & 0x00F0u) >> 4;

    emulator->variable_regs[regx] |= emulator->variable_regs[regy];
}

// AND Vx, Vy - Set Vx equal to the result of and-ing Vx and Vy
void op_8xy2(chip8 * const emulator , word opcode)
{
    byte regx = (opcode & 0x0F00u) >> 8;
    byte regy = (opcode & 0x00F0u) >> 4;

    emulator->variable_regs[regx] &= emulator->variable_regs[regy];
}

// XOR Vx, Vy - Set Vx equal to the result of xor-ing Vx and Vy
void op_8xy3(chip8 * const emulator , word opcode)
{
    byte regx = (opcode & 0x0F00u) >> 8;
    byte regy = (opcode & 0x00F0u) >> 4;

    emulator->variable_regs[regx] ^= emulator->variable_regs[regy];
}

// ADD Vx, Vy - Set Vx equal to the result of adding Vx and Vy
void op_8xy4(chip8 * const emulator , word opcode)
{
    byte regx = (opcode & 0x0F00u) >> 8;
    byte regy = (opcode & 0x00F0u) >> 4;

    emulator->variable_regs[regx] += emulator->variable_regs[regy];
}

// SUB Vx, Vy - Set Vx equal to the result of subtracting Vx and Vy
void op_8xy5(chip8 * const emulator , word opcode)
{
    byte regx = (opcode & 0x0F00u) >> 8;
    byte regy = (opcode & 0x00F0u) >> 4;

    emulator->variable_regs[regx] -= emulator->variable_regs[regy];
}

// SHR Vx {, Vy} - Right shift the value in Vx by 1, saving the LSB in VF
void op_8xy6(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;

    emulator->variable_regs[FLAG_REGISTER] = emulator->variable_regs[reg] & 0x1u;

    emulator->variable_regs[reg] >>= 1;
}

// SUBN Vx, Vy -  Set Vx equal Vy - Vx, set VF to not borrow
void op_8xy7(chip8 * const emulator , word opcode)
{
    byte regx = (opcode & 0x0F00u) >> 8;
    byte regy = (opcode & 0x00F0u) >> 4;

    if (emulator->variable_regs[regy] > emulator->variable_regs[regx])
        emulator->variable_regs[FLAG_REGISTER] = 1;
    else
        emulator->variable_regs[FLAG_REGISTER] = 0;

    emulator->variable_regs[regx] = emulator->variable_regs[regy] - emulator->variable_regs[regx];
}

// SHL Vx {, Vy} - Left shift the value in Vx by 1, saving the MSB in VF
void op_8xyE(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;

    emulator->variable_regs[FLAG_REGISTER] = emulator->variable_regs[reg] & 0x80 >> 7;

    emulator->variable_regs[reg] <<= 1;
}

// SNE Vx, Vy - Skip the next instruction if Vx != Vy
void op_9xy0(chip8 * const emulator , word opcode)
{
    byte regx = (opcode & 0x0F00u) >> 8;
    byte regy = (opcode & 0x00F0u) >> 4;

    if (emulator->variable_regs[regx] != emulator->variable_regs[regy])
        emulator->pc += 2;
}

// LD I, addr - Set index register to addr
void op_Annn(chip8 * const emulator , word opcode)
{
    word address = opcode & 0x0FFFu;

    emulator->index = address;
}

// JP V0, addr - Jump to location addr + V0 
void op_Bnnn(chip8 * const emulator , word opcode)
{
    word address = opcode & 0x0FFFu;

    emulator->pc = address + emulator->variable_regs[0x0u];
}

// RND Vx, byte - Set Vx to a random byte anded with kk
void op_Cxkk(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;
    word value = opcode * 0x00FFu;
    byte random_byte = rand_byte();

    emulator->variable_regs[reg] = random_byte & value;
}

// DRW Vx, Vy, nibble - Display n byte sprite starting at index at video location (Vx, Vy)
// Set VF if there any erased pixels 
void op_Dxyn(chip8 * const emulator , word opcode)
{
    byte regx = (opcode & 0x0F00u) >> 8;
    byte regy = (opcode & 0x00F0u) >> 4;
    byte num_bytes = opcode & 0x000Fu;

    emulator->variable_regs[FLAG_REGISTER] = 0;

    for (unsigned int row = 0; row < num_bytes; row++)
    {
        byte sprite = emulator->memory[emulator->index + row];

        byte x_pos = (emulator->variable_regs[regy] + row) % SCREEN_HEIGHT;

        for (unsigned int col = 0; col < 8; col++)
        {
            byte pixel = sprite & (0x80 >> 7);
            byte y_pos = (emulator->variable_regs[regx] + col) % SCREEN_WIDTH;
            word video_idx = x_pos * SCREEN_WIDTH + y_pos;

            if (pixel)
            {
                if (emulator->video[video_idx])
                {
                    emulator->variable_regs[FLAG_REGISTER] = 0x1u;
                }

                emulator->video[video_idx] ^= pixel;
            }
        }
    }
}

// SKP Vx - Skip next instruction if key at Vx is pressed
void op_Ex9E(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;
    byte key = emulator->variable_regs[reg];

    if (keys[key])
        emulator->pc += 2;
}

// SKNP Vx - Skip next instruction if key at Vx is not pressed
void op_ExA1(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;
    byte key = emulator->variable_regs[reg];

    if (!keys[key])
        emulator->pc += 2;
}

// LD Vx, DT - Load value of delay timer into Vx
void op_Fx07(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;

    emulator->variable_regs[reg] = emulator->delay_timer;
}

// LD Vx, K - Wait until a key is pressed and store the key in Vx;
void op_Fx0A(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;

    bool pressed = false;

    while (!pressed)
    {
        for (unsigned int key = 0; key < NUM_KEYS; key++)
        {
            if (keys[key])
            {
                emulator->variable_regs[reg] = key;
                pressed = true; 
                break;
            }
        }
    }
}


// LD DT, Vx - Load the value in Vx in the delay timer
void op_Fx15(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;

    emulator->delay_timer = emulator->variable_regs[reg];
}

// LD ST, Vx - Load the value in Vx in the sound timer
void op_Fx18(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;

    emulator->sound_timer = emulator->variable_regs[reg];
}

// ADD I, Vx - Sets I to the result of adding I and Vx
void op_Fx1E(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;

    emulator->index += emulator->variable_regs[reg];
}

// LD F, Vx - Set I to the location of the sprite of value in Vx
void op_Fx29(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;
    byte value = emulator->variable_regs[reg];
    
    word address = FONT_START_ADDR + (value * FONT_SPRITE_SIZE);
    emulator->index = address;
}

// LD B, Vx - Store BCD representation of Vx in locations I, I+1, I+2
void op_Fx33(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;
    byte value = emulator->variable_regs[reg];

    // Ones
    emulator->memory[emulator->index + 2] = value % 10;
    value /= 10;
    // Tens
    emulator->memory[emulator->index + 1] = value % 10;
    value /= 10;
    // Hundreds
    emulator->memory[emulator->index] = value % 10;
}

// LD [I], Vx - Load V0 through Vx into memory starting at I
void op_Fx55(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;

    memcpy(&emulator->memory[emulator->index], &emulator->variable_regs[0x0u], reg+1);
}

// LD Vx, [I] - Load from memory starting at I into V0 through Vx
void op_Fx65(chip8 * const emulator , word opcode)
{
    byte reg = (opcode & 0x0F00u) >> 8;

    memcpy(&emulator->variable_regs[0x0u], &emulator->memory[emulator->index], reg+1);
}

byte rand_byte()
{
    return rand() % (UINT8_MAX + 1);
}