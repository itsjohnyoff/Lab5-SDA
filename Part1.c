#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER 2048

/* Dynamically parses text into sentences and keeps the punctuation.
   Returns an array of string pointers.
*/
char** parseSentences(char* text, int* count) {
    int capacity = 10;
    // Allocate memory for an array of pointers
    char** sentences = (char**)malloc(capacity * sizeof(char*));
    *count = 0;

    char* start = text;
    char* current = text;

    while (*current != '\0') {
        // Skip spaces at the beginning of a new sentence
        if (start == current && *current == ' ') {
            start++;
            current++;
            continue;
        }

        // When we hit punctuation, extract the sentence
        if (*current == '.' || *current == '!' || *current == '?') {
            int len = current - start + 1; // +1 to include the punctuation
            
            // Dynamically allocate memory for this specific sentence
            sentences[*count] = (char*)malloc((len + 1) * sizeof(char));
            strncpy(sentences[*count], start, len);
            sentences[*count][len] = '\0';
            (*count)++;

            // Expand array if we run out of pointer space
            if (*count >= capacity) {
                capacity *= 2;
                sentences = (char**)realloc(sentences, capacity * sizeof(char*));
            }
            current++;
            start = current;
        } else {
            current++;
        }
    }

    // Catch any leftover text that didn't end with punctuation
    if (current > start) {
        int len = current - start;
        sentences[*count] = (char*)malloc((len + 1) * sizeof(char));
        strncpy(sentences[*count], start, len);
        sentences[*count][len] = '\0';
        (*count)++;
    }

    return sentences;
}

/* Interchanges sentences by swapping their pointers 
   (much more efficient than copying text).
*/
void interchangeSentences(char** sentences, int count, int step) {
    if (step < 1 || step > count) {
        printf("Error: Step %d is out of range. There are only %d sentences.\n", step, count);
        return;
    }
    
    // Swap the pointers directly
    char* temp = sentences[0];
    sentences[0] = sentences[step - 1];
    sentences[step - 1] = temp;
}

/* Joins the dynamically allocated sentences into one string.
*/
void joinSentences(char** sentences, int count, char* result) {
    result[0] = '\0';
    for (int i = 0; i < count; i++) {
        if (i > 0) strcat(result, " ");
        strcat(result, sentences[i]);
    }
}

int main(void) {
    char inputBuffer[MAX_BUFFER];
    char beforeStr[MAX_BUFFER];
    char afterStr[MAX_BUFFER];
    int step;

    // 1. Read from keyboard and write to input.txt
    printf("Introduce the string:\n> ");
    fgets(inputBuffer, MAX_BUFFER, stdin);
    inputBuffer[strcspn(inputBuffer, "\n")] = '\0'; 

    FILE* fileOut = fopen("input.txt", "w");
    if (!fileOut) {
        perror("Failed to create input.txt");
        return 1;
    }
    fprintf(fileOut, "%s", inputBuffer);
    fclose(fileOut);

    // 2. Read back from input.txt
    FILE* fileIn = fopen("input.txt", "r");
    if (!fileIn) {
        perror("Failed to open input.txt");
        return 1;
    }
    fgets(inputBuffer, MAX_BUFFER, fileIn);
    fclose(fileIn);

    printf("\nInput string is:\n%s\n", inputBuffer);

    // 3. Get the step
    printf("\nIntroduce your step of swaps: ");
    scanf("%d", &step);

    // 4. Parse sentences using dynamic memory
    int count = 0;
    char** sentences = parseSentences(inputBuffer, &count);

    // 5. Build and print the "before" string
    joinSentences(sentences, count, beforeStr);
    printf("\n--- Before changes ---\n%s\n", beforeStr);

    // 6. Swap and build the "after" string
    interchangeSentences(sentences, count, step);
    joinSentences(sentences, count, afterStr);
    printf("\n--- After changes ---\n%s\n", afterStr);

    // 7. Write results to output.txt
    FILE* finalOut = fopen("output.txt", "w");
    if (!finalOut) {
        perror("Failed to create output.txt");
        return 1;
    }
    fprintf(finalOut, "Before changes:\n%s\n\nAfter changes:\n%s\n", beforeStr, afterStr);
    fclose(finalOut);
    printf("\nResults successfully saved to output.txt\n");

    // 8. Free the dynamically allocated memory
    for (int i = 0; i < count; i++) {
        free(sentences[i]);
    }
    free(sentences);

    return 0;
}