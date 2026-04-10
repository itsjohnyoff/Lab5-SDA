#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
    #include <fcntl.h>
#endif

#define MAX_BUFFER 2048

/* Returns how many bytes the current sentence-ending punctuation takes.
   Returns 0 if current position is NOT a sentence-ender.
   Handles both ASCII and Chinese/fullwidth punctuation (UTF-8). */
int sentenceEndLen(char* current) {

    unsigned char* u = (unsigned char*)current;

    /* ASCII punctuation: . ! ? */
    if (u[0] == '.' || u[0] == '!' || u[0] == '?') {
        return 1;
    }

    /* Chinese/fullwidth punctuation (3-byte UTF-8 sequences):
       。= E3 80 82
       ？= EF BC 9F
       ！= EF BC 81                                          */
    if (u[0] == 0xE3 && u[1] == 0x80 && u[2] == 0x82) return 3; /* 。*/
    if (u[0] == 0xEF && u[1] == 0xBC && u[2] == 0x9F) return 3; /* ？*/
    if (u[0] == 0xEF && u[1] == 0xBC && u[2] == 0x81) return 3; /* ！*/

    return 0;
}

/* Splits the text into separate sentences using dynamic memory */
char** parseSentences(char* text, int* count) {
    int capacity = 10;

    char** sentences = (char**)malloc(capacity * sizeof(char*));

    *count = 0;

    char* start = text;
    char* current = text;

    while (*current != '\0') {

        /* Skip spaces before a sentence */
        if (start == current && *current == ' ') {
            start++;
            current++;
            continue;
        }

        /* Check if current position is a sentence-ending punctuation */
        int endLen = sentenceEndLen(current);

        if (endLen > 0) {

            /* Include the punctuation character(s) in the sentence */
            int len = (current - start) + endLen;

            sentences[*count] = (char*)malloc((len + 1) * sizeof(char));

            strncpy(sentences[*count], start, len);

            sentences[*count][len] = '\0';

            (*count)++;

            if (*count >= capacity) {
                capacity *= 2;
                sentences = (char**)realloc(sentences, capacity * sizeof(char*));
            }

            /* Jump over the punctuation (1 or 3 bytes) */
            current += endLen;
            start = current;

        } else {
            current++;
        }
    }

    /* Handles text without punctuation at the end */
    if (current > start) {
        int len = current - start;

        sentences[*count] = (char*)malloc((len + 1) * sizeof(char));

        strncpy(sentences[*count], start, len);

        sentences[*count][len] = '\0';

        (*count)++;
    }

    return sentences;
}

/* Swaps the first sentence with the sentence selected by the user */
void interchangeSentences(char** sentences, int count, int step) {

    if (step < 1 || step > count) {
        printf("Error: Step %d is out of range. There are only %d sentences.\n",
               step, count);
        return;
    }

    char* temp = sentences[0];
    sentences[0] = sentences[step - 1];
    sentences[step - 1] = temp;
}

/* Joins all sentences back into one text */
void joinSentences(char** sentences, int count, char* result) {

    result[0] = '\0';

    for (int i = 0; i < count; i++) {
        if (i > 0) {
            strcat(result, " ");
        }
        strcat(result, sentences[i]);
    }
}

int main(void) {
    setlocale(LC_ALL, "");

    #ifdef _WIN32
        SetConsoleCP(65001);
        SetConsoleOutputCP(65001);
        _setmode(_fileno(stdin),  _O_BINARY);
        _setmode(_fileno(stdout), _O_BINARY);
    #endif

    char inputBuffer[MAX_BUFFER];
    char beforeStr[MAX_BUFFER];
    char afterStr[MAX_BUFFER];

    int step;

    printf("Introduce the string:\n> ");
    fgets(inputBuffer, MAX_BUFFER, stdin);

    inputBuffer[strcspn(inputBuffer, "\n")] = '\0';
    inputBuffer[strcspn(inputBuffer, "\r")] = '\0';

    /* Write to file in BINARY mode - no encoding translation */
    FILE* fileOut = fopen("input.txt", "wb");
    if (!fileOut) {
        perror("Failed to create input.txt");
        return 1;
    }
    fprintf(fileOut, "%s", inputBuffer);
    fclose(fileOut);

    /* Read back in BINARY mode */
    FILE* fileIn = fopen("input.txt", "rb");
    if (!fileIn) {
        perror("Failed to open input.txt");
        return 1;
    }
    fgets(inputBuffer, MAX_BUFFER, fileIn);
    fclose(fileIn);

    inputBuffer[strcspn(inputBuffer, "\n")] = '\0';
    inputBuffer[strcspn(inputBuffer, "\r")] = '\0';

    printf("\nInput string is:\n%s\n", inputBuffer);

    printf("\nIntroduce your step of swaps: ");
    scanf("%d", &step);

    int count = 0;
    char** sentences = parseSentences(inputBuffer, &count);

    joinSentences(sentences, count, beforeStr);
    printf("\nBefore changes:\n%s\n", beforeStr);

    interchangeSentences(sentences, count, step);

    joinSentences(sentences, count, afterStr);
    printf("\nAfter changes:\n%s\n", afterStr);

    /* Save results in BINARY mode */
    FILE* finalOut = fopen("output.txt", "wb");
    if (!finalOut) {
        perror("Failed to create output.txt");
        return 1;
    }
    fprintf(finalOut,
            "Before changes:\n%s\n\nAfter changes:\n%s\n",
            beforeStr, afterStr);
    fclose(finalOut);

    printf("\nResults saved to output.txt\n");

    for (int i = 0; i < count; i++) {
        free(sentences[i]);
    }
    free(sentences);

    return 0;
}