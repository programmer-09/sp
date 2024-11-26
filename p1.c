#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Constants
#define MAX_MOT_SIZE 20
#define MAX_POT_SIZE 20
#define MAX_RT_SIZE 20
#define MAX_DST_SIZE 20
#define MAX_SYMBOL_SIZE 20
#define MAX_LINE_SIZE 100
#define MAX_LIT_SIZE 20

// Structures
typedef struct {
    int opcode;
    char operand[10];
} MOT, POT, RT, DST;

typedef struct {
    int sno;
    char symbol[10];
    int address;
} SYM;

typedef struct {
    int sno;
    char literal[20];
    int address;
} LIT;

// Tables
MOT motTable[MAX_MOT_SIZE] = {
    {1, "MOVER"}, {2, "MOVEM"}, {3, "ADD"}, {4, "SUB"}, {5, "MUL"},
    {6, "DIV"}, {7, "BC"}, {8, "COMP"}, {9, "READ"}, {10, "PRINT"}
};

POT potTable[MAX_POT_SIZE] = {
    {1, "START"}, {2, "END"}, {3, "ORIGIN"}, {4, "EQU"}, {5, "LTORG"}
};

RT regTable[MAX_RT_SIZE] = {
    {1, "AREG"}, {2, "BREG"}, {3, "CREG"}, {4, "DREG"}
};

DST dstTable[MAX_DST_SIZE] = {
    {1, "DS"}, {2, "DC"}
};

// Symbol and Literal Tables
SYM symbolTable[MAX_SYMBOL_SIZE];
LIT literalTable[MAX_LIT_SIZE];
int sycount = 0;
int litCount = 0;

// Check if a word is a literal
int isLiteral(char *word) {
    return word[0] == '=' && word[1] == '\'';
}

// Check if a word is numeric
int isNumeric(char *word) {
    for (int i = 0; word[i] != '\0'; i++) {
        if (!isdigit(word[i])) {
            return 0;
        }
    }
    return 1;
}

// Add a symbol to the symbol table
void addToSymbolTable(char *symbol, int address) {
    for (int i = 0; i < sycount; i++) {
        if (strcmp(symbolTable[i].symbol, symbol) == 0) {
            return;
        }
    }
    symbolTable[sycount].sno = sycount + 1;
    strcpy(symbolTable[sycount].symbol, symbol);
    symbolTable[sycount].address = address;
    sycount++;
}

// Add a literal to the literal table
void addToLiteral(char *literal, int address) {
    for (int i = 0; i < litCount; i++) {
        if (strcmp(literalTable[i].literal, literal) == 0) {
            return;
        }
    }
    literalTable[litCount].sno = litCount + 1;
    strcpy(literalTable[litCount].literal, literal);
    literalTable[litCount].address = address;
    litCount++;
}

// Main function
int main() {
    FILE *file = fopen("input.txt", "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char word[10];
    int lc = 100;
    int found = 0;

    printf("%d\n", lc);

    while (fscanf(file, "%s", word) != EOF) {
        found = 0;

        // Check MOT
        for (int i = 0; i < MAX_MOT_SIZE; i++) {
            if (strcmp(word, motTable[i].operand) == 0) {
                printf("\n%d ", lc);
                printf("(IS, %02d)", motTable[i].opcode);
                found = 1;
                lc++;
                break;
            }
        }

        // Check POT
        if (!found) {
            for (int i = 0; i < MAX_POT_SIZE; i++) {
                if (strcmp(word, potTable[i].operand) == 0) {
                    printf("\n%d ", lc);
                    printf("(AD, %02d)", potTable[i].opcode);
                    found = 1;
                    lc++;
                    break;
                }
            }
        }

        // Check Registers
        if (!found) {
            for (int i = 0; i < MAX_RT_SIZE; i++) {
                if (strcmp(word, regTable[i].operand) == 0) {                    
                    printf(" %02d", regTable[i].opcode);
                    found = 1;
                    break;
                }
            }
        }

        // Check DST
        if (!found) {
            for (int i = 0; i < MAX_DST_SIZE; i++) {
                if (strcmp(word, dstTable[i].operand) == 0) {                    
                    printf(" %02d", dstTable[i].opcode);
                    found = 1;
                    lc++;
                    break;
                }
            }
        }

        // Check Literals
        if (!found && isLiteral(word)) {
            addToLiteral(word, lc);            
            printf(" (L, %d)", lc);
            found = 1;
        }

        // Check Symbols
        if (!found) {
            if (!isNumeric(word)) {                
                printf("\n%d ", lc);
                printf(" (S, %02d)", sycount + 1);
                addToSymbolTable(word, lc);
            } else if (isNumeric(word)) {                
                printf(" (C, %s)", word);
            }
        }
    }

    fclose(file);

    // Print Symbol Table
    printf("\n\nSNO\tSYMBOL\tADDRESS\n");
    for (int i = 0; i < sycount; i++) {
        printf("%d\t%s\t%d\n", symbolTable[i].sno, symbolTable[i].symbol, symbolTable[i].address);
    }

    // Print Literal Table
    printf("\n\nSNO\tLITERAL\tADDRESS\n");
    for (int i = 0; i < litCount; i++) {
        printf("%d\t%s\t%d\n", literalTable[i].sno, literalTable[i].literal, literalTable[i].address);
    }

    return 0;
}
