// SPDX-License-Identifier: GPL-3.0-or-later

#include "tiir/operand.h"
#include "context/context.h"

pluto_Operand pluto_operand_literal(uint32_t literal) {
    return (pluto_Operand){ .kind = PLUTO_OPERAND_KIND_LITERAL, .data = literal };
}

pluto_Operand pluto_operand_register(uint32_t reg) {
    return (pluto_Operand){ .kind = PLUTO_OPERAND_KIND_REGISTER, .data = reg };
}

pluto_Operand pluto_operand_immediate(uint32_t imm) {
    return (pluto_Operand){ .kind = PLUTO_OPERAND_KIND_IMMEDIATE, .data = imm };
}

pluto_Operand pluto_operand_label(uint32_t label) {
    return (pluto_Operand){ .kind = PLUTO_OPERAND_KIND_LABEL, .data = label };
}

void pluto_operand_print(FILE *out, pluto_Operand operand, struct _pluto_Context *ctx) {
    switch (operand.kind) {
        case PLUTO_OPERAND_KIND_LITERAL: {
            pluto_Literal value = pluto_context_get_literal(ctx, operand.data);
            pluto_literal_print(out, value, ctx);
            break;
        }
        case PLUTO_OPERAND_KIND_REGISTER:
            fprintf(out, "%%%u", operand.data);
            break;
        case PLUTO_OPERAND_KIND_IMMEDIATE:
            fprintf(out, "%u", operand.data);
            break;
        case PLUTO_OPERAND_KIND_LABEL: {
            pluto_StringView label = pluto_context_get_string(ctx, operand.data);
            fprintf(out, "%.*s", (int)label.length, label.data);
            break;
        }
    }
}