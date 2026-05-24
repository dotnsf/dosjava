#include "symtable.h"
#include <stdio.h>

int main(void) {
    printf("sizeof(Symbol) = %u\n", (unsigned)sizeof(Symbol));
    printf("sizeof(SymbolKind) = %u\n", (unsigned)sizeof(SymbolKind));
    printf("sizeof(uint16_t) = %u\n", (unsigned)sizeof(uint16_t));
    printf("sizeof(TypeInfo) = %u\n", (unsigned)sizeof(TypeInfo));
    printf("sizeof(Symbol.data) = %u\n", (unsigned)sizeof(((Symbol*)0)->data));
    printf("\nSymbol structure layout:\n");
    printf("  kind offset: %u\n", (unsigned)((char*)&((Symbol*)0)->kind - (char*)0));
    printf("  name_offset offset: %u\n", (unsigned)((char*)&((Symbol*)0)->name_offset - (char*)0));
    printf("  type offset: %u\n", (unsigned)((char*)&((Symbol*)0)->type - (char*)0));
    printf("  scope_level offset: %u\n", (unsigned)((char*)&((Symbol*)0)->scope_level - (char*)0));
    printf("  data offset: %u\n", (unsigned)((char*)&((Symbol*)0)->data - (char*)0));
    return 0;
}

// Made with Bob
