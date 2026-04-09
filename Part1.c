#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER 2048

/* Function parseSentences
   Dynamically parses a block of text into individual sentences.
   Returns dynamically allocated array of string pointers (char**).
*/
char** parseSentences(char* text, int* count) {
    int capacity = 10; 
    
    // Allocate initial memory for an array of 10 pointers
    char** sentences = (char**)malloc(capacity * sizeof(char*));
    *count = 0;

    // Use two pointers to track the beginning and current position of a sentence
    char* start = text;
    char* current = text;

    while (*current != '\0') {
        // Skip any leading spaces at the start of a new sentence
        if (start == current && *current == ' ') {
            start++;
            current++;
            continue;
        }

        // Check for end of sentence punctuation
        if (*current == '.' || *current == '!' || *current == '?') {
            // Calculate exact length using pointer arithmetic (+1 to include the punctuation mark)
            int len = current - start + 1; 
            
            // Dynamically allocate memory for this specific sentence (+1 for the null terminator)
            sentences[*count] = (char*)malloc((len + 1) * sizeof(char));
            
            // Copy the extracted sentence into the allocated memory
            strncpy(sentences[*count], start, len);
            sentences[*count][len] = '\0'; // Ensure the string is properly terminated
            (*count)++;

            // If we exceed our pointer array capacity, double the size using realloc
            if (*count >= capacity) {
                capacity *= 2;
                sentences = (char**)realloc(sentences, capacity * sizeof(char*));
            }
            
            // Move pointers forward to start scanning the next sentence
            current++;
            start = current;
        } else {
            current++;
        }
    }

    // special case that matches any leftover text at the end that didn't have punctuation
    if (current > start) {
        int len = current - start;
        sentences[*count] = (char*)malloc((len + 1) * sizeof(char));
        strncpy(sentences[*count], start, len);
        sentences[*count][len] = '\0';
        (*count)++;
    }

    return sentences;
}

/* Function interchangeSentences
   Swaps two sentences based on the user's step index.
   This swaps memory addresses instead of copying strings for O(1) efficiency.
*/
void interchangeSentences(char** sentences, int count, int step) {
    // Validate the user input to prevent out-of-bounds array access
    if (step < 1 || step > count) {
        printf("Error: Step %d is out of range. There are only %d sentences.\n", step, count);
        return;
    }
    
    // Swap the pointers directly using a temporary pointer
    // much faster and uses less memory than copying the actual text characters
    char* temp = sentences[0];
    sentences[0] = sentences[step - 1];
    sentences[step - 1] = temp;
}

/* Function joinSentences
   Reassembles the dynamically allocated array of sentences back into a single string.
*/
void joinSentences(char** sentences, int count, char* result) {
    result[0] = '\0'; // Initialize as an empty string
    
    for (int i = 0; i < count; i++) {
        if (i > 0) {
            strcat(result, " "); // Add a space between sentences
        }
        strcat(result, sentences[i]); // Append the sentence
    }
}

int main(void) {
    char inputBuffer[MAX_BUFFER];
    char beforeStr[MAX_BUFFER];
    char afterStr[MAX_BUFFER];
    int step;

    // --- Step 1: Read input from keyboard ---
    printf("Introduce the string:\n> ");
    fgets(inputBuffer, MAX_BUFFER, stdin);
    inputBuffer[strcspn(inputBuffer, "\n")] = '\0'; // Strip the trailing newline left by fgets

    // --- Step 2: Write standard input to input.txt ---
    FILE* fileOut = fopen("input.txt", "w");
    if (!fileOut) {
        perror("Failed to create input.txt");
        return 1;
    }
    fprintf(fileOut, "%s", inputBuffer);
    fclose(fileOut);

    // --- Step 3: Read data back from input.txt ---
    FILE* fileIn = fopen("input.txt", "r");
    if (!fileIn) {
        perror("Failed to open input.txt");
        return 1;
    }
    fgets(inputBuffer, MAX_BUFFER, fileIn);
    fclose(fileIn);

    printf("\nInput string is:\n%s\n", inputBuffer);

    // --- Step 4: Get swap index from user ---
    printf("\nIntroduce your step of swaps: ");
    scanf("%d", &step);

    // --- Step 5: Process data using dynamic memory ---
    int count = 0;
    // sentences is a double pointer holding our array of string addresses
    char** sentences = parseSentences(inputBuffer, &count); 

    // --- Step 6: Build and display the original text ---
    joinSentences(sentences, count, beforeStr);
    printf("\n--- Before changes ---\n%s\n", beforeStr);

    // --- Step 7: Perform pointer swap and display updated text ---
    interchangeSentences(sentences, count, step);
    joinSentences(sentences, count, afterStr);
    printf("\n--- After changes ---\n%s\n", afterStr);

    // --- Step 8: Save final results to output.txt ---
    FILE* finalOut = fopen("output.txt", "w");
    if (!finalOut) {
        perror("Failed to create output.txt");
        return 1;
    }
    fprintf(finalOut, "Before changes:\n%s\n\nAfter changes:\n%s\n", beforeStr, afterStr);
    fclose(finalOut);
    printf("\nResults saved to output.txt\n");

    // --- Step 9: Clean up dynamically allocated memory ---
    // free each individual sentence first, then free the main pointer array
    // This prevents memory leaks
    for (int i = 0; i < count; i++) {
        free(sentences[i]);
    }
    free(sentences);

    return 0;
}