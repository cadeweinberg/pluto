// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PLUTO_TIIR_OPERAND_H
#define PLUTO_TIIR_OPERAND_H

#include <stdint.h>
#include <stdio.h>

typedef enum _pluto_OperandKind
{
    PLUTO_OPERAND_KIND_CONSTANT,
    PLUTO_OPERAND_KIND_REGISTER,
    PLUTO_OPERAND_KIND_IMMEDIATE,
    PLUTO_OPERAND_KIND_LABEL,
} pluto_OperandKind;

typedef struct _pluto_Operand
{
    pluto_OperandKind kind;
    uint32_t data;
} pluto_Operand;

pluto_Operand pluto_operand_constant(uint32_t constant);
pluto_Operand pluto_operand_register(uint32_t reg);
pluto_Operand pluto_operand_immediate(uint32_t imm);
pluto_Operand pluto_operand_label(uint32_t label);

struct _pluto_Context;
void pluto_operand_print(FILE *out, pluto_Operand operand, struct _pluto_Context *ctx);

#endif // !PLUTO_TIIR_OPERAND_H
