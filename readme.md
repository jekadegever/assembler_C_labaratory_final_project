# 🛠️ C Assembler Project

This project is an **Assembler implementation in C**, developed as part of the course **20465 – 2023B (Open University of Israel)**.  
The project implements an assembler for a **custom assembly language**, designed for a **fictional hardware architecture**.

## 🎯 Project Goals

The main goals of this project are:

- **Learning to design and implement a large-scale software system**  
  Developing a working **assembler**, which is a fundamental and widely used system in computer science.

- **Practicing programming in pure C**  
  Writing the entire project in **ANSI C90 standard**, without advanced library functions,  
  in order to gain deeper understanding of low-level programming.

- **Memory management**  
  Using `malloc`, `free`, and pointer manipulation responsibly, ensuring correct allocation, deallocation, and error handling.

- **Modular design**  
  Splitting the project into multiple source and header files, each with a clear responsibility  
  (preprocessor, passes, encoding, memory management, tables, etc.).

- **Error handling and robustness**  
  Building a reliable system that detects and reports syntax and semantic errors in assembly code.

- **Understanding linkage and loading concepts**  
  Generating intermediate files (`.obj`, `.ent`, `.ext`) that are required for the later linkage and loading phases.

This project simulates the real-world development of a **compiler/assembler component**,  
while teaching both **theoretical concepts** and **practical C programming skills**.

---
## Assembler
###	⚙️ Hardware characteristics:
- **8 registers** (r0–r7)
- **Word size:** 10 bits per memory word
- **Program counter:** up to 256 memory words
- **Initial address:** 100
- **Available memory: 156 words**
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





# 🧑‍💻 Usage

The assembler processes source files (`.as`) written in the custom assembly language and generates machine code output files (`.obj`, `.ent`, `.ext`, `.bin`).


## 📜 Instruction Set
The assembler supports 16 opcodes. Each opcode is identified by its mnemonic:


| Opcode | Mnemonic | Full Name                | Operands             | Example           |
|--------|----------|--------------------------|----------------------|------------------|
| 0      | mov      | Move                     | source, destination  | `mov r3, r1`     |
| 1      | cmp      | Compare                  | source, destination  | `cmp r2, LIST`   |
| 2      | add      | Add                      | source, destination  | `add #5, r6`     |
| 3      | sub      | Subtract                 | source, destination  | `sub r1, r0`     |
| 4      | not      | Bitwise NOT              | destination          | `not r2`         |
| 5      | clr      | Clear                    | destination          | `clr LIST`       |
| 6      | lea      | Load Effective Address   | source, destination  | `lea LABEL, r4`  |
| 7      | inc      | Increment                | destination          | `inc r5`         |
| 8      | dec      | Decrement                | destination          | `dec r2`         |
| 9      | jmp      | Jump                     | destination(label)   | `jmp END`        |
| 10     | bne      | Branch if Not Equal      | destination(label)   | `bne LOOP`       |
| 11     | red      | Read                     | destination          | `red r1`         |
| 12     | prn      | Print                    | src                  | `prn #10`        |
| 13     | jsr      | Jump to Subroutine       | destination(label)   | `jsr FUNC`       |
| 14     | rts      | Return from Subroutine   | —                    | `rts`            |
| 15     | stop     | Stop Execution           | —                    | `stop`           |


### 📌 Notes on Operands

- ### 🖥️ Registers

- Registers are named with the letter **`r`** followed by their number.
- Valid registers are: `r0`–`r7` (total of 8 registers).
- Example usage:
```asm
 ; copy the value from register r3 into register r5
  mov r3, r5  
```
- ### 🔢 Constants
- To use a **constant number** as an operand in an instruction, you must prefix it with `#`.  
  Example:
```asm
  ; move the constant value 5 into register r3
  mov #5, r3
```
- ### 🎛️ Matrix access
- To access **matrix value**, you must provided the matrix `label name` and indexes with register numbers in next format `[r1][r2]`
```asm
;move the values at given matrix index into register r6
mov MAT1[r1][r3], r6
```

### 🏷️ Labels
- Labels are symbolic names that represent memory addresses.
- They must begin with a letter and can contain letters and digits.
- A label is defined by writing it at the beginning of a line, followed by a colon `:`.
- Example:
  ```asm
  LOOP:   cmp r1, r2
          bne LOOP
  END:    stop

## 🛠️ Directives

- Directives allow defining data, matrix, strings, and symbol attributes.
- **Available directives:** `.mat` `.data` `.string` `.entry` `.extern`


 `.data` – define numeric constants.
	
  ```asm
  NUMS: .data 5, -2, 13
  ```

 `.string` – define a null-terminated string.
  ```asm
 MSG: .string "hello"
  ```
 `.mat`  – define a numeric matrix of the given size. 
 If fewer numbers are provided than the matrix size, the assembler automatically fills the remaining entries with `0`.
    
  ```asm
   .mat [3][3] 5,6,7,1,2
   ```
This defines a 3×3 matrix:
```asm
      5 6 7
      1 2 0
      0 0 0
 ```
 `.entry` – mark a label as entry (exported to .ent file with defined address) - to be able using this label in another file.
```asm
  .entry MSG
```

`.extern` – declare a label defined in another file (appears in .ext file).
```asm
.extern EXT_LABEL
```




## 🔁 Macros
•	Macros are defined in the preprocessor stage.
•	They allow grouping a set of instructions under a name.
•	Syntax:
```asm
mcro PRINT_HELLO
prn #72
prn #101
prn #108
prn #108
prn #111
mcroend
```

Usage:
```asm
PRINT_HELLO
stop
```


The preprocessor will expand this into the sequence of prn instructions.
the .am file is the file with expanded macros. the program process and works with the (.am) generated file


## 🗒️ Comments

- Comments are written using a semicolon (`;`).
- A comment must appear **at the beginning of a line**.
- Once a semicolon is encountered, the entire line is ignored by the assembler.
- Comments cannot appear inline after instructions or directives — they must be on a separate line.

**Examples:**
```asm
; This is a valid comment line
MAIN:   mov r1, r2

; Another comment
        add r3, r4
```      
#### ❌ Invalid usage (not supported):   
```asm
mov r1, r2   ; comment on the same line (not allowed)
```





## 📄 Output Files (for linkage and load proccesses)
•	file.am – After preprocessing (macro expansion). Input to the assembler.

•	file.obj – Object file with machine code in custom base-4 (a–d) format.

•	file.ent – Entry table: lists labels defined as entry, with their resolved memory addresses.

 •	file.ext – Externals: lists all uses of labels defined in external files, with the addresses where they appear.

 •	file.bin – (Extra, beyond course requirements) raw memory image, parallel to .obj.

 ⚠️ .ent and .ext are only generated if relevant (i.e., only if entry/extern symbols are present).

---

# 📝 Example program

### Input file: valid1.as

```asm
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
### 🖥️ Program UI
```bash

================ Assembler started ================




- - - Running assembler on file: <valid1.as> - - -

Preprocessing stage completed.

First pass completed.

Second pass completed.

Output files generated: valid1.obj, valid1.ext, valid1.bin, valid1.ent

File <valid1.as> assembled successfully.




================ Assembler finished ================

Summary: 1 out of 1 files assembled successfully.

	
```
### 📦 valid1 object file (`valid1.obj`):
Contains the program’s machine code encoded in **base-4 representation**.  
Each 10-bit memory word is translated into base-4 digits and written using the letters `a–d`  
(where `a = 0`, `b = 1`, `c = 2`, `d = 3`).  
This file is the main output of the assembler and is required for the later **linkage and loading stages**.
```bash

	        daa   	dd  		
		bcba	aacba		
		bcbb	cbddc		
		bcbc	acbda		
		bcbd	aaaab		
		bcca	acdba		
		bccb	acaaa		
		bccc	cbbac		
		bccd	bdada		
		bcda	aaaba		
		bcdb	bdada		
		bcdc	aaaca		
		bcdd	bdada		
		bdaa	aaada		
		bdab	bdada		
		bdac	aabaa		
		bdad	babda		
		bdba	bcbac		
		bdbb	aaaca		
		bdbc	cbaba		
		bdbd	aaaab		
		bdca	dbaaa		
		bdcb	ddcda		
		bdcc	addda		
		bdcd	abbaa		
		bdda	bdaba		
		bddb	cbdcc		
		bddc	aadda		
		bddd	acbca		
		caaa	cbada		
		caab	aaaba		
		caac	bdada		
		caad	aaaba		
		caba	bdada		
		cabb	aaaca		
		cabc	bdada		
		cabd	aaada		
		caca	bdada		
		cacb	aabaa		
		cacc	babda		
		cacd	bcbac		
		cada	aaaca		
		cadb	aacda		
		cadc	cbddc		
		cadd	adada		
		cbaa	aaada		
		cbab	ccaba		
		cbac	aaaab		
		cbad	ddaaa		
		cbba	abcab		
		cbbb	abcac		
		cbbc	abcad		
		cbbd	abcba		
		cbca	abcbb		
		cbcb	abcbc		
		cbcc	aaaaa		
		cbcd	aaabc		
		cbda	dddbd		
		cbdb	aaadd		
		cbdc	aabbc		
		cbdd	aaaab		
		ccaa	aaaac		
		ccab	aaaad		
		ccac	aaaba		
```
### 💾 valid1 binary file (valid1.bin):

In addition to the required output files (`.obj`, `.ent`, `.ext`, `.am`),  
this assembler also generates a **`.bin` file** for each source program.

- The `.bin` file contains the **same machine code as the `.obj` file**,  
  but represented in **base 2 (binary)** instead of base 4 (a–d).
- This feature is **not required in the course specification** but was added for convenience.
- It allows directly viewing the raw 10-bit words of the program, one after another,  
  in a more standard binary representation.

```bash


		  address	  data	
		0000110000	0000001111		
		0001100100	0000100100		
		0001100101	1001111110		
		0001100110	0010011100		
		0001100111	0000000001		
		0001101000	0010110100		
		0001101001	0010000000		
		0001101010	1001010010		
		0001101011	0111001100		
		0001101100	0000000100		
		0001101101	0111001100		
		0001101110	0000001000		
		0001101111	0111001100		
		0001110000	0000001100		
		0001110001	0111001100		
		0001110010	0000010000		
		0001110011	0100011100		
		0001110100	0110010010		
		0001110101	0000001000		
		0001110110	1001000100		
		0001110111	0000000001		
		0001111000	1101000000		
		0001111001	1111101100		
		0001111010	0011111100		
		0001111011	0001010000		
		0001111100	0111000100		
		0001111101	1001111010		
		0001111110	0000111100		
		0001111111	0010011000		
		0010000000	1001001100		
		0010000001	0000000100		
		0010000010	0111001100		
		0010000011	0000000100		
		0010000100	0111001100		
		0010000101	0000001000		
		0010000110	0111001100		
		0010000111	0000001100		
		0010001000	0111001100		
		0010001001	0000010000		
		0010001010	0100011100		
		0010001011	0110010010		
		0010001100	0000001000		
		0010001101	0000101100		
		0010001110	1001111110		
		0010001111	0011001100		
		0010010000	0000001100		
		0010010001	1010000100		
		0010010010	0000000001		
		0010010011	1111000000		
		0010010100	0001100001		
		0010010101	0001100010		
		0010010110	0001100011		
		0010010111	0001100100		
		0010011000	0001100101		
		0010011001	0001100110		
		0010011010	0000000000		
		0010011011	0000000110		
		0010011100	1111110111		
		0010011101	0000001111		
		0010011110	0000010110		
		0010011111	0000000001		
		0010100000	0000000010		
		0010100001	0000000011		
		0010100010	0000000100		
		
```
### 🌍 valid1 External's file (valid1.ext)

When a source file uses **external labels** (declared with `.extern` directive),  
the assembler generates an `.ext` file that lists **all the memory addresses where these labels are referenced**.

#### This file is required during the linkage process in order to resolve and update the final addresses of the external labels.

- The file is created **only if the program uses externals**.
- Each line in the file shows:
    - The **label name** (the external symbol).
    - The **address in memory** where the symbol was used.

```bash
   	W	bcbd		
	W	bdbd		
	L3	cbac			
```

### 🔑 valid1 entry file (`valid1.ent`):  
Contains all labels that were declared with the `.entry` directive, along with their resolved memory addresses.  
This file is important because it exposes selected symbols as **entry points**,  
allowing them to be referenced as external symbols in other source files during the linkage process.

```bash
   	LOOP	bdbc		
	LENGTH	cbcd	
```
---

### invalid Example ❌:
#### Invalid Source File and Error Handling

File invalid2.as , invalid5.as: (see file content in provided test folder)
```

================ Assembler started ================




- - - Running assembler on file: <invalid5.as> - - -

Preprocessing stage completed.

First pass completed.


invalid5.as::21: ERROR: Entry directive error: can't define the label as entry, label doesn't exist. 


invalid5.as::26: ERROR: Entry directive error: can't define external label as entry. 


invalid5.as::29: ERROR: Entry directive error: label not found. 


invalid5.as::32: ERROR: Entry directive error: unexpected token after label name. 


invalid5.as::16: ERROR: Attempted to use an undeclared label. 

Second pass failed.



File <invalid5.as> assembly failed.





- - - Running assembler on file: <invalid2.as> - - -

Preprocessing stage completed.


invalid2.as::10: ERROR: Unknown type of operation. 


invalid2.as::13: ERROR: Matrix column index is not a register number. 


invalid2.as::13: ERROR: Matrix row index is not a register number. 


invalid2.as::14: ERROR: Matrix column index is not a register number. 


invalid2.as::14: ERROR: Matrix row index is not a register number. 


invalid2.as::17: ERROR: floating point are not allowed at immediate operand 


invalid2.as::24: ERROR: Data declaration error: matrix size definition is missing, format: [rows][cols]. 


invalid2.as::27: ERROR: Data declaration error: no numeric value provided. 


invalid2.as::30: ERROR: The provided number is greater than the maximum storable value in memory. 


invalid2.as::31: ERROR: The provided number is greater than the maximum storable value in memory. 


invalid2.as::34: ERROR: Data declaration error: invalid matrix size definition, expected format [rows][cols]. 


invalid2.as::36: ERROR: Data declaration error: invalid matrix size definition, expected format [rows][cols]. 


invalid2.as::39: ERROR: Out of memory: program reached to the max available memory. 


invalid2.as::43: ERROR: Data declaration error: too many values provided for the declared matrix dimensions. 


invalid2.as::46: ERROR: Data declaration error: characters are not allowed, only numeric values. 

First pass failed.



File <invalid2.as> assembly failed.




================ Assembler finished ================

Summary: 0 out of 2 files assembled successfully.


````
## 🏗️ Build Instructions

1. Clone the repository:
   ```bash
   git clone https://github.com/jekadegever/assembler_C_labaratory_final_project.git
    ```

2.  navigate to the project folder:
   ```bash
     cd ~/assembler_C_labaratory_final_project
   ```


3. compile with the provided makefile:
   ```bash
   make
   ``` 


---

4. Run the assembler with one or more .as source files:
   ```bash
    ./assembler file1.as file2.as
   
   or with file directory
   
    ./assembler <file-directory>/file1.as   <file-directory>/file2.as 
   ```

---
# 👤 Author

### Ivgeny Tokarzhevsky
### Third-year Computer Science / Programming student at the Open University.
### Project developed for course 20465 – 2024B.

---

# 📜 License

MIT License – feel free to use, copy, and modify.
