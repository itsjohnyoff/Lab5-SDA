# SDA Lab 5 — File I/O & Citizen Registry with UDTs

**Course:** Data Structures and Algorithms (SDA)  
**Lab No.:** 5 — Solving problems using FILE data type & UDT (user-defined data type)  
**Language:** C (procedural style)

---

## Table of Contents

- [Overview](#overview)
- [Problem Statement](#problem-statement)
- [Custom Data Type (CDT)](#custom-data-type-cdt)
- [Project Structure](#project-structure)
- [How It Works](#how-it-works)
  - [Part 1 — Sentence Manipulation](#part-1--sentence-manipulation)
  - [Part 2 — Citizen Registry](#part-2--citizen-registry)
- [Building & Running](#building--running)
  - [Prerequisites](#prerequisites)
  - [Compile](#compile)
  - [Run](#run)
- [Menu Reference](#menu-reference)
- [File I/O Formats](#file-io-formats)
- [Key Design Decisions](#key-design-decisions)

---

## Overview

This repository contains two standalone procedural C programs for SDA Laboratory 5. Each program focuses on a different combination of **FILE stream operations**, **dynamic memory management**, and **pointer-based data processing**.

| Program | Focus | Source |
|---------|-------|--------|
| **Part 1** | Dynamic sentence parsing, pointer-level swap, file read/write | `Part1.c` |
| **Part 2** | Citizen registry with nested UDTs, age computation, sorting, and file merging | `Part2.c` |

Both programs produce and consume text files. Part 2 additionally supports loading citizen records directly from a pre-formatted file, skipping manual keyboard entry.

---

## Problem Statement

**Task 5.1 — FILE data type**

Develop a C program that:

- Creates `input.txt` and writes a keyboard-entered string into it.
- Reads the string back from `input.txt` and splits it into sentences.
- Interchanges sentence 1 with sentence at a user-chosen swap position.
- Displays the text **before** and **after** the interchange.
- Writes both versions to `output.txt`.

**Task 5.2 — FILE data type & UDT**

Develop a C program that:

- Defines a `Citizen` struct with nested `Date` and `Address` UDTs.
- Accepts citizen records from keyboard input or loads them from a file.
- Saves raw records to `experiment.txt` and displays them on screen.
- Computes each citizen's age (years, months, days) from a current date entered at runtime.
- Assigns a category and payment amount based on age.
- Sorts citizens by category in descending order (bubble sort on pointers).
- Writes the processed results to `output.txt`.
- Merges `experiment.txt` and `output.txt` into `result.txt`.
- All generated files can be reopened in read mode.

---

## Custom Data Type (CDT)

Part 2 is built around the `Citizen` struct, composed of two nested UDTs:

```
Citizen
├── name[50]          (char array)
├── surname[50]       (char array)
├── dob               (Date struct: day, month, year)
├── gender            (char: 'm' or 'f')
├── home              (Address struct: city, street, postCode)
├── work              (Address struct: city, street, postCode)
├── ageYears          (int — computed from DOB vs. current date)
├── ageMonths         (int — computed from DOB vs. current date)
├── ageDays           (int — computed from DOB vs. current date)
├── category          (int: 1 / 2 / 3)
└── amountPaid        (float — derived from category)
```

Derived fields are computed automatically after the current date is entered:

| Category | Condition | Amount Paid |
|:--------:|-----------|------------:|
| 1 | age < 18 | 500.00 |
| 2 | 18 ≤ age < 65 | 1500.00 |
| 3 | age ≥ 65 | 2500.00 |

The citizen registry is stored as a `Citizen**` — a dynamically allocated array of individual `malloc`'d citizen pointers. Sorting swaps pointers only, never copying struct data.

---

## Project Structure

```
Lab_5/
├── Part1.c            # Task 5.1 — Sentence manipulation via FILE I/O
├── Part2.c            # Task 5.2 — Citizen registry with UDTs & file processing
├── citizens.txt       # Pre-formatted data file for Part 2 load-from-file mode
├── .gitignore         # Excludes binaries, PDFs, and runtime-generated text files
└── README.md          # This file
```

Generated at runtime (git-ignored):

| File | Created By | Purpose |
|------|-----------|---------|
| `input.txt` | Part 1 | Stores the user-entered string |
| `output.txt` | Part 1 / Part 2 | Before/after sentences (Part 1) or sorted citizen data (Part 2) |
| `experiment.txt` | Part 2 | Raw citizen records as entered |
| `result.txt` | Part 2 | Merged content of `experiment.txt` + `output.txt` |

---

## How It Works

### Part 1 — Sentence Manipulation

The program reads one line of text from the keyboard, saves it to `input.txt`, then reads it back. It parses the text into individual sentences using `.`, `!`, `?` as delimiters (including their fullwidth/Chinese UTF-8 equivalents `。`, `？`, `！`). It then swaps sentence 1 with the sentence at a user-chosen position and writes both versions to `output.txt`.

**Core functions:**

| # | Function | Description |
|---|----------|-------------|
| 1 | `sentenceEndLen(char* current)` | Returns the byte-length of a sentence-ending punctuation mark at the current position (1 for ASCII, 3 for UTF-8 fullwidth), or 0 if none found |
| 2 | `parseSentences(char* text, int* count)` | Splits text into a dynamically allocated `char**` array of sentence strings; grows with `realloc` when capacity is exceeded |
| 3 | `interchangeSentences(char** sentences, int count, int step)` | Swaps `sentences[0]` with `sentences[step - 1]` via an O(1) pointer exchange; prints an error if `step` is out of range |
| 4 | `joinSentences(char** sentences, int count, char* result)` | Concatenates all sentence strings back into a single buffer, separated by a space |

**Complexity:**

| Operation | Time | Space |
|-----------|------|-------|
| Parse sentences | O(n) | O(n) |
| Swap sentences | O(1) | O(1) |
| Rebuild string | O(n) | O(n) |

*Where n = length of the input string.*

**Sample session:**

```
Introduce the string:
> Hello world! This is a test. How are you? I am fine.

Input string is:
Hello world! This is a test. How are you? I am fine.

Introduce your step of swaps: 3

Before changes:
Hello world! This is a test. How are you? I am fine.

After changes:
How are you? This is a test. Hello world! I am fine.

Results saved to output.txt
```

---

### Part 2 — Citizen Registry

The program offers two input modes on startup: **manual keyboard entry** or **load from file** (e.g. `citizens.txt`). After input, it writes raw records to `experiment.txt`, displays them on screen, reads the current date, computes ages and categories, sorts the registry, and writes results to `output.txt`. Finally it merges both files into `result.txt`.

**Core functions:**

| # | Function | Description |
|---|----------|-------------|
| 1 | `clearBuffer()` | Flushes remaining characters from `stdin` after `scanf` |
| 2 | `isValidDate(int d, int m, int y)` | Validates day/month/year ranges; loops until a valid date is provided |
| 3 | `isValidGender(char g)` | Ensures gender is `'m'` or `'f'` |
| 4 | `createCitizens(int count)` | Allocates an array of `count` citizen pointers |
| 5 | `inputCitizensManually(Citizen** reg, int n)` | Interactive loop: reads name, surname, gender, DOB, home & work address for each citizen with validation |
| 6 | `loadCitizensFromFile(const char* fn, int* count)` | Parses a structured text file (count on first line, then fields line-by-line) into the registry; includes per-citizen error handling and full cleanup on failure |
| 7 | `displayCitizen(Citizen* c)` | Prints one citizen record in a human-readable format |
| 8 | `calculateAgeAndCategory(Citizen* c, Date current)` | Computes age breakdown and assigns category + payment; handles negative month/day adjustments |
| 9 | `sortCitizensByCategory(Citizen** reg, int n)` | Bubble sort in descending category order; swaps pointers only |
| 10 | `mergeFiles(const char* f1, const char* f2, const char* fOut)` | Concatenates two text files into a third with labeled section headers |

**Complexity:**

| Operation | Time | Space |
|-----------|------|-------|
| Input / file load | O(n) | O(n) |
| Age calculation | O(n) | O(1) per citizen |
| Bubble sort | O(n²) | O(1) — pointer swaps only |
| File merge | O(total file size) | O(1) — buffered line-by-line |

**Data flow:**

```
Keyboard input          experiment.txt
or citizens.txt   ───►  (raw records)  ──────────────────────────┐
                                                                  │
                         output.txt                              │
                  ───►  (sorted + processed)  ──────────────────►│
                                                                  ▼
                                                           result.txt
                                                           (merged)
```

---

## Building & Running

### Prerequisites

- GCC (MinGW on Windows) or any C99-compatible compiler.
- Windows, Linux, or macOS.

### Compile

```bash
gcc Part1.c -o Part1
gcc Part2.c -o Part2
```

### Run

**Windows:**
```bash
.\Part1.exe
.\Part2.exe
```

**Linux / macOS:**
```bash
./Part1
./Part2
```

---

## Menu Reference

**Part 2** starts with an input mode selection:

```
--- Citizen Registry System ---

1. Manual input
2. Load from file
Choice:
```

Selecting `1` prompts for the number of citizens and enters the manual entry loop. Selecting `2` prompts for a filename (e.g. `citizens.txt`) and loads records from it.

After input, the program runs all processing steps automatically (display → age calculation → sort → save → merge) without further menu interaction.

---

## File I/O Formats

| Format | Part 1 | Part 2 |
|--------|--------|--------|
| `input.txt` | Raw string as typed (binary mode, UTF-8 safe) | — |
| `output.txt` | "Before changes" and "After changes" text blocks | Sorted citizen records, one per line |
| `experiment.txt` | — | Raw records: name, gender, DOB, home & work addresses |
| `result.txt` | — | `experiment.txt` content followed by `output.txt` content, with section headers |

**`citizens.txt` format for load mode (Part 2):**

```
<count>
<name>
<surname>
<gender>
<day> <month> <year>
<home city>
<home street>
<home postal code>
<work city>
<work street>
<work postal code>
... (repeat for each citizen)
```

All generated files are plain text and can be opened in any text editor.

---

## Key Design Decisions

**Binary file mode for strings (Part 1)** — `input.txt` and `output.txt` in Part 1 are opened with `"wb"` / `"rb"` instead of text mode. This prevents the Windows C runtime from translating `\n` to `\r\n` and from mangling multi-byte UTF-8 sequences, ensuring the stored bytes exactly match what was typed.

**UTF-8 awareness in sentence parsing (Part 1)** — `sentenceEndLen()` inspects raw byte values to detect 3-byte Chinese and fullwidth punctuation (`。` = `E3 80 82`, `？` = `EF BC 9F`, `！` = `EF BC 81`). This lets the program correctly parse multilingual text without any locale dependency.

**O(1) pointer swap (Part 1)** — `interchangeSentences` exchanges two `char*` pointers rather than copying sentence content. The sentence strings themselves are never moved in memory.

**Citizen\*\* pointer registry (Part 2)** — The registry is a dynamically allocated array of pointers. Each citizen is individually `malloc`'d. Sorting swaps pointer values, not entire `Citizen` structs, which is efficient for large records and demonstrates the intended pointer-based design.

**Validation loops (Part 2)** — Date and gender inputs use `while (!valid)` retry loops so the user is never left with an invalid record silently stored in the registry.

**Robust file loading (Part 2)** — `loadCitizensFromFile` tracks every allocated citizen pointer. If any field fails to parse, all previously allocated memory is freed before returning `NULL`, preventing memory leaks on partial-load failures.

**30-day month approximation (Part 2)** — When adjusting negative day differences during age calculation, the previous month is approximated as 30 days. This matches the simplified model common in academic registry problems.
