#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// User-defined types
// These group related values into one meaningful unit.

typedef struct {
    int day;
    int month;
    int year;
} Date;

typedef struct {
    char city[50];
    char street[50];
    char postCode[20];
} Address;

// Main citizen record
// It combines personal data, addresses, and the extra fields needed later.
typedef struct {
    char name[50];
    char surname[50];
    Date dob;
    char gender;
    Address home;
    Address work;

    // These fields are filled after the age is calculated
    int ageYears;
    int ageMonths;
    int ageDays;
    int category;
    float amountPaid;
} Citizen;

/* Function prototypes */
void clearBuffer();
Citizen** createCitizens(int count);
void inputCitizensManually(Citizen** registry, int count);
Citizen** loadCitizensFromFile(const char* filename, int* count);
void calculateAgeAndCategory(Citizen* c, Date current);
void displayCitizen(Citizen* c);
void sortCitizensByCategory(Citizen** registry, int count);
void mergeFiles(const char* file1, const char* file2, const char* fileOut);

// Reads one line from a file, removes the newline, and skips empty lines.
static int readNonEmptyLine(FILE* file, char* out, size_t outSize);

int main() {
    int count = 0;
    int choice;
    Date currentDate;
    Citizen** registry = NULL;

    printf("--- Citizen Registry System ---\n\n");
    printf("1. Manual input\n");
    printf("2. Load from file\n");
    printf("Choice: ");
    scanf("%d", &choice);
    clearBuffer();

    // Choose how the citizens will be entered
    if (choice == 1) {
        // Manual input mode
        printf("\nEnter the number of citizens to register: ");
        scanf("%d", &count);
        clearBuffer();

        // Create the array of citizen pointers
        registry = createCitizens(count);
        inputCitizensManually(registry, count);
    } else if (choice == 2) {
        // File loading mode
        char filename[100];
        printf("\nEnter filename: ");
        fgets(filename, 100, stdin);
        filename[strcspn(filename, "\n")] = 0; // Remove the trailing newline

        registry = loadCitizensFromFile(filename, &count);
        if (registry == NULL) {
            printf("Failed to load citizens from file.\n");
            return 1;
        }
    } else {
        printf("Invalid choice.\n");
        return 1;
    }

    // Create experiment.txt and save the raw citizen data there
    FILE* expFile = fopen("experiment.txt", "w");
    if (!expFile) {
        perror("Could not create experiment.txt");
        return 1;
    }

    // Write every citizen to the experiment file
    for (int i = 0; i < count; i++) {
        fprintf(expFile, "%s %s | Gender: %c | DOB: %02d/%02d/%04d\n",
                registry[i]->name, registry[i]->surname, registry[i]->gender,
                registry[i]->dob.day, registry[i]->dob.month, registry[i]->dob.year);
        fprintf(expFile, "   Home: %s, str. %s, %s\n",
                registry[i]->home.city, registry[i]->home.street, registry[i]->home.postCode);
        fprintf(expFile, "   Work: %s, str. %s, %s\n\n",
                registry[i]->work.city, registry[i]->work.street, registry[i]->work.postCode);
    }
    fclose(expFile);

    // Show what was saved in the first file
    printf("\n=== Data Saved to experiment.txt ===\n");
    for (int i = 0; i < count; i++) {
        displayCitizen(registry[i]);
    }

    // Ask for the current date so age can be calculated
    printf("\nEnter the CURRENT DATE (DD MM YYYY) to calculate ages and amounts: ");
    scanf("%d %d %d", &currentDate.day, &currentDate.month, &currentDate.year);

    for (int i = 0; i < count; i++) {
        // The function updates the same citizen structure directly
        calculateAgeAndCategory(registry[i], currentDate);
    }

    // Sort citizens by category, highest first
    sortCitizensByCategory(registry, count);

    // Save the processed result into output.txt
    FILE* outFile = fopen("output.txt", "w");
    if (!outFile) {
        perror("Could not create output.txt");
        return 1;
    }

    fprintf(outFile, "--- Processed Citizens (Sorted by Category Descending) ---\n");
    for (int i = 0; i < count; i++) {
        fprintf(outFile, "Cat: %d | %s %s | Age: %d Y, %d M, %d D | Paid: %.2f | Home: %s | Work: %s\n",
                registry[i]->category, registry[i]->name, registry[i]->surname,
                registry[i]->ageYears, registry[i]->ageMonths, registry[i]->ageDays,
                registry[i]->amountPaid, registry[i]->home.city, registry[i]->work.city);
    }
    fclose(outFile);
    printf("\nProcessed calculations saved to output.txt.\n");

    // Merge the two files into one final result file
    mergeFiles("experiment.txt", "output.txt", "result.txt");
    printf("Merged experiment.txt and output.txt into result.txt.\n");

    // Free every citizen first, then free the pointer array
    for (int i = 0; i < count; i++) {
        free(registry[i]);
    }
    free(registry);

    return 0;
}

/* Function implementations */

// Clears the remaining characters from the input buffer
void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

// Creates the array that stores citizen pointers
Citizen** createCitizens(int count) {
    return (Citizen**)malloc(count * sizeof(Citizen*));
}

// Reads citizens one by one from the keyboard
void inputCitizensManually(Citizen** registry, int count) {
    for (int i = 0; i < count; i++) {
        // Allocate memory for one citizen
        registry[i] = (Citizen*)malloc(sizeof(Citizen));

        printf("\n--- Entering data for Citizen %d ---\n", i + 1);

        // fgets is used here because it allows spaces in names and addresses
        printf("Name: ");
        fgets(registry[i]->name, 50, stdin);
        registry[i]->name[strcspn(registry[i]->name, "\n")] = 0;

        printf("Surname: ");
        fgets(registry[i]->surname, 50, stdin);
        registry[i]->surname[strcspn(registry[i]->surname, "\n")] = 0;

        printf("Gender (m/f): ");
        scanf("%c", &registry[i]->gender);

        printf("Date of Birth (DD MM YYYY): ");
        scanf("%d %d %d", &registry[i]->dob.day, &registry[i]->dob.month, &registry[i]->dob.year);
        clearBuffer();

        // Home address
        printf("\n[ Home Address ]\n");
        printf("City: ");
        fgets(registry[i]->home.city, 50, stdin);
        registry[i]->home.city[strcspn(registry[i]->home.city, "\n")] = 0;

        printf("Street: ");
        fgets(registry[i]->home.street, 50, stdin);
        registry[i]->home.street[strcspn(registry[i]->home.street, "\n")] = 0;

        printf("Postal Code (e.g., MD-4580): ");
        fgets(registry[i]->home.postCode, 20, stdin);
        registry[i]->home.postCode[strcspn(registry[i]->home.postCode, "\n")] = 0;

        // Work address
        printf("\n[ Work Address ]\n");
        printf("City: ");
        fgets(registry[i]->work.city, 50, stdin);
        registry[i]->work.city[strcspn(registry[i]->work.city, "\n")] = 0;

        printf("Street: ");
        fgets(registry[i]->work.street, 50, stdin);
        registry[i]->work.street[strcspn(registry[i]->work.street, "\n")] = 0;

        printf("Postal Code: ");
        fgets(registry[i]->work.postCode, 20, stdin);
        registry[i]->work.postCode[strcspn(registry[i]->work.postCode, "\n")] = 0;
    }
}

// Loads citizens from a text file
Citizen** loadCitizensFromFile(const char* filename, int* count) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Could not open file");
        return NULL;
    }

    *count = 0;

    // Temporary buffer for reading lines
    char buffer[128];

    // The first non-empty line contains the number of citizens
    if (!readNonEmptyLine(file, buffer, sizeof(buffer)) || sscanf(buffer, "%d", count) != 1 || *count <= 0) {
        printf("Error reading citizen count from file.\n");
        fclose(file);
        return NULL;
    }

    // Create the registry
    Citizen** registry = createCitizens(*count);
    if (registry == NULL) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return NULL;
    }

    // Set pointers to NULL in case something fails later
    for (int i = 0; i < *count; i++) {
        registry[i] = NULL;
    }

    // Read each citizen from the file
    for (int i = 0; i < *count; i++) {
        registry[i] = (Citizen*)malloc(sizeof(Citizen));
        if (registry[i] == NULL) {
            printf("Memory allocation failed for citizen %d.\n", i + 1);
            fclose(file);
            for (int k = 0; k < i; k++) {
                free(registry[k]);
            }
            free(registry);
            return NULL;
        }

        // Basic data
        if (!readNonEmptyLine(file, registry[i]->name, sizeof(registry[i]->name)) ||
            !readNonEmptyLine(file, registry[i]->surname, sizeof(registry[i]->surname)) ||
            !readNonEmptyLine(file, buffer, sizeof(buffer))) {
            printf("Error reading citizen %d basic data.\n", i + 1);
            fclose(file);
            for (int k = 0; k <= i; k++) {
                free(registry[k]);
            }
            free(registry);
            return NULL;
        }

        registry[i]->gender = buffer[0];

        // Date of birth
        if (!readNonEmptyLine(file, buffer, sizeof(buffer)) ||
            sscanf(buffer, "%d %d %d", &registry[i]->dob.day, &registry[i]->dob.month, &registry[i]->dob.year) != 3) {
            printf("Error reading citizen %d date of birth.\n", i + 1);
            fclose(file);
            for (int k = 0; k <= i; k++) {
                free(registry[k]);
            }
            free(registry);
            return NULL;
        }

        // Addresses
        if (!readNonEmptyLine(file, registry[i]->home.city, sizeof(registry[i]->home.city)) ||
            !readNonEmptyLine(file, registry[i]->home.street, sizeof(registry[i]->home.street)) ||
            !readNonEmptyLine(file, registry[i]->home.postCode, sizeof(registry[i]->home.postCode)) ||
            !readNonEmptyLine(file, registry[i]->work.city, sizeof(registry[i]->work.city)) ||
            !readNonEmptyLine(file, registry[i]->work.street, sizeof(registry[i]->work.street)) ||
            !readNonEmptyLine(file, registry[i]->work.postCode, sizeof(registry[i]->work.postCode))) {
            printf("Error reading citizen %d addresses.\n", i + 1);
            fclose(file);
            for (int k = 0; k <= i; k++) {
                free(registry[k]);
            }
            free(registry);
            return NULL;
        }
    }

    fclose(file);
    printf("Successfully loaded %d citizen(s) from file.\n", *count);
    return registry;
}

// Reads a line and ignores empty ones
static int readNonEmptyLine(FILE* file, char* out, size_t outSize) {
    while (fgets(out, (int)outSize, file)) {
        out[strcspn(out, "\n")] = 0;
        if (out[0] != '\0') {
            return 1;
        }
        // If the line is empty, keep reading
    }
    return 0;
}

// Prints one citizen in a readable form
void displayCitizen(Citizen* c) {
    printf("Citizen: %s %s, Gender: %c, DOB: %02d/%02d/%04d\n",
           c->name, c->surname, c->gender, c->dob.day, c->dob.month, c->dob.year);
    printf("  Home: %s, %s, %s\n", c->home.city, c->home.street, c->home.postCode);
    printf("  Work: %s, %s, %s\n\n", c->work.city, c->work.street, c->work.postCode);
}

// Calculates age and assigns category and payment
void calculateAgeAndCategory(Citizen* c, Date current) {
    c->ageDays = current.day - c->dob.day;
    c->ageMonths = current.month - c->dob.month;
    c->ageYears = current.year - c->dob.year;

    if (c->ageDays < 0) {
        c->ageMonths--;
        c->ageDays += 30; // Simple month approximation
    }
    if (c->ageMonths < 0) {
        c->ageYears--;
        c->ageMonths += 12;
    }

    // Category rules
    if (c->ageYears < 18) {
        c->category = 1;
        c->amountPaid = 500.0;
    } else if (c->ageYears >= 18 && c->ageYears < 65) {
        c->category = 2;
        c->amountPaid = 1500.0;
    } else {
        c->category = 3;
        c->amountPaid = 2500.0;
    }
}

/*
 * sortCitizensByCategory
 * Sorts the citizens in descending order by category.
 * Only the pointers are swapped, so the actual citizen data stays in place.
 */
void sortCitizensByCategory(Citizen** registry, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (registry[j]->category < registry[j + 1]->category) {
                // Swap pointer addresses, not full records
                Citizen* temp = registry[j];
                registry[j] = registry[j + 1];
                registry[j + 1] = temp;
            }
        }
    }
}

// Merges the content of two files into one output file
void mergeFiles(const char* file1, const char* file2, const char* fileOut) {
    FILE *f1 = fopen(file1, "r");
    FILE *f2 = fopen(file2, "r");
    FILE *fOut = fopen(fileOut, "w");
    char buffer[256];

    if (!f1 || !f2 || !fOut) {
        printf("Error opening files for merging.\n");
        return;
    }

    fprintf(fOut, "=== DATA FROM EXPERIMENT.TXT ===\n");
    while (fgets(buffer, sizeof(buffer), f1)) {
        fputs(buffer, fOut);
    }

    fprintf(fOut, "\n=== DATA FROM OUTPUT.TXT ===\n");
    while (fgets(buffer, sizeof(buffer), f2)) {
        fputs(buffer, fOut);
    }

    fclose(f1);
    fclose(f2);
    fclose(fOut);
}