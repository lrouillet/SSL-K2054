#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* leer_archivo(char* filename);
void getCSV(char* data, char* destination);
char* getColumnValue(char* row, int columnNumber);
char* replaceString(char* s, char* oldW, char* newW);

int VARIATION_COLUMN = 8;

int main (void)
{
    char* fileContent = leer_archivo("./SSL-TP2.html");
    char* csv = NULL;
    int dataStartPosition = 0;
    int startPositionFound = 0;
    int endPositionFound = 0;
    int i = 0;

    for(i = 0; *(fileContent + i) != '\0' && !startPositionFound; i++) {
        if (*(fileContent + i) == '<' && strncmp(fileContent + i, "<tbody>", 7) == 0) {
            dataStartPosition = i + 7;
            startPositionFound = 1;
        }
    }

    while(*(fileContent+i) != '\0' && !endPositionFound) {
        if (*(fileContent + i) == '<' && strncmp(fileContent + i, "</tbody", 6) == 0) {
            *(fileContent+i) = '\0';
            endPositionFound = 1;
        }
        
        i++;
    }

    getCSV(fileContent + dataStartPosition, csv);
}

char* leer_archivo(char* filename)
{
    FILE* file = fopen(filename,"r");
    if(file == NULL)
    {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long int size = ftell(file);
    rewind(file);

    char* content = calloc(size + 1, 1);

    fread(content,1,size,file);

    return content;
}

void getCSV(char* data, char* destination)
{
    int i = 0;
    int rowOpenTag = 0;
    int dataOpenTag = 0;
    int spanOpenTag = 0;
    int iOpenTag = 0;
    char auxString[500000];
    int lastIndex = 0;
    char auxChar;

    while(*(data + i) != '\0') {
        if (*(data + i) == '<') {
            if (strncmp(data + i, "<tr", 3) == 0) {
                rowOpenTag = 1;
            } else if(strncmp(data + i, "<td", 3) == 0) {
                dataOpenTag = 1;
            } else if(strncmp(data + i, "<span", 5) == 0) {
                spanOpenTag = 1;
            } else if(strncmp(data + i, "<i", 2) == 0) {
                iOpenTag = 1;
            } else if(strncmp(data + i, "</tr", 4) == 0) {
                auxString[lastIndex] = '\n';
                lastIndex++;
                i+=4;
            } else if(strncmp(data + i, "</td", 4) == 0) {
                auxString[lastIndex] = ';';
                lastIndex++;
                i+=4;
            } else if(strncmp(data + i, "</span", 6) == 0) {
                i+=6;
            } else if(strncmp(data + i, "</i", 3) == 0) {
                i+=3;
            }
        } else if (*(data + i) == '>') {
            if (iOpenTag) {
                iOpenTag = 0;
            } else if (spanOpenTag) {
                spanOpenTag = 0;
            } else if (dataOpenTag) {
                dataOpenTag = 0;
            } else if(rowOpenTag) {
                rowOpenTag = 0;
            }
        } else if (!rowOpenTag && !dataOpenTag && !spanOpenTag && !iOpenTag) {
            auxChar = *(data + i);
            auxString[lastIndex] = auxChar;
            lastIndex++;
        }

        i++;
    }
    auxString[lastIndex] = '\0';

    printf("%s", auxString);

    destination = (char*) malloc(sizeof(auxString));
    strcpy(destination, auxString);
}

char* getColumnValue(char* row, int columnNumber) {
    return "test";
}

char* replaceString(char* s, char* oldW, char* newW)
{
    char* result;
    int i, cnt = 0;
    int newWlen = strlen(newW);
    int oldWlen = strlen(oldW);
  
    for (i = 0; s[i] != '\0'; i++) {
        if (strstr(&s[i], oldW) == &s[i]) {
            cnt++;
  
            i += oldWlen - 1;
        }
    }

    result = (char*)malloc(i + cnt * (newWlen - oldWlen) + 1);
  
    i = 0;
    while (*s) {
        if (strstr(s, oldW) == s) {
            strcpy(&result[i], newW);
            i += newWlen;
            s += oldWlen;
        }
        else
            result[i++] = *s++;
    }
  
    result[i] = '\0';
    return result;
}