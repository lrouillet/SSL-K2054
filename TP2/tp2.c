#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* readFile(char* filename);
void processData(char* data, char* destination);
void printLine(char** columns);
void addRowToHtml(FILE* htmlFile, char* species, char* variation, char* purchaseValue, char* saleValue, char* apertureValue);

int VARIATION_COLUMN = 7;
int SPECIES_COLUMN = 0;

int main (void)
{
    char* fileContent = readFile("./SSL-TP2.html");
    char* csv = NULL;
    int dataStartPosition = 0;
    int startPositionFound = 0;
    int endPositionFound = 0;
    int i = 0;
    char html[500000];

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

    processData(fileContent + dataStartPosition, csv);
}

void printLine(char** columns) {
    printf("%s %s\n", columns[SPECIES_COLUMN], columns[VARIATION_COLUMN]);
}

char* readFile(char* filename)
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

void processData(char* data, char* destination)
{
    int i = 0;
    int rowOpenTag = 0;
    int dataOpenTag = 0;
    int spanOpenTag = 0;
    int iOpenTag = 0;
    char csv[500000];
    char csvHeader[] = "Especie;Precio de compra;Precio de venta;Apertura;Precio Máximo;Precio Mínimo\n";
    char htmlHead[] = "<html>\n<table>\n<thead>\n<tr><th>Especie</th><th>Variacion</th></tr>\n</thead>\n";
    char htmlTail[] = "</table>\n</html>";
    char csvRowBuffer[20000];
    char* rest = csv;
    int lastIndex = 0;
    char auxChar;
    char* row;
    char* columns[16];
    FILE* csvFile;
    FILE* htmlFile;

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
                csv[lastIndex] = '\n';
                lastIndex++;
                i+=4;
            } else if(strncmp(data + i, "</td", 4) == 0) {
                csv[lastIndex] = ';';
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
            csv[lastIndex] = auxChar;
            lastIndex++;
        }

        i++;
    }
    csv[lastIndex] = '\0';

    row = strtok_r(csv, "\n", &rest);

    csvFile = fopen("./csvReport.csv", "w");
    htmlFile = fopen("./htmlReport.html", "w");
    fwrite(htmlHead, strlen(htmlHead), 1, htmlFile);
    fwrite(csvHeader, strlen(csvHeader), 1, csvFile);
    
    while( row != NULL ) {
        columns[0] = strtok(row, ";");
        for(i=1; i<16; i++) {
            columns[i] = strtok(NULL, ";");
        }

        if(strchr(columns[VARIATION_COLUMN], '-') != NULL) {
            printLine(columns);
            addRowToHtml(htmlFile, columns[0], columns[7], columns[3], columns[4], columns[8]);
        }

        snprintf(csvRowBuffer, sizeof(csvRowBuffer), "%s;%s;%s;%s;%s;%s\n", columns[0], columns[3], columns[4], columns[8], columns[9], columns[10]);
        fwrite(csvRowBuffer, strlen(csvRowBuffer), 1, csvFile);

        row = strtok_r(NULL, "\n", &rest);
    }

    fwrite(htmlTail, strlen(htmlTail), 1, htmlFile);

    fclose(csvFile);
    fclose(htmlFile);
}

void addRowToHtml(FILE* htmlFile, char* species, char* variation, char* purchaseValue, char* saleValue, char* apertureValue) {
    float aValueFloat = atof(apertureValue);
    float pValueFloat = atof(purchaseValue);
    float sValueFloat = atof(saleValue);

    fprintf(htmlFile, "<tr><td>%s</td><td>%s</td></tr>\n", species, variation);
}