#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

const char *keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "int", "long", "register", "return", "short", "signed", "sizeof",
    "static", "struct", "switch", "typedef", "union", "unsigned", "void",
    "volatile", "while"
};


int isKeyword(char *word) {
    for (int i = 0; i < sizeof(keywords) / sizeof(char *); i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}


int isOperator(char ch) {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' ||
           ch == '=' || ch == '<' || ch == '>' || ch == '!' || ch == '&' || 
           ch == '|' || ch == '^' || ch == '~';
}


int isPunctuation(char ch) {
    return ch == ';' || ch == ',' || ch == '(' || ch == ')' || ch == '{' ||
           ch == '}' || ch == '[' || ch == ']' || ch == '#';
}


void processLine(char *line) {
    char token[100];
    int i = 0, j = 0;
    while (line[i] != '\0') {
        
        if (isspace(line[i])) {
            i++;
            continue;
        }

        
        if (isalpha(line[i]) || line[i] == '_') {
            j = 0;
            while (isalnum(line[i]) || line[i] == '_') {
                token[j++] = line[i++];
            }
            token[j] = '\0';
            if (isKeyword(token)) {
                printf("Keyword: %s\n", token);
            } else {
                printf("Identifier: %s\n", token);
            }
        }
        
        else if (isdigit(line[i])) {
            j = 0;
            while (isdigit(line[i])) {
                token[j++] = line[i++];
            }
            token[j] = '\0';
            printf("Integer Constant: %s\n", token);
        }
        
        else if (isOperator(line[i])) {
            printf("Operator: %c\n", line[i]);
            i++;
        }
        
        else if (line[i] == '\"') {
            j = 0;
            token[j++] = line[i++]; 
            while (line[i] != '\"' && line[i] != '\0') {
                token[j++] = line[i++];
            }
            if (line[i] == '\"') {
                token[j++] = line[i++]; 
                token[j] = '\0';
                printf("String Literal: %s\n", token);
            }
        }
        
        else if (isPunctuation(line[i])) {
            printf("Punctuation: %c\n", line[i]);
            i++;
        }
        else {
            i++; 
        }
    }
}

int main() {
    FILE *input = fopen("sample.c", "r");
    if (!input) {
        printf("Error: Could not open file.\n");
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), input)) {
        processLine(line);
    }

    fclose(input);
    return 0;
}
