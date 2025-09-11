מצוין 🙌
הכנתי לך גרסה אחת מלאה, ברצף, שתוכל פשוט להעתיק כ־README.md ל־GitHub שלך:

# 🛠️ C Assembler Project

This project is an **Assembler implementation in C**, developed as part of a university programming course.  
It takes assembly source files (`.as`) as input, performs **preprocessing, first pass, and second pass**, and generates the required output files (`.ob`, `.ext`, `.ent`).

---

## 🚀 Features
- **Pre-Processor**  
  Handles macros and expands them before the first pass.
- **First Pass**
    - Scans the source code.
    - Builds the **symbol table** for labels.
    - Keeps track of instruction counter (IC) and data counter (DC).
    - Collects fix-up information for unresolved symbols.
- **Second Pass**
    - Resolves labels and symbols from the first pass.
    - Encodes the final machine code.
    - Generates external and entry tables.
- **Instruction Encoding**
    - Supports A, B, and C type records.
    - Handles direct, immediate, register, and matrix addressing.
- **Error Handling**
    - Syntax validation.
    - Undefined labels detection.
    - Invalid operand checks.
    - Clear error messages.

---

## 📂 Project Structure

.
├── SourceFiles/         # All .c and .h source files
│   ├── assembler.c      # Main entry point
│   ├── preproc.c        # Preprocessor
│   ├── first_pass.c     # First pass logic
│   ├── second_pass.c    # Second pass logic
│   ├── code_conversion.c# Encoding functions
│   ├── data_strct.c     # Data structures
│   ├── table.c          # Symbol tables
│   ├── util.c           # Utility functions
│   ├── Errors.c         # Error handling
│   ├── handle_text.c    # Text parsing
│   ├── lexer.c          # Tokenizer / Lexer
│   └── globals.h        # Global definitions
├── makefile             # Build configuration
├── README.md            # Project documentation
└── Tests/               # Example input/output test files

---

## 🏗️ Build Instructions

1. Clone the repository:
   ```bash
   git clone https://github.com/<your-username>/<repo-name>.git
   cd <repo-name>

	2.	Compile using make:

make


	3.	Clean build artifacts:

make clean



⸻

🧑‍💻 Usage

Run the assembler with one or more .as source files:

./assembler file1.as file2.as

For each source file, the assembler will generate:
•	file.ob → Object file (encoded machine code).
•	file.ext → External symbols (if used).
•	file.ent → Entry points (if defined).

⸻

📝 Example

Input (hello.as):

MAIN:    mov  r3,  LIST[2]
add  r2,  r3
prn  #-5
stop
LIST:    .data 6, -9
.string "hello"

Output (hello.ob):

010010 000011
...


⸻

⚙️ Technologies
•	Language: ANSI C (C90 standard, portable)
•	Build System: Makefile
•	Platform: Tested on macOS & Linux

⸻

📖 Learning Goals
•	Working with multiple passes in an assembler.
•	Handling symbol tables and fix-up lists.
•	Memory management in C (malloc/free).
•	Bit-level instruction encoding.
•	Writing production-style C code with error handling.

⸻

👤 Author

Ivgeny Tokarzhevsky
Third-year Computer Science / Programming student.
Project developed as part of a university lab assignment in C.

⸻

📜 License

MIT License – feel free to use, copy, and modify.

---

רוצה שאכין גם גרסה בעברית (README.he.md) שתסביר הכל לקוראים מקומיים, או שאתה מעדיף להשאיר באנגלית בלבד?