#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char name[10];
    int mdtIndex; 
} MNTEntry;

typedef struct {
    char definition[50]; 
} MDTEntry;

MNTEntry mnt[50];
MDTEntry mdt[100];
char ALA[10][10];
int mntCount = 0, mdtCount = 0, alaIndex = 0;

void trimWhitespace(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++; 
    end = str + strlen(str);
    while (end > str && isspace((unsigned char)*(end - 1))) end--;
    *end = '\0'; 
}

void loadTables() {
    FILE *mntFile = fopen("MNT.txt", "r");
    FILE *mdtFile = fopen("MDT.txt", "r");
    while (fscanf(mntFile, "%s %d", mnt[mntCount].name, &mnt[mntCount].mdtIndex) != EOF) mntCount++;
    while (fgets(mdt[mdtCount].definition, sizeof(mdt[mdtCount].definition), mdtFile)) {
        trimWhitespace(mdt[mdtCount++].definition);
    }
    fclose(mntFile);
    fclose(mdtFile);
}

int findMDTIndex(char *macroName) {
    for (int i = 0; i < mntCount; i++) {
        if (strcmp(macroName, mnt[i].name) == 0) return mnt[i].mdtIndex;
    }
    return -1;
}

void expandMacro(char *macroName, char *line, FILE *output) {
    char args[100];
    strcpy(args, line + strlen(macroName));
    trimWhitespace(args);

    char *arg = strtok(args, ", ");
    while (arg != NULL && alaIndex < 10) {
        trimWhitespace(arg);
        strcpy(ALA[alaIndex++], arg);
        arg = strtok(NULL, ", ");
    }

    int mdtIndex = findMDTIndex(macroName);
    while (strcmp(mdt[mdtIndex].definition, "MEND") != 0) {
        char resultLine[100] = "";
        for (int i = 0; i < alaIndex; i++) {
            char placeholder[6], *pos;
            sprintf(placeholder, "&ARG%d", i + 1);
            pos = strstr(mdt[mdtIndex].definition, placeholder);
            if (pos) {
                strncat(resultLine, mdt[mdtIndex].definition, pos - mdt[mdtIndex].definition);
                strcat(resultLine, ALA[i]);
                strcat(resultLine, pos + strlen(placeholder));
            }
        }
        fprintf(output, "%s\n", (*resultLine ? resultLine : mdt[mdtIndex].definition));
        mdtIndex++;
    }
}

int main() {
    FILE *input = fopen("intermediate.txt", "r");
    FILE *output = fopen("output.txt", "w");
    loadTables();

    char line[100];
    while (fgets(line, sizeof(line), input)) {
        trimWhitespace(line);
        if (strlen(line) > 0) {
            char macroName[10];
            sscanf(line, "%s", macroName);
            if (findMDTIndex(macroName) != -1) {
                expandMacro(macroName, line, output);
            } else {
                fprintf(output, "%s\n", line);
            }
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}
