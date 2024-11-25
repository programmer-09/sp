#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MNT 50
#define MAX_MDT 100
#define MAX_ALA 10

typedef struct {
    char name[10];
    int mdtIndex; 
} MNTEntry;

typedef struct {
    char definition[50];
} MDTEntry;

MNTEntry MNT[MAX_MNT];  
MDTEntry MDT[MAX_MDT];  
int mntCount = 0, mdtCount = 1;

void storeMacroDefinition(char *line) {
    strcpy(MDT[mdtCount++].definition, line);
}

void processMacroDefinition(FILE *input, FILE *nonMacroFile) {
    char line[100], macroName[10];

    while (fgets(line, sizeof(line), input)) {
        if (strstr(line, "MACRO")) {
            fscanf(input, "%s", macroName);  
            strcpy(MNT[mntCount].name, macroName);
            MNT[mntCount].mdtIndex = mdtCount;
            mntCount++;

            fgets(line, sizeof(line), input); 

            while (fgets(line, sizeof(line), input)) {
                if (strstr(line, "MEND")) {
                    storeMacroDefinition(line);  
                    break;
                }
                storeMacroDefinition(line);  
            }
        } else {
            fprintf(nonMacroFile, "%s", line);  // Write non-macro lines to nput.txt
        }
    }
}

void saveTables() {
    FILE *mntFile = fopen("MNT.txt", "w");
    FILE *mdtFile = fopen("MDT.txt", "w");

    if (!mntFile || !mdtFile) {
        printf("Error opening file for saving tables.\n");
        return;
    }

    for (int i = 0; i < mntCount; i++) {
        fprintf(mntFile, "%s\t%d\n", MNT[i].name, MNT[i].mdtIndex);
    }

    for (int i = 0; i < mdtCount; i++) {
        fprintf(mdtFile, "%s", MDT[i].definition);
    }

    fclose(mntFile);
    fclose(mdtFile);
}

int main() {
    FILE *input = fopen("input.txt", "r");
    FILE *nonMacroFile = fopen("intermediate.txt", "w");  

    if (!input || !nonMacroFile) {
        printf("Error opening file.\n");
        return 1;
    }

    processMacroDefinition(input, nonMacroFile);

    fclose(input);
    fclose(nonMacroFile);

    saveTables();  
    
    return 0;
}
