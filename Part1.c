#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER 2048

/* Splits the text into separate sentences using dynamic memory */
char** parseSentences(char* text, int* count) {
    int capacity = 10;

    // Allocate memory for 10 sentence pointers
    char** sentences = (char**)malloc(capacity * sizeof(char*));

    // At the beginning there are no sentences
    *count = 0;

    // start marks where a sentence begins
    // current moves through the text character by character
    char* start = text;
    char* current = text;

    // Continue until end of string
    while (*current != '\0') {

        // Skip spaces before a sentence
        if (start == current && *current == ' ') {
            start++;
            current++;
            continue;
        }

        // Detect end of sentence
        if (*current == '.' || *current == '!' || *current == '?') {

            // Calculate sentence length
            int len = current - start + 1;

            // Allocate memory for the sentence
            sentences[*count] = (char*)malloc((len + 1) * sizeof(char));

            // Copy sentence into allocated memory
            strncpy(sentences[*count], start, len);

            // Add end of string character
            sentences[*count][len] = '\0';

            // Increase sentence count
            (*count)++;

            // Increase memory size if needed
            if (*count >= capacity) {
                capacity *= 2;

                sentences = (char**)realloc(sentences,
                                         capacity * sizeof(char*));
            }

            // Move to next sentence
            current++;
            start = current;
        } else {

            // Continue scanning text
            current++;
        }
    }

    // Handles text without punctuation at the end
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

    // Check if entered step is valid
    if (step < 1 || step > count) {
        printf("Error: Step %d is out of range. There are only %d sentences.\n",
               step, count);

        return;
    }

    // Save first sentence temporarily
    char* temp = sentences[0];

    // Put selected sentence first
    sentences[0] = sentences[step - 1];

    // Put original first sentence in selected position
    sentences[step - 1] = temp;
}

/* Joins all sentences back into one text */
void joinSentences(char** sentences, int count, char* result) {

    // Start with empty string
    result[0] = '\0';

    for (int i = 0; i < count; i++) {

        // Add spaces only between sentences
        if (i > 0) {
            strcat(result, " ");
        }

        // Append current sentence
        strcat(result, sentences[i]);
    }
}

int main(void) {

    char inputBuffer[MAX_BUFFER];
    char beforeStr[MAX_BUFFER];
    char afterStr[MAX_BUFFER];

    int step;

    // Read text from keyboard
    printf("Introduce the string:\n> ");

    fgets(inputBuffer, MAX_BUFFER, stdin);

    // Remove newline left by fgets
    inputBuffer[strcspn(inputBuffer, "\n")] = '\0';

    // Create input.txt and write text into it
    FILE* fileOut = fopen("input.txt", "w");

    if (!fileOut) {
        perror("Failed to create input.txt");

        return 1;
    }

    fprintf(fileOut, "%s", inputBuffer);

    fclose(fileOut);

    // Open input.txt for reading
    FILE* fileIn = fopen("input.txt", "r");

    if (!fileIn) {
        perror("Failed to open input.txt");

        return 1;
    }

    // Read text back from file
    fgets(inputBuffer, MAX_BUFFER, fileIn);

    fclose(fileIn);

    printf("\nInput string is:\n%s\n", inputBuffer);

    // Ask user which sentence will be swapped
    printf("\nIntroduce your step of swaps: ");

    scanf("%d", &step);

    int count = 0;

    // Split text into separate sentences
    char** sentences = parseSentences(inputBuffer, &count);

    // Build original text
    joinSentences(sentences, count, beforeStr);

    printf("\nBefore changes:\n%s\n", beforeStr);

    // Swap first sentence with selected sentence
    interchangeSentences(sentences, count, step);

    // Build modified text
    joinSentences(sentences, count, afterStr);

    printf("\nAfter changes:\n%s\n", afterStr);

    // Save results into output.txt
    FILE* finalOut = fopen("output.txt", "w");

    if (!finalOut) {
        perror("Failed to create output.txt");

        return 1;
    }

    fprintf(finalOut,
            "Before changes:\n%s\n\nAfter changes:\n%s\n",
            beforeStr, afterStr);

    fclose(finalOut);

    printf("\nResults saved to output.txt\n");

    // Free memory for each sentence
    for (int i = 0; i < count; i++) {
        free(sentences[i]);
    }

    // Free main pointer array
    free(sentences);

    return 0;
}