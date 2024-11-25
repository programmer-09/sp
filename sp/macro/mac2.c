#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef struct {
    char name[10];
    int mdtIndex; 
}mntet;

typedef struct {
    char definition[50]; 
}mdtet;

mntet mnt[50];  
mdtet mdt[100];  
char ALA[10][10]; 
char bl[10][10]; 
int xc[10];
int mntCount = 0, mdtCount = 0;
int alaIndex = 0, c = 0, f = 0;

void trimWhitespace(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++; 
    end = str + strlen(str);
    while (end > str && isspace((unsigned char)*(end - 1))) end--; 
    *end = '\0'; 
}

int ca(int i) {
    for (int z = 0; z < alaIndex; z++) {
        if (strcmp(bl[z], ALA[z]) != 0) {
            xc[z] = i;
            return z;
        } else if (xc[z] == i) {
            return z;
        }
    }
}

void lt() {
    FILE *mntFile = fopen("MNT.txt", "r");
    FILE *mdtFile = fopen("MDT.txt", "r");



    while (fscanf(mntFile, "%s %d", mnt[mntCount].name, &mnt[mntCount].mdtIndex) != EOF) {
        mntCount++;
    }
    while (fgets(mdt[mdtCount].definition, sizeof(mdt[mdtCount].definition), mdtFile)) {
        trimWhitespace(mdt[mdtCount].definition); 
        mdtCount++;
    }

    fclose(mntFile);
    fclose(mdtFile);
}

int evaluateCondition(char *condition) {
    char arg[10], op[3], value[10];
    sscanf(condition, "&%s %s %s", arg, op, value);  
    printf("sfds%s %s %s\n",arg,op,value);
    //printf("sfds%s\n",condition);
    for (int i = 0; i < 10; i++) {
        if (strcmp(ALA[i], value) == 0 && strcmp(op, "EQ") == 0) {
            return 1; 
        }
    }
    return 0;  
}

void sag(char *line, FILE *output) {
    char resultLine[100] = "";
    int cnt = 0;
    int foundSubstitution = 0;

    printf("sfl: '%s'\n", line);

    for (int i = 0; i < 10; i++) {
        char formalParam[6];
        sprintf(formalParam, "&ARG%d", i + 1);  

        char *argPos = strstr(line, formalParam);  
        if (argPos != NULL) {
            if (f == 0) {
                strcpy(bl[c], ALA[c]);
                xc[c] = i + 1;
                cnt = 0;
                c++;
                f = 1;
            } else {
                cnt = ca(i + 1);
                strcpy(bl[c], ALA[c]);
                c++;
            }

            printf("subt '%s' with '%s' %d\n", formalParam, ALA[cnt], cnt); 
            strncat(resultLine, line, argPos - line); 
            strcat(resultLine, ALA[cnt]);
            strcat(resultLine, argPos + strlen(formalParam)); 
            strcpy(line, resultLine);
        }
    }

    printf("ALA:\n");
    for (int j = 0; j < 5; j++) {
        printf("ALA[%d]: '%s'\n", j, ALA[j]);
    }

    if (!foundSubstitution) {
        strcpy(resultLine, line); 
    }

    printf("Fls: '%s'\n", resultLine);
    fprintf(output, "%s\n", resultLine);
}

int isMacro(char *name) {
    for (int i = 0; i < mntCount; i++) {
        if (strcmp(name, mnt[i].name) == 0) {
            return 1;
        }
    }
    return 0;
}

int findMDT(char *macroName) {
    for (int i = 0; i < mntCount; i++) {
        if (strcmp(mnt[i].name, macroName) == 0) {
            return mnt[i].mdtIndex;
        }
    }
    return -1;
}

int findLabelInMDT(char *label) {
    for (int i = 0; i < mdtCount; i++) {
        if (strncmp(mdt[i].definition, label, strlen(label)) == 0 && 
            (mdt[i].definition[strlen(label)] == ' ' || mdt[i].definition[strlen(label)] == '\0')) {
            return i;
        }
    }
    return -1;
}

void expandMacro(char *macroName, char *line, FILE *output) {
    char actualArgs[100];
    strcpy(actualArgs, line + strlen(macroName));
    trimWhitespace(actualArgs);

    char *arg = strtok(actualArgs, ", ");
    while (arg != NULL && alaIndex < 10) {
        trimWhitespace(arg);
        int isMacro = findMDT(arg);
        if (isMacro == -1) {
            strcpy(ALA[alaIndex++], arg);
        } else {
            printf("Skipping macro name: '%s' from being added to ALA.\n", arg);
        }
        arg = strtok(NULL, ", ");
    }

    int mdtptr = findMDT(macroName);
    int labelFlag = 0,cf=0;

    while (mdtptr != -1 && strcmp(mdt[mdtptr].definition, "MEND") != 0) {
        char command[10],t[20],t1[20];
        sscanf(mdt[mdtptr].definition, "%s", command);
        int fields=sscanf(mdt[mdtptr].definition,"%s %s ",t,t1);
        printf("Processing mdt[%d]: '%s' %d\n", mdtptr, mdt[mdtptr].definition,fields);
        //printf("Processing mdt[%d]: '%s' \n", mdtptr, mdt[mdtptr].definition);
        if(fields<2 && labelFlag!=1 && cf!=1){
            return;
        }
        else if (labelFlag == 1) {
            labelFlag = 0;
        }else if(cf ==1){
            cf=0;
        } else if (strcmp(command, "AIF") == 0) {
            char condition[50], label[10],c1[50],c2[50];
            sscanf(mdt[mdtptr].definition, "AIF %s %s %s %s", condition,c1,c2, label);
            printf("\ndasd%s %s %s %s %s",condition,label,c1,c2);

            strcat(condition," ");
            strcat(condition,c1);
            strcat(condition," ");
            strcat(condition,c2);
            printf("\ncondition:%s\n",condition);
            if (evaluateCondition(condition)) {
                printf("Condition '%s' is true, jumping to label '%s'.\n", condition, label);
                mdtptr = findLabelInMDT(label);
                if (mdtptr == -1) {
                    printf("Error: Label '%s' not found in mdt.\n", label);
                    break;
                }
                cf=1;
                continue;
            }
        } else if (strcmp(command, "AGO") == 0) {
            char label[10];
            sscanf(mdt[mdtptr].definition, "AGO %s", label);
            printf("Jumping to label '%s'.\n", label);
            mdtptr = findLabelInMDT(label);
            labelFlag = 1;
            if (mdtptr == -1) {
                printf("Error: Label '%s' not found in mdt.\n", label);
                break;
            }
            continue;
        } else if (strcmp(command, "ANOP") == 0) {
            printf("ANOP encountered, skipping this line.\n");
            mdtptr++;
            continue;
        } else {
            char nmn[10];
            sscanf(mdt[mdtptr].definition, "%s", nmn);
            if (findMDT(nmn) != -1) {
                printf("nested macro: '%s'\n", nmn);
                expandMacro(nmn, mdt[mdtptr].definition, output);
            } else {
                sag(mdt[mdtptr].definition, output);
            }
        }
        mdtptr++;
    }

    if (strcmp(mdt[mdtptr].definition, "MEND") == 0) {
        printf("Finished macro: '%s'\n", macroName);
    } else {
        printf("mend missing !! macro '%s'.\n", macroName);
    }
}

int main() {
    FILE *input = fopen("intermediate.txt", "r");
    FILE *output = fopen("output.txt", "w");

    lt();
    char line[100];
    while (fgets(line, sizeof(line), input)) {
        trimWhitespace(line);
        if (strlen(line) > 0) {
            char macroName[10];
            sscanf(line, "%s", macroName);
            if (isMacro(macroName)) {
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
