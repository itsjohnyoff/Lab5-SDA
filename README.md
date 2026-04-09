# SDA Lab 5 - Task 26: Dynamic Sentence Manipulation

## Overview
This project is a procedural C implementation for a Data Structures and Algorithms (SDA) lab task.
The program reads a line of text, splits it into sentences using dynamic memory, and swaps the first sentence with another sentence chosen by a 1-based index (called step in the program).

The solution focuses on:
- dynamic allocation and deallocation of memory;
- pointer-based sentence swapping for efficiency;
- file input/output using input.txt and output.txt.

## Repository Structure
- Part1.c - source code for the solution.
- input.txt - stores the raw input string.
- output.txt - stores text before and after sentence interchange.
- README.md - project documentation.

## Program Behavior
The executable performs the following steps:
1. Reads one line of text from standard input.
2. Writes that line to input.txt.
3. Reads the line back from input.txt.
4. Reads an integer step from the user.
5. Parses the text into separate sentences using ., !, and ? as delimiters.
6. Prints the reconstructed text before changes.
7. Swaps sentence 1 with sentence step (if step is valid).
8. Prints the reconstructed text after changes.
9. Writes both versions to output.txt.
10. Frees all dynamically allocated memory.

## Implementation Analysis
### Core Functions
- parseSentences(char* text, int* count)
   - Dynamically builds an array of sentence pointers.
   - Uses malloc for initial pointer storage and realloc for growth.
   - Preserves punctuation by including delimiter characters in each sentence.

- interchangeSentences(char** sentences, int count, int step)
   - Validates step in range [1, count].
   - Swaps pointers directly in O(1), avoiding string copies.

- joinSentences(char** sentences, int count, char* result)
   - Rebuilds a display/output string from sentence fragments.
   - Inserts a space between adjacent sentences.

### Complexity
- Parsing: O(n), where n is input length.
- Swap operation: O(1).
- Joining sentences for output: O(n) overall text traversal with repeated concatenation.
- Extra memory: O(n) for sentence copies and pointer array.

## Build and Run
### Requirements
- GCC (or another C compiler).
- Terminal/console environment.

### Compile
```bash
gcc Part1.c -o Part1
```

### Execute
Windows (PowerShell or CMD):
```bash
.\Part1.exe
```

Linux/macOS:
```bash
./Part1
```

## Example
Input text:
Eu sunt Ion. Sunt din satul Ustia. Invat la UTM

Step:
3

Before changes:
Eu sunt Ion. Sunt din satul Ustia. Invat la UTM

After changes:
Invat la UTM Sunt din satul Ustia. Eu sunt Ion.

## Notes and Edge Cases
- Maximum input line length is controlled by MAX_BUFFER (2048).
- Sentences are detected only by ., !, and ?.
- If text ends without punctuation, the remaining text is treated as the final sentence.
- If step is out of range, the program prints an error and keeps sentence order unchanged.

## Quality Observations and Possible Improvements
- The program correctly frees all allocated sentence memory.
- Pointer swapping is efficient and avoids unnecessary copies.
- Recommended improvements for production-level robustness:
   - check malloc/realloc return values before use;
   - guard against potential output buffer overflow when joining large sentence sets;
   - support more advanced sentence tokenization rules.

## Author
SDA Lab 5 submission.