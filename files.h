

#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include "boolean.h"
#include "context.h"



/**
 * @brief File access mode used by open_file().
 *
 * Maps to fopen() modes:
 *  - READ       → "r"
 *  - WRITE      → "w"
 *  - READ_WRITE → "r+"
 */
typedef enum {
    READ,
    WRITE,
    READ_WRITE
} file_mode;

/**
 * @brief Logical file types produced/consumed by the assembler.
 *
 * Used by change_file_extension() to pick the proper suffix:
 *  - OBJECT_FILE   → ".obj"
 *  - EXTERNAL_FILE → ".ext"
 *  - ENTRY_FILE    → ".ent"
 *  - AM_FILE       → ".am"
 *  - NO_EXTENSION  → "" (no suffix)
 */
typedef enum {
    OBJECT_FILE,
    EXTERNAL_FILE,
    ENTRY_FILE,
    AM_FILE,
    NO_EXTENSION
} file_type;



/**
 * @brief Create (or truncate) a file and write optional content.
 *
 * Opens @p file_name in write mode and writes @p content (or an empty string if NULL).
 *
 * @param file_name  File name to create.
 * @param content    Null-terminated string to write (may be NULL for empty file).
 * @param asmContext Assembler context for error reporting.
 * @return true on success, false on failure (also prints an error).
 *
 * @note On failure, errno is consulted and mapped to user-facing errors.
 */
boolean create_file(const char* file_name, char* content, const assembler_context *asmContext);

/**
 * @brief Open a file with the requested access mode.
 *
 * Wraps fopen() and printing external errors on failure.
 *
 * @param file_name  Path to open.
 * @param file_mode  Access mode (READ/WRITE/READ_WRITE).
 * @param asmContext Assembler context for error reporting.
 * @return FILE* on success, NULL on failure (error already printed).
 */
FILE* open_file(const char* file_name, file_mode file_mode, const assembler_context *asmContext);

/**
 * @brief Generate the .ent file with all entry labels and their addresses.
 *
 * Iterates the labels list in @p asmContext and emits name/address pairs in base-4 format.
 *
 * @param asmContext Assembler context (provides labels list and file names).
 * @return true on success, false on allocation/open/write errors (error printed).
 */
boolean create_ent_file(assembler_context *asmContext);

/**
 * @brief Generate the .ext file with usage addresses of external labels.
 *
 * Iterates the externals list in @p asmContext and emits label/usage-address pairs in base-4.
 *
 * @param asmContext Assembler context (provides external usages and file names).
 * @return true on success, false on allocation/open/write errors (error printed).
 */
boolean create_ext_file(assembler_context *asmContext);

/**
 * @brief Generate the .obj file (object image) from instruction and data memories.
 *
 * Writes IC/DC header (in base-4), then all instruction words followed by data words.
 *
 * @param asmContext Assembler context (IC, DC, memories, file names).
 * @return true on success, false on allocation/open/write errors (error printed).
 */
boolean create_obj_file(assembler_context *asmContext);

/**
 * @brief Build a new file name by replacing the extension.
 *
 * Copies @p as_file_name, removes its existing extension (if any), and appends the
 * extension that corresponds to @p type (e.g., .obj/.ext/.ent/.am).
 *
 * @param type         Target file type.
 * @param as_file_name Source file name to transform.
 * @return Newly allocated string with the new name, or NULL on error.
 *
 * @note Caller owns the returned buffer and must free() it.
 */
char* change_file_extension(file_type type, const char *as_file_name);

/**
 * @brief Validate that a file name contains only allowed characters.
 *
 * Allowed: letters/digits, '.', '-', '_', whitespace.
 *
 * @param file_name The file name to validate (must be non-NULL).
 * @param asmContext
 * @return true if valid, false otherwise.
 */
boolean is_file_name_valid(char* file_name, const assembler_context *asmContext);


/**
 * @brief Removes previously generated assembler output files for a given source file.
 *
 * This function deletes any existing output files (`.ob`, `.am`, `.ent`, `.ext`)
 * associated with the current assembly source file.
 * It is typically called before starting a new assembly process to ensure
 * that old files do not interfere with the newly generated ones.
 *
 * The function:
 *  - Builds the expected output file names by replacing the extension of the
 *    source file name (`.as`) with the respective target extensions.
 *  - Calls `remove()` on each file to delete it if it exists.
 *  - Frees the dynamically allocated memory for the generated file name strings.
 *
 * @param asmContext Pointer to the assembler context containing the
 *                   source file name and output file names.
 *
 * @note Safe to call even if some of the files do not exist,
 *       as `remove()` will silently fail in that case.
 * @note After execution, all file name pointers in `asmContext`
 *       are freed and set to `NULL`.
 */
void remove_old_files(assembler_context *asmContext);


/**
 * @brief Extracts the file extension (including the leading dot) from a file name.
 *
 * Scans @p file_name from the end toward the beginning for the last '.' and,
 * if found, returns a newly allocated C-string that starts at that dot
 * (e.g., ".as", ".obj"). If no dot exists, returns NULL.
 *
 * The returned string is heap-allocated; the caller owns it and must free it
 * via `safe_free((void**)&ptr).
 *
 * @param file_name  Null-terminated file name string (must not be NULL).
 * @return char*     Newly allocated extension string (including '.'), or NULL
 *                   when no extension is present or on allocation failure.
 *
 * @note Leading/trailing whitespace in the extracted extension is trimmed.
 * @warning This function assumes @p file_name is non-NULL and non-empty.
 */
char* get_file_extension(char* file_name);
#endif

