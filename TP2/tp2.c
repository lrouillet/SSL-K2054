#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* readFile(char* filename);
void processData(char* data, char* destination);
void addRowToHtml(char* htmlContent, char** columns);
void addRowtoPrintCSV(char* printContent, char** columns);

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

void addRowtoPrintCSV(char* printContent, char** columns) {
    char buff[500000];
    snprintf(buff, sizeof(buff), "%s %s\n", columns[SPECIES_COLUMN], columns[VARIATION_COLUMN]);
    strcat(printContent, buff);
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

    char csvContent[500000];
    char htmlContent[500000];
    char printContent[500000];

    char selectedOption;

    FILE* csvFile = NULL;
    FILE* htmlFile = NULL;

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
    
    while( row != NULL ) {
        columns[0] = strtok(row, ";");
        for(i=1; i<16; i++) {
            columns[i] = strtok(NULL, ";");
        }

        if(strchr(columns[VARIATION_COLUMN], '-') != NULL) {
            addRowtoPrintCSV(printContent, columns);
            addRowToHtml(htmlContent, columns);
        }

        snprintf(csvRowBuffer, sizeof(csvRowBuffer), "%s;%s;%s;%s;%s;%s\n", columns[0], columns[3], columns[4], columns[8], columns[9], columns[10]);
        strcat(csvContent, csvRowBuffer);

        row = strtok_r(NULL, "\n", &rest);
    }

    while(selectedOption != 'q') {
        printf("Elija una opcion:\n");
        printf("A: Imprimir especies con variacion negativa por consola\n");
        printf("B: Crear reporte CSV.\n");
        printf("C: Crear reporte HTML.\n");
        printf("q: Salir del programa.\n\n");

        scanf(" %c", &selectedOption);
        switch(selectedOption) {
            case 'A':
                printf("%s\n", printContent);
            break;
            case 'B':
                csvFile = fopen("./csvReport.csv", "w");
                fwrite(csvHeader, strlen(csvHeader), 1, csvFile);
                fwrite(csvContent, strlen(csvContent), 1, csvFile);
                fclose(csvFile);
                printf("Reporte CSV generado. \n\n");
            break;
            case 'C':
                htmlFile = fopen("./htmlReport.html", "w");
                fwrite(htmlHead, strlen(htmlHead), 1, htmlFile);
                fwrite(htmlContent, strlen(htmlContent), 1, htmlFile);
                fwrite(htmlTail, strlen(htmlTail), 1, htmlFile);
                fclose(htmlFile);
                printf("Reporte HTML generado. \n\n");
            break;
            case 'q':
                printf("Hasta luego! \n");
            break;
            default:
                printf("Opcion incorrecta. \n");
        }
    }
}

void addRowToHtml(char* htmlContent, char** columns) {
    float aValue = atof(columns[8]);
    float pValue = atof(columns[3]);
    float sValue = atof(columns[4]);
    char backgroundColor[6];

    if(aValue > pValue && aValue > sValue) {
        strcat(backgroundColor, "green");
    } else {
        strcat(backgroundColor, "none");
    }

    char htmlRowBuffer[500000];
    snprintf(htmlRowBuffer, sizeof(htmlRowBuffer), "<tr style=\"background-color:%s\"><td>%s</td><td>%s</td></tr>\n", backgroundColor, columns[0], columns[7]);
    strcat(htmlContent, htmlRowBuffer);
}