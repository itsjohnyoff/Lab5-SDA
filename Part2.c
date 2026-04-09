#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- User-Defined Types (UDT) --- */
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

typedef struct {
    char name[50];
    char surname[50];
    Date dob;
    char gender;
    Address home;
    Address work;
    
    // Added to satisfy the "student/amount paid/age" requirement 
    int ageYears;
    int ageMonths;
    int ageDays;
    int category;
    float amountPaid;
} Citizen;

/* --- Function Prototypes --- */
void clearBuffer();
Citizen** createCitizens(int count);
void calculateAgeAndCategory(Citizen* c, Date current);
void displayCitizen(Citizen* c);
void sortCitizensByCategory(Citizen** registry, int count);
void mergeFiles(const char* file1, const char* file2, const char* fileOut);

int main() {
    int count;
    Date currentDate;

    printf("--- Citizen Registry System ---\n");
    printf("Enter the number of citizens to register: ");
    scanf("%d", &count);
    clearBuffer();

    // Dynamically allocate an array of POINTERS to Citizens
    Citizen** registry = createCitizens(count);

    // 1. Open experiment.txt and write data
    FILE* expFile = fopen("experiment.txt", "w");
    if (!expFile) {
        perror("Could not create experiment.txt");
        return 1;
    }

    for (int i = 0; i < count; i++) {
        registry[i] = (Citizen*)malloc(sizeof(Citizen));
        
        printf("\n--- Entering data for Citizen %d ---\n", i + 1);
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

        printf("Home City: ");
        fgets(registry[i]->home.city, 50, stdin);
        registry[i]->home.city[strcspn(registry[i]->home.city, "\n")] = 0;

        // Writing raw data to experiment.txt
        fprintf(expFile, "%s %s | Gender: %c | DOB: %02d/%02d/%04d | City: %s\n", 
                registry[i]->name, registry[i]->surname, registry[i]->gender, 
                registry[i]->dob.day, registry[i]->dob.month, registry[i]->dob.year, 
                registry[i]->home.city);
    }
    fclose(expFile);

    // 2. Display entered data on screen
    printf("\n=== Data Saved to experiment.txt ===\n");
    for (int i = 0; i < count; i++) {
        displayCitizen(registry[i]);
    }

    // 3. Process data (Age, Category, Amount Paid)
    printf("\nEnter the CURRENT DATE (DD MM YYYY) to calculate ages and amounts: ");
    scanf("%d %d %d", &currentDate.day, &currentDate.month, &currentDate.year);

    for (int i = 0; i < count; i++) {
        calculateAgeAndCategory(registry[i], currentDate);
    }

    // 4. Sort descending by category using pointers
    sortCitizensByCategory(registry, count);

    // 5. Write calculations to output.txt
    FILE* outFile = fopen("output.txt", "w");
    if (!outFile) {
        perror("Could not create output.txt");
        return 1;
    }
    
    fprintf(outFile, "--- Processed Citizens (Sorted by Category Descending) ---\n");
    for (int i = 0; i < count; i++) {
        fprintf(outFile, "Category: %d | Name: %s %s | Age: %d Y, %d M, %d D | Amount Paid: %.2f\n",
                registry[i]->category, registry[i]->name, registry[i]->surname,
                registry[i]->ageYears, registry[i]->ageMonths, registry[i]->ageDays,
                registry[i]->amountPaid);
    }
    fclose(outFile);
    printf("\nProcessed calculations saved to output.txt.\n");

    // 6. Merge files into result.txt
    mergeFiles("experiment.txt", "output.txt", "result.txt");
    printf("Merged experiment.txt and output.txt into result.txt.\n");

    // Free memory
    for (int i = 0; i < count; i++) {
        free(registry[i]);
    }
    free(registry);

    return 0;
}

/* --- Function Implementations --- */

void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

Citizen** createCitizens(int count) {
    return (Citizen**)malloc(count * sizeof(Citizen*));
}

void displayCitizen(Citizen* c) {
    printf("Citizen: %s %s, Gender: %c, DOB: %02d/%02d/%04d\n", 
            c->name, c->surname, c->gender, c->dob.day, c->dob.month, c->dob.year);
}

void calculateAgeAndCategory(Citizen* c, Date current) {
    c->ageDays = current.day - c->dob.day;
    c->ageMonths = current.month - c->dob.month;
    c->ageYears = current.year - c->dob.year;

    if (c->ageDays < 0) {
        c->ageMonths--;
        c->ageDays += 30; // Approximation for simplicity
    }
    if (c->ageMonths < 0) {
        c->ageYears--;
        c->ageMonths += 12;
    }

    // Assign category and dummy amount paid based on age
    if (c->ageYears < 18) {
        c->category = 1; // Minor
        c->amountPaid = 500.0;
    } else if (c->ageYears >= 18 && c->ageYears < 65) {
        c->category = 2; // Adult
        c->amountPaid = 1500.0;
    } else {
        c->category = 3; // Senior
        c->amountPaid = 2500.0;
    }
}

// Uses pointer swapping (O(1) memory moves) instead of copying large structs
void sortCitizensByCategory(Citizen** registry, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (registry[j]->category < registry[j + 1]->category) {
                Citizen* temp = registry[j];
                registry[j] = registry[j + 1];
                registry[j + 1] = temp;
            }
        }
    }
}

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