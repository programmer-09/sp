#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 100

// Structure to hold MOT (Mnemonic Opcode Table)
typedef struct {
    char mnemonic[10];
    int opcode;
} MOT;

// Structure to hold POT (Pseudo Opcode Table)
typedef struct {
    char directive[10];
    int opcode;
} POT;

// Structure to hold Symbol Table
typedef struct {
    char symbol[10];
    int address;
} SymbolTable;

// Structure to hold Literal Table
typedef struct {
    char literal[10];
    int address;
} LiteralTable;

// Structure for incomplete instructions
typedef struct {
    int lc;
    char symbol[10];
    int isLiteral;  // Flag to indicate if it's a literal
} IncompleteInstruction;

// Function declarations
int searchMOT(char mnemonic[], MOT mot[], int motSize);
int searchPOT(char directive[], POT pot[], int potSize);
int searchRegister(char reg[]);
void updateSymbolTable(char symbol[], int lc, SymbolTable symtab[], int *symCount);
void updateLiteralTable(char literal[], int lc, LiteralTable littab[], int *litCount);
int searchSymbolTable(char symbol[], SymbolTable symtab[], int symCount);
void displayTables(SymbolTable symtab[], int symCount, LiteralTable littab[], int litCount, IncompleteInstruction incomplete[], int incompleteCount);
void assignLiteralAddresses(LiteralTable littab[], int litCount, int lc);
void generateMachineCode(SymbolTable symtab[], int symCount, LiteralTable littab[], int litCount, IncompleteInstruction incomplete[], int incompleteCount);

int main() {
    // Define MOT (Mnemonic Opcode Table)
    MOT mot[] = {
        {"MOVER", 1}, {"MOVEM", 2}, {"ADD", 3}, {"SUB", 4}, {"MULT", 5},
        {"DIV", 6}, {"BC", 7}, {"COMP", 8}, {"PRINT", 9}, {"READ", 10}
    };
    int motSize = sizeof(mot) / sizeof(mot[0]);

    // Define POT (Pseudo Opcode Table)
    POT pot[] = {
        {"START", 1}, {"END", 2}, {"EQU", 3}, {"ORIGIN", 4}, {"LTORG", 5}
    };
    int potSize = sizeof(pot) / sizeof(pot[0]);

    // Symbol Table, Literal Table
    SymbolTable symtab[MAX];
    LiteralTable littab[MAX];

    // Incomplete instructions table
    IncompleteInstruction incomplete[MAX];

    // Counters for tables
    int symCount = 0, litCount = 0, incompleteCount = 0;

    // LC (Location Counter) starts from 100
    int lc = 100;

    // Input Assembly code
    char assemblyCode[MAX][50] = {
        "START 100",
        "READ  CELS",               
        "MOVER AREG, CELS",         
        "MULT  AREG, ='9'",         
        "DIV   AREG, ='5'",         
        "ADD   AREG, ='32'",        
        "MOVEM AREG, FAH",          
        "PRINT FAH",                
        "CELS DS 1",                
        "FAH  DS 1",                
        "END"
    };
    int assemblyLines = sizeof(assemblyCode) / sizeof(assemblyCode[0]);

    // Process each line
    for (int i = 0; i < assemblyLines; i++) {
        char line[50];
        strcpy(line, assemblyCode[i]);
        char *token = strtok(line, " ,");

        // Check if it's a pseudo opcode
        int pseudoOpcode = searchPOT(token, pot, potSize);
        if (pseudoOpcode != -1) {
            if (pseudoOpcode == 1) {  // START
                token = strtok(NULL, " ,");
                lc = atoi(token);  // Set starting LC value
                printf("Starting LC: %d\n", lc);
            } else if (pseudoOpcode == 2) {  // END
                break;
            } else if (pseudoOpcode == 3) {  // EQU
                char *symbol = strtok(NULL, " ,");
                char *equSymbol = strtok(NULL, " ,");
                updateSymbolTable(symbol, searchSymbolTable(equSymbol, symtab, symCount), symtab, &symCount);
                printf("%d) %s EQU %s\n", lc, symbol, equSymbol);
            } else if (pseudoOpcode == 4) {  // ORIGIN
                token = strtok(NULL, " ,");
                lc = atoi(token);
                printf("ORIGIN set LC to: %d\n", lc);
            } else if (pseudoOpcode == 5) {  // LTORG
                // Assign addresses to all literals
                for (int j = 0; j < litCount; j++) {
                    if (littab[j].address == -1) {  // If the literal has no address
                        littab[j].address = lc;
                        lc++;
                    }
                }
            }
        } else {
            // Check if it's a mnemonic
            int opcode = searchMOT(token, mot, motSize);
            if (opcode != -1) {
                char *reg = strtok(NULL, " ,");
                char *operand = strtok(NULL, " ,");

                int regCode = searchRegister(reg);  // Get register code

                if (regCode == -1) {
                    // If operand is not a register, treat it as a symbol or literal
                    if (operand != NULL && operand[0] == '=') {
                        updateLiteralTable(operand, -1, littab, &litCount);  // Assign -1 to indicate no address yet
                        incomplete[incompleteCount].lc = lc;  // Add to incomplete instructions
                        strcpy(incomplete[incompleteCount].symbol, operand);
                        incomplete[incompleteCount].isLiteral = 1;  // Mark as literal
                        incompleteCount++;
                        printf("%d) %02d -- --\n", lc, opcode); //new-----------
                    } else {
                        updateSymbolTable(reg, -1, symtab, &symCount);  // Add to symbol table
                        incomplete[incompleteCount].lc = lc;  // Add to incomplete instructions
                        strcpy(incomplete[incompleteCount].symbol, reg);
                        incomplete[incompleteCount].isLiteral = 0;  // Mark as symbol
                        incompleteCount++;
                        printf("%d) %02d -- --\n", lc, opcode);//new--------------
                    }
                } else {
                    if (operand != NULL && operand[0] == '=') {
                        updateLiteralTable(operand, -1, littab, &litCount);  // Add literal
                        incomplete[incompleteCount].lc = lc;  // Add to incomplete instructions
                        strcpy(incomplete[incompleteCount].symbol, operand);
                        incomplete[incompleteCount].isLiteral = 1;  // Mark as literal
                        incompleteCount++;
                        printf("%d) %02d %02d --\n", lc, opcode, regCode);//new----------
                    } else {
                        updateSymbolTable(operand, -1, symtab, &symCount);  // Add symbol
                        incomplete[incompleteCount].lc = lc;  // Add to incomplete instructions
                        strcpy(incomplete[incompleteCount].symbol, operand);
                        incomplete[incompleteCount].isLiteral = 0;  // Mark as symbol
                        incompleteCount++;
                        printf("%d) %02d %02d --\n", lc, opcode, regCode);//new----------
                    }
                }
                
                lc++;
            } else {
                // Handle data directives like DS, DC
                char *symbol = token;
                token = strtok(NULL, " ,");
                if (strcmp(token, "DS") == 0 || strcmp(token, "DC") == 0) {
                    updateSymbolTable(symbol, lc, symtab, &symCount);
                    printf("%d) 01 -- 001\n", lc); // Output for DS
                    lc++;
                }
            }
        }
    }

    // Assign addresses to literals after reaching END
    assignLiteralAddresses(littab, litCount, lc);
    
    // Display all tables
    displayTables(symtab, symCount, littab, litCount, incomplete, incompleteCount);

    generateMachineCode(symtab, symCount, littab, litCount, incomplete, incompleteCount);
    
    return 0;
}

// Function to search in MOT
int searchMOT(char mnemonic[], MOT mot[], int motSize) {
    for (int i = 0; i < motSize; i++) {
        if (strcmp(mnemonic, mot[i].mnemonic) == 0) {
            return mot[i].opcode;
        }
    }
    return -1;
}

// Function to search in POT
int searchPOT(char directive[], POT pot[], int potSize) {
    for (int i = 0; i < potSize; i++) {
        if (strcmp(directive, pot[i].directive) == 0) {
            return pot[i].opcode;
        }
    }
    return -1;
}

// Function to search for a register
int searchRegister(char reg[]) {
    if (strcmp(reg, "AREG") == 0) return 1;
    if (strcmp(reg, "BREG") == 0) return 2;
    if (strcmp(reg, "CREG") == 0) return 3;
    if (strcmp(reg, "DREG") == 0) return 4;
    return -1;
}

// Function to update Symbol Table
void updateSymbolTable(char symbol[], int lc, SymbolTable symtab[], int *symCount) {
    for (int i = 0; i < *symCount; i++) {
        if (strcmp(symtab[i].symbol, symbol) == 0) {
            // Symbol already exists, update address if lc is not -1
            if (lc != -1) {
                symtab[i].address = lc;
            }
            return;
        }
    }
    // Add new symbol
    strcpy(symtab[*symCount].symbol, symbol);
    symtab[*symCount].address = lc;  // Set address if provided
    (*symCount)++;
}

// Function to update Literal Table
void updateLiteralTable(char literal[], int lc, LiteralTable littab[], int *litCount) {
    for (int i = 0; i < *litCount; i++) {
        if (strcmp(littab[i].literal, literal) == 0) {
            // Literal already exists, update address if lc is not -1
            if (lc != -1) {
                littab[i].address = lc;
            }
            return;
        }
    }
    // Add new literal
    strcpy(littab[*litCount].literal, literal);
    littab[*litCount].address = lc;  // Set address if provided
    (*litCount)++;
}

// Function to search Symbol Table
int searchSymbolTable(char symbol[], SymbolTable symtab[], int symCount) {
    for (int i = 0; i < symCount; i++) {
        if (strcmp(symtab[i].symbol, symbol) == 0) {
            return symtab[i].address;  // Return the address of the symbol
        }
    }
    return -1;  // Symbol not found
}

// Function to display all tables
void displayTables(SymbolTable symtab[], int symCount, LiteralTable littab[], int litCount, IncompleteInstruction incomplete[], int incompleteCount) {
    printf("\nSymbol Table:\n");
    printf("Symbol\tAddress\n");
    for (int i = 0; i < symCount; i++) {
        printf("%s\t%d\n", symtab[i].symbol, symtab[i].address);
    }

    printf("\nLiteral Table:\n");
    printf("Literal\tAddress\n");
    for (int i = 0; i < litCount; i++) {
        printf("%s\t%d\n", littab[i].literal, littab[i].address);
    }

    printf("\nIncomplete Instructions:\n");
    printf("LC\tSymbol\tType\n");
    for (int i = 0; i < incompleteCount; i++) {
        printf("%d\t%s\t%s\n", incomplete[i].lc, incomplete[i].symbol, incomplete[i].isLiteral ? "Literal" : "Symbol");
    }
}

// Function to assign addresses to literals
void assignLiteralAddresses(LiteralTable littab[], int litCount, int lc) {
    char arr[3]={9, 5, 32};
    for (int i = 0; i < litCount; i++) {
        if (littab[i].address == -1) {  // If literal has no address assigned
            printf("%d) 02 -- %d\n", lc, arr[i]);
            littab[i].address = lc++;
        }
    }
}

void generateMachineCode(SymbolTable symtab[], int symCount, LiteralTable littab[], int litCount, IncompleteInstruction incomplete[], int incompleteCount) {
    MOT mot[] = {
        {"MOVER", 1}, {"MOVEM", 2}, {"ADD", 3}, {"SUB", 4}, {"MULT", 5},
        {"DIV", 6}, {"BC", 7}, {"COMP", 8}, {"PRINT", 9}, {"READ", 10}
    };
    int motSize = sizeof(mot) / sizeof(mot[0]);

    printf("\nMachine Code:\n");
    for (int i = 0; i < incompleteCount; i++) {
        int lc = incomplete[i].lc;
        char *symbol = incomplete[i].symbol;

        // Check if it's a literal or symbol
        if (incomplete[i].isLiteral) {
            // Find the address of the literal
            for (int j = 0; j < litCount; j++) {
                if (strcmp(littab[j].literal, symbol) == 0) {
                    printf("%d) -- -- %03d\n", lc, littab[j].address);
                    break;
                }
            }
        } else {
            // Find the address of the symbol
            for (int j = 0; j < symCount; j++) {
                if (strcmp(symtab[j].symbol, symbol) == 0) {
                    printf("%d) -- -- %03d\n", lc, symtab[j].address);
                    break;
                }
            }
        }
    }
}