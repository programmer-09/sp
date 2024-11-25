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

typedef struct {
    char arguments[MAX_ALA][10];  
    int count;  
} ALAEntry;

MNTEntry MNT[MAX_MNT];  
MDTEntry MDT[MAX_MDT];  
ALAEntry ALA;  
int mntCount = 0, mdtCount = 1;

void storeMacroDefinition(char *line) {
    strcpy(MDT[mdtCount++].definition, line);
}

void saveALA() {
    FILE *alaFile = fopen("ALA.txt", "w");
    if (!alaFile) {
        printf("Error opening ALA file.\n");
        return;
    }

    for (int i = 0; i < ALA.count; i++) {
        fprintf(alaFile, "%s\n", ALA.arguments[i]);
    }

    fclose(alaFile);
}

void extractArguments(char *line) {
    char *token = strtok(line, " ,\n");
    ALA.count = 0;

    while (token) {
        strcpy(ALA.arguments[ALA.count++], token);
        token = strtok(NULL, " ,\n");
    }
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
            extractArguments(line);  

            while (fgets(line, sizeof(line), input)) {
                if (strstr(line, "MEND")) {
                    storeMacroDefinition(line);  
                    break;
                }
                storeMacroDefinition(line);  
            }
        } else {
            fprintf(nonMacroFile, "%s", line);  
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
    saveALA();  
    
    return 0;
}
