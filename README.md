# SDA Lab 5 - Task 26: Pointers, UDTs, and File I/O

## Overview
This repository contains two procedural C programs for SDA Laboratory 5.
Both parts emphasize dynamic memory management, pointer-based processing, and file stream operations.

- **Part 1** focuses on dynamic sentence parsing and pointer swapping.
- **Part 2** focuses on nested User-Defined Types (UDTs), dynamic arrays of struct pointers, age/category computation, sorting, and file merging.

## Repository Structure
- `Part1.c` - Dynamic sentence manipulation.
- `Part2.c` - Citizen registry and processing system.
- `README.md` - Project documentation.
- `.gitignore` - Ignores compiled artifacts and runtime text outputs.

Generated at runtime (currently ignored by Git):
- `input.txt`
- `output.txt`
- `experiment.txt`
- `result.txt`

## Part 1: Dynamic Sentence Manipulation (`Part1.c`)

### What It Does
1. Reads one text line from standard input.
2. Saves it to `input.txt`.
3. Reads it back from `input.txt`.
4. Splits text into sentences using `.`, `!`, `?` delimiters.
5. Swaps sentence 1 with sentence `step` (1-based index).
6. Prints text before and after changes.
7. Saves both versions to `output.txt`.

### Technical Highlights
- Dynamic sentence storage with `char**`.
- Uses `malloc`, `realloc`, and `free`.
- O(1) pointer swap (no full-string copying during interchange).

### Core Functions
- `parseSentences(char* text, int* count)`
- `interchangeSentences(char** sentences, int count, int step)`
- `joinSentences(char** sentences, int count, char* result)`

### Complexity
- Parsing: **O(n)**
- Sentence swap: **O(1)**
- Rebuild output string: **O(n)**
- Additional memory: **O(n)**

### Notes
- `MAX_BUFFER` is 2048 characters.
- If `step` is out of range, swap is skipped and an error is shown.
- Trailing text without punctuation is treated as one sentence.

## Part 2: Citizen Registry System (`Part2.c`)

### What It Does
1. Reads citizen records from keyboard.
2. Stores raw entries into `experiment.txt`.
3. Calculates age breakdown (years, months, days).
4. Assigns category and payment amount by age.
5. Sorts citizens by category in descending order.
6. Saves processed data to `output.txt`.
7. Merges `experiment.txt` + `output.txt` into `result.txt`.

### UDT Design
- `Date` (day, month, year)
- `Address` (city, street, postCode)
- `Citizen` (identity fields + nested UDTs + computed fields)

### Pointer and Memory Model
- Registry is allocated as `Citizen**`.
- Each citizen record is allocated independently with `malloc`.
- Sorting swaps pointers, not entire structs.
- All allocated memory is released at the end.

### Processing Rules
- Category 1: age < 18, amount = 500.0
- Category 2: 18 <= age < 65, amount = 1500.0
- Category 3: age >= 65, amount = 2500.0

### Complexity
- Input and calculations: **O(n)**
- Bubble sort by category: **O(n^2)**
- Merge two files: **O(total file size)**
- Additional memory: **O(n)** for pointer registry and citizens

### Notes
- Age day/month adjustment uses a simplified 30-day month approximation.
- All nested UDT fields (City, Street, Postal Code for both Home and Work addresses) are fully populated during the input flow and successfully saved to the experiment.txt file.

## Build and Run

### Requirements
- GCC (recommended) or another C compiler.

### Compile
```bash
gcc Part1.c -o Part1
gcc Part2.c -o Part2
```

### Run
Windows:
```bash
.\Part1.exe
.\Part2.exe
```

Linux/macOS:
```bash
./Part1
./Part2
```

## Verified Status
Both programs compile successfully in the current workspace with GCC and produce their expected output files.
