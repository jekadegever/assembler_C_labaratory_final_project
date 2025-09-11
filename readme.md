# 🛠️ C Assembler Project

This project is an **Assembler implementation in C**, developed as part of the course **20465 – 2023B (Open University of Israel)**.  
The project implements an assembler for a **custom assembly language**, designed for a **fictional hardware architecture** with the following characteristics:

- **7 registers** (r0–r6)
- **Word size:** 10 bits per memory word
- **Program counter:** up to 256 memory words
- **Instruction set:** 16 commands (see below)
- **Custom base-4 output format** using the letters `a–d`

---

## 🚀 Features
- **Two-pass assembler**:
    - **Pre-processor:** expands macros into a `.am` file.
    - **First pass:** builds the symbol table, counts memory, and collects unresolved symbols.
    - **Second pass:** encodes final addresses, resolves labels, and generates output files.
- **Instruction encoding:**
    - Supports addressing modes: immediate, direct, register, and matrix.
    - Handles relocatable addresses for labels.
- **Error handling:**
    - Clear error messages for undefined labels, invalid operands, and syntax violations.
- **Memory management system** for tracking code and data areas.
- **Extra functionality:** in addition to course requirements, a `.bin` file is generated showing the memory image in raw binary.

---

## 📂 Project Structure
```markdown
├── Source_Files/                 # Implementation files (.c)
│   ├── addresses.c               # Handles parsing and validation of addressing modes (immediate, direct, register, matrix)
│   ├── assembler.c               # Main entry point; orchestrates preprocessing, first pass, second pass, and output generation
│   ├── data_memory.c             # Manages data memory (DC), allocation of .data and .string directives
│   ├── directives.c              # Handles assembler directives (.data, .string, .entry, .extern)
│   ├── encoder.c                 # Encodes instructions into 10-bit machine code words
│   ├── errors.c                  # Error reporting system; prints syntax/semantic errors with line numbers
│   ├── externals.c               # Handles external symbols and manages .ext file creation
│   ├── files.c                   # File handling (open, read, write) and generation of .obj, .ent, .ext, .bin
│   ├── first_pass.c              # Implements the first pass: builds symbol table, counts IC/DC, collects unresolved labels
│   ├── instruction_memory.c      # Manages instruction memory (IC), stores encoded instructions before output
│   ├── instructions.c            # Contains opcode table (mnemonics → opcode mapping, allowed addressing modes)
│   ├── labels.c                  # Symbol table management for labels (definition, lookup, attributes: code/data/entry/extern)
│   ├── lines_map.c               # Keeps mapping between input source lines and memory addresses for debugging/error messages
│   ├── pre_processor.c           # Macro preprocessor: expands macros, generates the .am intermediate file
│   ├── second_pass.c             # Implements the second pass: resolves label addresses, finalizes encoding, writes outputs
│   ├── sys_memory.c              # Abstraction of system memory (array of 256 words, 10 bits each)
│   ├── tables.c                  # Generic table structures (used for labels, externals, entries, etc.)
│   └── util.c                    # Utility helper functions (string trimming, parsing, conversions, etc.)
│
├── Header_Files/                 # Header files (.h)
│   ├── addresses.h               # Prototypes and definitions for addresses.c
│   ├── assembler.h               # Global definitions for assembler.c
│   ├── boolean.h                 # Boolean type and constants (true/false) for C90 compatibility
│   ├── config.h                  # Project-wide constants (max line length, memory size, etc.)
│   ├── context.h                 # Global assembler context struct (IC, DC, error flags, etc.)
│   ├── data_memory.h             # Interfaces for data memory management
│   ├── directives.h              # Interfaces for handling directives
│   ├── encoder.h                 # Interfaces for instruction encoding
│   ├── errors.h                  # Error codes and error handling functions
│   ├── externals.h               # Interfaces for externals management
│   ├── files.h                   # Interfaces for file operations and output generation
│   ├── first_pass.h              # Interfaces for the first pass
│   ├── instruction_memory.h      # Interfaces for instruction memory management
│   ├── instructions.h            # Instruction table and opcode definitions
│   ├── labels.h                  # Symbol table structures and function prototypes
│   ├── lines_map.h               # Interfaces for line-to-memory mapping
│   ├── pre_processor.h           # Interfaces for the macro preprocessor
│   ├── second_pass.h             # Interfaces for the second pass
│   ├── sys_memory.h              # System memory abstraction
│   ├── tables.h                  # Generic table data structures
│   ├── typedef.h                 # Common typedefs for project-wide usage
│   └── util.h                    # Utility functions prototypes
│
├── Tests/                        # Example input/output test files
├── makefile                      # Build configuration (compiles all sources, generates assembler executable)
└── README.md                     # Project documentation
```
---

## 📜 Instruction Set

| Opcode | command  | Description (short)      |
|--------|----------|--------------------------|
| 0      | mov      | Move data                |
| 1      | cmp      | Compare                  |
| 2      | add      | Add                      |
| 3      | sub      | Subtract                 |
| 4      | not      | Bitwise NOT              |
| 5      | clr      | Clear register           |
| 6      | lea      | Load effective address   |
| 7      | inc      | Increment                |
| 8      | dec      | Decrement                |
| 9      | jmp      | Jump                     |
| 10     | bne      | Branch if not equal      |
| 11     | red      | Read input               |
| 12     | prn      | Print output             |
| 13     | jsr      | Jump to subroutine       |
| 14     | rts      | Return from subroutine   |
| 15     | stop     | Halt program             |

---

## 🏗️ Build Instructions

1. Clone the repository:
   ```bash
   git clone https://github.com/jekadegever/assembler_C_labaratory_final_project.git
    ```
   
2.  navigate to the project folder:
   ```bash
     cd ~/assembler_C_labaratory_final_project
   ```


3. Build Instructions
   ```bash
   make
   ``` 


---

## 🧑‍💻 Usage
1. Run the assembler with one or more .as source files: 
   ```bash
    ./assembler file1.as file2.as
   
   or with file directory
   
    ./assembler <file-directory>/file1.as   <file-directory>/file2.as 
   ```
---

## 📄 Output Files (for linkage and load proccesses)
### •	file.am – After preprocessing (macro expansion). Input to the assembler.

### •	file.obj – Object file with machine code in custom base-4 (a–d) format.

### •	file.ent – Entry table: lists labels defined as entry, with their resolved memory addresses.

### •	file.ext – Externals: lists all uses of labels defined in external files, with the addresses where they appear.

### •	file.bin – (Extra, beyond course requirements) raw memory image, parallel to .obj.

#### ⚠️ .ent and .ext are only generated if relevant (i.e., only if entry/extern symbols are present).

---

# 📝 Example

Input file valid1.as:
```bash
;======= file: valid1.as =======

;entry/ external declaration

    .entry LOOP
    .entry LENGTH
    .extern L3
    .extern W

; ----- macros ------
    mcro MC_1
    inc r1
    inc r2
    inc r3
    inc r4
    lea MAIN
    mcroend
    
    mcro MC_2
    mov r2,r6
    jmp r1
    mcroend
    
;----- main ------    
    MAIN: mov M1[r2][r7],W
    add r2,STR
    
    MC_1
    
    LOOP: jmp W
    prn #-5
    sub r1, r4
    inc K
    
    MC_2
    MC_1
    
    mov M1[r3][r3],r3
    bne L3
    END: stop

; ----- data declaration -----
    STR: .string "abcdef"
    LENGTH: .data 6,-9,15
    K: .data 22
    M1: .mat [2][2] 1,2,3,4

```
 valid1.as obj file (valid1.obj):
```bash
		bbc 	dd  		
		bcba	aacba		
		bcbb	cabbc		
		bcbc	acbda		
		bcbd	aaaab		
		bcca	acdba		
		bccb	acaaa		
		bccc	bdccc		
		bccd	cbaba		
		bcda	aaaab		
		bcdb	dbaaa		
		bcdc	ddcda		
		bcdd	addda		
		bdaa	abbaa		
		bdab	bdaba		
		bdac	cabac		
		bdad	aacda		
		bdba	cabbc		
		bdbb	adada		
		bdbc	aaada		
		bdbd	ccaba		
		bdca	aaaab		
		bdcb	ddaaa		
		bdcc	abcab		
		bdcd	abcac		
		bdda	abcad		
		bddb	abcba		
		bddc	abcbb		
		bddd	abcbc		
		caaa	aaaaa		
		caab	aaabc		
		caac	dddbd		
		caad	aaadd		
		caba	aabbc		
		cabb	aaaab		
		cabc	aaaac		
		cabd	aaaad		
		caca	aaaba		
```
valid1.as ext file (valid1.ext):
```bash
	W	bcbd		
	W	bcda		
	L3	bdca		
```
valid1.as ext file (valid1.ent):
```bash
	LOOP	bccd		
	LENGTH	caab		
```
---
# 👤 Author

### Ivgeny Tokarzhevsky
### Third-year Computer Science / Programming student at the Open University.
### Project developed for course 20465 – 2024B.

---

# 📜 License

MIT License – feel free to use, copy, and modify.
