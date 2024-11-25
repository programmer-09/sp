#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    char symbol[10];
    int address;
} syymbol;

typedef struct {
    char literal[10];
    int address;
} littab;

typedef struct {
    char mnemonic[10];
    int opcode;
} mot;

typedef struct {
    char reg[10];
    int code;
} registertab;


mot MOT[] = {
    {"MOVER", 1}, {"MOVEM", 2}, {"ADD", 3}, {"SUB", 4}, {"MULT", 5},
    {"DIV", 6}, {"BC", 7}, {"COMP", 8}, {"PRINT", 9}, {"READ", 10} ,{"SHIFT",11},{"BC",12}
};
mot POT[] = {
    {"START", 1}, {"END", 2}, {"EQU", 3}, {"ORIGIN", 4}, {"LTORG", 5}
};
mot DL[] = {
    {"DS", 1}, {"DC", 2}
};
registertab registers[] = {
    {"AREG", 1}, {"BREG", 2}, {"CREG", 3}, {"DREG", 4}
};

syymbol symtab[50];
littab litt[50];
int symcount = 0, litcount = 0, locctr = 0;

int searsys(char *symbol) {
    for (int i = 0; i < symcount; i++) {
        if (strcmp(symtab[i].symbol, symbol) == 0) {
            return i;
        }
    }
    return -1;
}

int searl(char *literal) {
    for (int i = 0; i < litcount; i++) {
        if (strcmp(litt[i].literal, literal) == 0) {
            return i;
        }
    }
    return -1;
}

int gop(char *mnemonic, mot table[], int size) {
    for (int i = 0; i < size; i++) {
        if (strcmp(table[i].mnemonic, mnemonic) == 0) {
            return table[i].opcode;
        }
    }
    return -1;
}


int grp(char *reg) {
    for (int i = 0; i < 4; i++) {
        if (strcmp(registers[i].reg, reg) == 0) {
            return registers[i].code;
        }
    }
    return -1;
}


void processLine(char *line, FILE *intermediate) {
    char label[10] = "", opcode[10] = "", operand1[10] = "", operand2[10] = "";
    int fields = sscanf(line, "%s %s %s %s", label, opcode, operand1, operand2);    
    if (fields >= 3 && strlen(label) > 0 && strcmp(opcode, "START") != 0 && strcmp(opcode, "END") != 0  && strcmp(label, "END") != 0 && strcmp(label, "START") != 0) {
        
        int symIndex = searsys(label);
        if (symIndex == -1) {
            strcpy(symtab[symcount].symbol, label);
            symtab[symcount].address = locctr;
            symcount++;
        } else {
            symtab[symIndex].address = locctr;
        }
    }
    else if (fields>=3 && strlen(label) > 0 && strcmp(opcode, "START") != 0 && strcmp(opcode, "END") != 0 && strcmp(label, "END") != 0 && strcmp(label, "START") != 0) {
        int symIndex = searsys(label);
        if (symIndex == -1) {
            strcpy(symtab[symcount].symbol, label);
            symtab[symcount].address = locctr;
            symcount++;
        } else {
            symtab[symIndex].address = locctr;
        }
    }
    if (fields==3 && operand1[0] == '='){
        int opcodeVal = gop(label, MOT, 11);        
        if (opcodeVal != -1) {
            int regCode = grp(opcode);
            int litIndex = searl(operand1);
            if (litIndex == -1 && operand1[0] == '=') {
                strcpy(litt[litcount].literal, operand1);
                litt[litcount].address = 0; 
                litcount++;
                litIndex = litcount - 1;
            }
            fprintf(intermediate, "%d\t(IS, %02d)\t(%d)\t(L, %d)\n", locctr, opcodeVal, regCode, litIndex + 1);
            locctr++;
        } else {
            
            opcodeVal = gop(opcode, POT, 5);
            if (opcodeVal != -1) {
                fprintf(intermediate, "%d\t(AD, %02d)\t(C, %s)\n", locctr, opcodeVal, operand1);
                if (strcmp(opcode, "START") == 0 || strcmp(opcode, "ORIGIN") == 0) {
                    locctr = atoi(operand1);
                    
                }
            } else {
                opcodeVal = gop(opcode, DL, 2);
                if (opcodeVal != -1) {
                    fprintf(intermediate, "%d\t(DL, %02d)\t(C, %s)\n", locctr, opcodeVal, operand1);
                    locctr += atoi(operand1);
                    
                }
            }
        }
    }
    else if (fields==3 && operand1[0] != '=' && (label[0]!='B' && label[1]!='C')){
        int opcodeVal = gop(label, MOT, 11);        
        if (opcodeVal != -1) {
            int regCode = grp(opcode);           
            fprintf(intermediate, "%d\t(IS, %02d)\t(%d)\t(S, %d)\n", locctr, opcodeVal, regCode, searsys(operand1)+1);
            locctr++;
        } else if (gop(opcode, POT, 5)==3){
            
        }else {
            
            opcodeVal = gop(opcode, POT, 5);
            if (opcodeVal != -1) {
                fprintf(intermediate, "%d\t(AD, %02d)\t(C, %s)\n", locctr, opcodeVal, operand1);
                if (strcmp(opcode, "START") == 0 || strcmp(opcode, "ORIGIN") == 0) {
                    locctr = atoi(operand1);
                    
                }
            } else {
                opcodeVal = gop(opcode, DL, 2);
                if (opcodeVal != -1) {
                   int symIndex = searsys(label);                
                   if (symIndex == -1) {                        
                        strcpy(symtab[symcount].symbol, label);
                        symtab[symcount].address = locctr;
                        symcount++;
                    } else {
                        symtab[symIndex].address = locctr;
                    }
                    fprintf(intermediate, "%d\t(DL, %02d)\t %s\n", locctr, opcodeVal, operand1);
                   
                   locctr ++;
                    
                }
            }
        }
    }
    else if (fields >= 3) {
              
        int opcodeVal = gop(opcode, MOT, 11);
        if (opcodeVal != -1) {
            int regCode = grp(operand1);
            int litIndex = searl(operand2);
            if (litIndex == -1 && operand2[0] == '=') {
                strcpy(litt[litcount].literal, operand2);
                litt[litcount].address = 0; 
                litcount++;
                litIndex = litcount - 1;
            }
            fprintf(intermediate, "%d\t(IS, %02d)\t(%d)\t(L, %d)\n", locctr, opcodeVal, regCode, litIndex + 1);
            locctr++;
        } else {
            
            opcodeVal = gop(opcode, POT, 5);
            if (opcodeVal != -1) {
                fprintf(intermediate, "%d\t(AD, %02d)\t(C, %s)\n", locctr, opcodeVal, operand1);
                if (strcmp(opcode, "START") == 0 || strcmp(opcode, "ORIGIN") == 0) {
                    locctr = atoi(operand1);
                    
                }
            } else if (strcmp(label, "BC") == 0) {
                   int conditionCode = atoi(opcode);
                   int symIndex = searsys(operand1);
                   if (symIndex == -1) {                        
                        strcpy(symtab[symcount].symbol, operand1);
                        symtab[symcount].address = locctr;
                        symcount++;
                    } else {
                        symtab[symIndex].address = locctr;
                    }
                        fprintf(intermediate, "%d\t(IS, %02d)\t(S, %d)\n", locctr, gop(label, MOT, 11), symcount);
                        locctr++;
                    
                }else {
                opcodeVal = gop(opcode, DL, 2);
                if (opcodeVal != -1) {
                   int symIndex = searsys(label);                
                   if (symIndex == -1) {                        
                        strcpy(symtab[symcount].symbol, label);
                        symtab[symcount].address = locctr;
                        symcount++;
                    } else {
                        symtab[symIndex].address = locctr;
                    }
                    fprintf(intermediate, "%d\t(DL, %02d)\t %s\n", locctr, opcodeVal, operand1);
                   
                   locctr ++;
                    
                    
                }
            }
        }
    } else if (fields == 2) {        
        int opcodeVal = gop(label, POT, 5);
        if (strcmp(label, "PRINT") == 0) {
            int regCode = grp(opcode); 
            if(regCode!=-1){
                fprintf(intermediate, "%d\t(IS, 09)\t(S, %d)\n", locctr, regCode);
                locctr++;
            }else if (searsys(opcode)!=-1){
                fprintf(intermediate, "%d\t(IS, 09)\t(S, %d)\n", locctr,searsys(opcode));
                locctr++;
            }else if (searl(opcode)!=-1){
                fprintf(intermediate, "%d\t(IS, 09)\t(L, %d)\n", locctr, searl(opcode));
                locctr++;
            }
        }
        else if (opcodeVal != -1) {
            if ( strcmp(label, "ORIGIN") == 0) {
                locctr = atoi(opcode);
                
            }
            else if  (strcmp(label, "START") == 0 ){
                locctr = atoi(opcode);
                fprintf(intermediate, "%d\t(AD, %02d)\t(C, %s)\n", locctr, opcodeVal, opcode);
            }
        } else if (gop(label, MOT, 11)==10){
            
            int symIndex = searsys(opcode);            
            if (symIndex == -1) {                
                strcpy(symtab[symcount].symbol, opcode);
                symtab[symcount].address = locctr;
                symcount++;
            } else {
                symtab[symIndex].address = locctr;
            }
            fprintf(intermediate, "%d\t(IS, %02d)\t(S, %d)\n", locctr, gop(label, MOT, 11), symcount);
            locctr++;
        }else if(gop(label, MOT, 11)!=-1){
            
            if(opcode[0]!='='){                
                opcodeVal = gop(label, MOT, 11);
                int reg=0;
                reg=grp(opcode);
                if(reg==-1){
                    fprintf(intermediate, "%d\t(IS, %02d)\t(S, %d)\n", locctr, opcodeVal, opcode);
                    locctr++;
                }else{
                    fprintf(intermediate, "%d\t(IS, %02d)\t(S, %d)\n", locctr, opcodeVal, reg);
                    locctr++;
                }
             }else{
                int litIndex = searl(opcode);
                opcodeVal = gop(label, MOT, 11);
                if (litIndex == -1 && opcode[0] == '=') {
                    strcpy(litt[litcount].literal, opcode);
                    litt[litcount].address = 0; 
                    litcount++;
                    litIndex = litcount - 1;
                }
                fprintf(intermediate, "%d\t(IS, %02d)\t(L, %d)\n", locctr, opcodeVal, litIndex + 1);
                locctr++;
            }
        }else {
            opcodeVal = gop(label, DL, 2);
            if (opcodeVal != -1) {                
                fprintf(intermediate, "%d\t(DL, %02d)\t(C, %s)\n", locctr, opcodeVal, opcode);
                locctr += atoi(opcode);
                
            } else {
                opcodeVal = gop(label, MOT, 11);
                if (opcodeVal != -1) {
                    int regCode = grp(opcode);
                    fprintf(intermediate, "%d\t(IS, %02d)\t(%d)\n", locctr, opcodeVal, regCode);
                }
            }
        }
        
    } else if (fields == 1) {
     
        int opcodeVal = gop(label, POT, 5);
       
        if (opcodeVal != -1) {
            if (strcmp(label, "LTORG") == 0||strcmp(label, "END") == 0) {                
                for (int i = 0; i < litcount; i++) {
                    if (litt[i].address == 0) {
                        litt[i].address = locctr;
                        // fprintf(intermediate, "%d\t(AD, %02d)\t%d\n", locctr, opcodeVal, litt[i].address);
                        locctr++;
                    }
                }
            }
        } 
    }else if (strcmp(label, "BC") == 0) {
    

            int conditionCode = atoi(operand1);
            if (conditionCode >= 1 && conditionCode <= 12) {
                fprintf(intermediate, "%d\t(IS, %02d)\t(C, %d)\n", locctr, gop(opcode, MOT, 11), conditionCode);
                locctr++;
            } 
        }
    else {
            fprintf(intermediate, "%d\t%s\n", locctr, label);
            locctr++;
        }
    }



void writeLiteralTableToFile() {
    FILE *file = fopen("literal.txt", "w"); 

    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }
    
    fprintf(file, "Literal Table:\n");
    for (int i = 0; i <= litcount; i++) {
        fprintf(file, "%s\t%d\n", litt[i].literal, litt[i].address); 
    }
    fclose(file);
    printf("Literal table written to literal.txt\n");
}

int main() {
    FILE *input = fopen("input.txt", "r");
    FILE *intermediate = fopen("intermediate.txt", "w");    

    char line[50];
    while (fgets(line, sizeof(line), input)) {
        processLine(line, intermediate);
    }

    fclose(input);
    fclose(intermediate);

    intermediate = fopen("intermediate.txt", "r");
    

    fclose(intermediate);    
    writeLiteralTableToFile();
  
    printf("Literal Table:\n");
    for (int i = 0; i <= litcount; i++) {
        printf("%s\t%d\n", litt[i].literal, litt[i].address);
    }




    return 0;
}
