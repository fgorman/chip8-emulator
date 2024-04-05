#include "chip8.h"
#include "font.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int quit = 0;

/////////////////////////////////////////////////////
/// Ops Definitions
/////////////////////////////////////////////////////

typedef void op_func(chip8 * const, word);

void op_noop(chip8 * const, word);
void op_00E0(chip8 * const, word);
void op_00EE(chip8 * const, word);
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

op_func * op_table[0x10];
op_func * op_table0[0xF];
op_func * op_table8[0xF];
op_func * op_tableE[0xF];
op_func * op_tableF[0x66];

void table0_lookup(chip8 * const em, word op)
{
    op_table0[(op & 0x000Fu)](em, op);
}
void table8_lookup(chip8 * const em, word op)
{
    op_table8[(op & 0x000Fu)](em, op);
}
void tableE_lookup(chip8 * const em, word op)
{
    op_tableE[(op & 0x000Fu)](em, op);
}
void tableF_lookup(chip8 * const em, word op)
{
    op_tableF[(op & 0x00FFu)](em, op);
}

/////////////////////////////////////////////////////
/// Helper Method Definitions
/////////////////////////////////////////////////////

byte rand_byte();

/////////////////////////////////////////////////////
/// CHIP8 Methods Implementations
/////////////////////////////////////////////////////

void chip8_init(chip8 * const emulator)
{
    srand(time(NULL));

    memset(emulator->memory, 0, MEMORY_SIZE);
    memcpy(emulator->memory+FONT_START_ADDR, FONT, FONT_NUM_SPRITES*FONT_SPRITE_SIZE);

    emulator->pc = ROM_START_ADDR;
    emulator->sp = 0;

    // Op tables setup
    op_table[0x0u] = &table0_lookup;
    op_table[0x1u] = &op_1nnn;
    op_table[0x2u] = &op_2nnn;
    op_table[0x3u] = &op_3xkk;
    op_table[0x4u] = &op_4xkk;
    op_table[0x5u] = &op_5xy0;
    op_table[0x6u] = &op_6xkk;
    op_table[0x7u] = &op_7xkk;
    op_table[0x8u] = &table8_lookup;
    op_table[0x9u] = &op_9xy0;
    op_table[0xAu] = &op_Annn;
    op_table[0xBu] = &op_Bnnn;
    op_table[0xCu] = &op_Cxkk;
    op_table[0xDu] = &op_Dxyn;
    op_table[0xEu] = &tableE_lookup;
    op_table[0xFu] = &tableF_lookup;

    for (byte i = 0; i < 0xFu; i++)
    {
        op_table0[i] = &op_noop;
        op_table8[i] = &op_noop;
        op_tableE[i] = &op_noop;
    }

    for (byte i = 0; i < 0x66u; i++)
    {
        op_tableF[i] = &op_noop;
    }

    op_table0[0x0u] = &op_00E0;
    op_table0[0xEu] = &op_00EE;

    op_table8[0x0u] - &op_8xy0;
    op_table8[0x1u] - &op_8xy1;
    op_table8[0x2u] - &op_8xy2;
    op_table8[0x3u] - &op_8xy3;
    op_table8[0x4u] - &op_8xy4;
    op_table8[0x5u] - &op_8xy5;
    op_table8[0x6u] - &op_8xy6;
    op_table8[0x7u] - &op_8xy7;
    op_table8[0xEu] - &op_8xyE;

    op_tableE[0x1u] = &op_ExA1;
    op_tableE[0xEu] = &op_Ex9E;

    op_tableF[0x07u] = &op_Fx07;
    op_tableF[0x0Au] = &op_Fx0A;
    op_tableF[0x15u] = &op_Fx15;
    op_tableF[0x18u] = &op_Fx18;
    op_tableF[0x1Eu] = &op_Fx1E;
    op_tableF[0x29u] = &op_Fx29;
    op_tableF[0x33u] = &op_Fx33;
    op_tableF[0x55u] = &op_Fx55;
    op_tableF[0x65u] = &op_Fx65;
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

void chip8_cycle(chip8 * const emulator)
{
    // Fetch
    word instruction = emulator->memory[emulator->pc];

    emulator->pc += 2;

    // Decode
    op_func * op = op_table[(instruction & 0xF000u) >> 12];

    // Execute
    op(emulator, instruction);

    if (emulator->delay_timer != 0)
        emulator->delay_timer--;

    if (emulator->sound_timer != 0)
        emulator->sound_timer--;
}

/////////////////////////////////////////////////////
/// Ops Implementations
/////////////////////////////////////////////////////

// NOOP - Do nothing for undefined instruction
void op_noop(chip8 * const _, word __)
{
    return;
}

// CLS - Clear video screen
void op_00E0(chip8 * const emulator, word _)
{
    memset(emulator->video, 0, sizeof(emulator->video));
}

// RET - Return from subroutine
void op_00EE(chip8 * const emulator, word _)
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

/////////////////////////////////////////////////////
/// Helper Method Implementations
/////////////////////////////////////////////////////

byte rand_byte()
{
    return rand() % (UINT8_MAX + 1);
}