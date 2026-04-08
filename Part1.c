#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN     2048
#define MAX_SENT    100
#define MAX_SENT_LEN 512

/* ---------------------------------------------------------------
   Parse a text into individual sentences (split on '.', '!', '?')
   Returns the number of sentences found.
--------------------------------------------------------------- */
int parseSentences(const char *text, char sentences[][MAX_SENT_LEN]) {
    int count = 0;
    int i = 0;
    char buf[MAX_SENT_LEN];

    while (*text && count < MAX_SENT) {
        /* Skip leading spaces between sentences */
        if (i == 0 && *text == ' ') {
            text++;
            continue;
        }

        buf[i++] = *text;

        /* End of sentence detected */
        if (*text == '.' || *text == '!' || *text == '?') {
            buf[i] = '\0';
            strcpy(sentences[count++], buf);
            i = 0;
        }
        text++;
    }

    /* Handle trailing text with no ending punctuation */
    if (i > 0) {
        buf[i] = '\0';
        strcpy(sentences[count++], buf);
    }

    return count;
}

/* ---------------------------------------------------------------
   Interchange sentence at index 0 with sentence at index (step-1)
--------------------------------------------------------------- */
void interchangeSentences(char sentences[][MAX_SENT_LEN], int count, int step) {
    if (step < 1 || step > count) {
        printf("Error: step %d is out of range (1 to %d).\n", step, count);
        return;
    }

    char temp[MAX_SENT_LEN];
    strcpy(temp,               sentences[0]);
    strcpy(sentences[0],       sentences[step - 1]);
    strcpy(sentences[step - 1], temp);
}

/* ---------------------------------------------------------------
   Join sentences array back into a single string
--------------------------------------------------------------- */
void joinSentences(char sentences[][MAX_SENT_LEN], int count, char *result) {
    result[0] = '\0';
    for (int i = 0; i < count; i++) {
        if (i > 0) strcat(result, " ");
        strcat(result, sentences[i]);
    }
}

/* ===============================================================
   MAIN
=============================================================== */
int main(void) {
    char inputStr[MAX_LEN];
    char sentences[MAX_SENT][MAX_SENT_LEN];
    char before[MAX_LEN], after[MAX_LEN];
    int  step;

    /* ---- Step 1: Read string from keyboard and write to input.txt ---- */
    printf("Enter the string:\n> ");
    fgets(inputStr, MAX_LEN, stdin);
    inputStr[strcspn(inputStr, "\n")] = '\0'; /* remove trailing newline */

    FILE *fwrite = fopen("input.txt", "w");
    if (!fwrite) {
        perror("Cannot create input.txt");
        return 1;
    }
    fprintf(fwrite, "%s", inputStr);
    fclose(fwrite);

    /* ---- Step 2: Re-read the string from input.txt ---- */
    FILE *fread = fopen("input.txt", "r");
    if (!fread) {
        perror("Cannot open input.txt");
        return 1;
    }
    fgets(inputStr, MAX_LEN, fread);
    fclose(fread);

    printf("\nInput string is:\n%s\n", inputStr);

    /* ---- Step 3: Read step from keyboard ---- */
    printf("\nIntroduce your step of swaps: ");
    scanf("%d", &step);

    /* ---- Step 4: Parse sentences ---- */
    int count = parseSentences(inputStr, sentences);
    printf("Sentences found: %d\n", count);

    /* ---- Step 5: Build "before" string and display ---- */
    joinSentences(sentences, count, before);
    printf("\n--- Before changes ---\n%s\n", before);

    /* ---- Step 6: Interchange and build "after" string ---- */
    interchangeSentences(sentences, count, step);
    joinSentences(sentences, count, after);
    printf("\n--- After changes ---\n%s\n", after);

    /* ---- Step 7: Write results to output.txt ---- */
    FILE *fout = fopen("output.txt", "w");
    if (!fout) {
        perror("Cannot create output.txt");
        return 1;
    }
    fprintf(fout, "Before changes:\n%s\n\nAfter changes:\n%s\n", before, after);
    fclose(fout);

    printf("\nResults saved to output.txt\n");
    return 0;
}