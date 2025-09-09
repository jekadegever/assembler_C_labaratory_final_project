


/**
 * @union error_codes
 * @brief Holds one of the possible error codes.
 *
 * Since an error can only belong to a single category, this union stores
 * the specific code depending on its type: system, internal, or external.
 */

#ifndef ERRORS_H
#define ERRORS_H
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#include "context.h"

/**
 * @enum internal_error_code
 * @brief Represents internal assembler error codes.
 *
 * These codes indicate bugs, invalid states, or logic errors
 * that occur inside the assembler itself. They are primarily
 * used for debugging and should not typically appear in valid
 * user input.
 *
 * Value range: 25–100 (mapped to indexes 0–74 here).
 */
typedef enum internal_error_code {
    ERROR_CODE_25 = 0,
    ERROR_CODE_26 = 1,
    ERROR_CODE_27 = 2,
    ERROR_CODE_28 = 3,
    ERROR_CODE_29 = 4,
    ERROR_CODE_30 = 5,
    ERROR_CODE_31 = 6,
    ERROR_CODE_32 = 7,
    ERROR_CODE_33 = 8,
    ERROR_CODE_34 = 9,
    ERROR_CODE_35 = 10
} internal_error_code;


/**
 * @enum external_error_code
 * @brief Represents assembler external (user-facing) error codes.
 *
 * These codes correspond to errors detected in the source assembly files,
 * such as invalid syntax, undeclared labels, or misuse of directives.
 *
 * Value range: 100–199 (mapped to indexes 0–99 here).
 */
typedef enum external_error_code {
    ERROR_CODE_100 = 0,
    ERROR_CODE_101 = 1,
    ERROR_CODE_102 = 2,
    ERROR_CODE_103 = 3,
    ERROR_CODE_104 = 4,
    ERROR_CODE_105 = 5,
    ERROR_CODE_106 = 6,
    ERROR_CODE_107 = 7,
    ERROR_CODE_108 = 8,
    ERROR_CODE_109 = 9,
    ERROR_CODE_110 = 10,
    ERROR_CODE_111 = 11,
    ERROR_CODE_112 = 12,
    ERROR_CODE_113 = 13,
    ERROR_CODE_114 = 14,
    ERROR_CODE_115 = 15,
    ERROR_CODE_116 = 16,
    ERROR_CODE_117 = 17,
    ERROR_CODE_118 = 18,
    ERROR_CODE_119 = 19,
    ERROR_CODE_120 = 20,
    ERROR_CODE_121 = 21,
    ERROR_CODE_122 = 22,
    ERROR_CODE_123 = 23,
    ERROR_CODE_124 = 24,
    ERROR_CODE_125 = 25,
    ERROR_CODE_126 = 26,
    ERROR_CODE_127 = 27,
    ERROR_CODE_128 = 28,
    ERROR_CODE_129 = 29,
    ERROR_CODE_130 = 30,
    ERROR_CODE_131 = 31,
    ERROR_CODE_132 = 32,
    ERROR_CODE_133 = 33,
    ERROR_CODE_134 = 34,
    ERROR_CODE_135 = 35,
    ERROR_CODE_136 = 36,
    ERROR_CODE_137 = 37,
    ERROR_CODE_138 = 38,
    ERROR_CODE_139 = 39,
    ERROR_CODE_140 = 40,
    ERROR_CODE_141 = 41,
    ERROR_CODE_142 = 42,
    ERROR_CODE_143 = 43,
    ERROR_CODE_144 = 44,
    ERROR_CODE_145 = 45,
    ERROR_CODE_146 = 46,
    ERROR_CODE_147 = 47,
    ERROR_CODE_148 = 48,
    ERROR_CODE_149 = 49,
    ERROR_CODE_150 = 50,
    ERROR_CODE_151 = 51,
    ERROR_CODE_152 = 52,
    ERROR_CODE_153 = 53,
    ERROR_CODE_154 = 54,
    ERROR_CODE_155 = 55,
    ERROR_CODE_156 = 56,
    ERROR_CODE_157 = 57,
    ERROR_CODE_158 = 58,
    ERROR_CODE_159 = 59,
    ERROR_CODE_160 = 60,
    ERROR_CODE_161 = 61,
    ERROR_CODE_162 = 62,
    ERROR_CODE_163 = 63,
    ERROR_CODE_164 = 64,
    ERROR_CODE_165 = 65,
    ERROR_CODE_166 = 66,
    ERROR_CODE_167 = 67,
    ERROR_CODE_168 = 68,
    ERROR_CODE_169 = 69,
    ERROR_CODE_170 = 70,
    ERROR_CODE_171 = 71,
    ERROR_CODE_172 = 72,
    ERROR_CODE_173 = 73,
    ERROR_CODE_174 = 74,
    ERROR_CODE_175 = 75,
    ERROR_CODE_176 = 76,
    ERROR_CODE_177 = 77,
    ERROR_CODE_178 = 78,
    ERROR_CODE_179 = 79,
    ERROR_CODE_180 = 80,
    ERROR_CODE_181 = 81,
    ERROR_CODE_182 = 82,
    ERROR_CODE_183 = 83
} external_error_code;


/**
 * @enum system_error_code
 * @brief Represents system-level error codes.
 *
 * These codes indicate failures from the environment (e.g., memory allocation,
 * file I/O issues). They are not caused by the user’s assembly code but rather
 * by system-level problems.
 *
 * Value range: 1–24 (mapped to indexes 0–23 here).
 */
typedef enum system_error_code {
    ERROR_CODE_1 = 0,
    ERROR_CODE_2 = 1,
    ERROR_CODE_3 = 2,
    ERROR_CODE_4 = 3,
    ERROR_CODE_5 = 4,
    ERROR_CODE_6 = 5,
    ERROR_CODE_7 = 6,
    ERROR_CODE_8 = 7,
    ERROR_CODE_9 = 8,
    ERROR_CODE_10 = 9,
    ERROR_CODE_11 = 10,
    ERROR_CODE_12 = 11,
    ERROR_CODE_13 = 12,
    ERROR_CODE_14 = 13,
    ERROR_CODE_15 = 14,
    ERROR_CODE_16 = 15,
    ERROR_CODE_17 = 16,
    ERROR_CODE_18 = 17,
    ERROR_CODE_19 = 18,
    ERROR_CODE_20 = 19,
    ERROR_CODE_21 = 20,
    ERROR_CODE_22 = 21,
    ERROR_CODE_23 = 22,
    ERROR_CODE_24 = 23
} system_error_code;


/**
 * @union error_codes
 * @brief Holds one of the possible error codes.
 *
 * Union for representing a specific error code depending on its type:
 * system, internal, or external.
 */
typedef union error_Codes {
    system_error_code system_error_code;   /**< Code for system-level errors */
    internal_error_code internal_error_code; /**< Code for internal assembler logic errors */
    external_error_code external_error_code; /**< Code for user-facing external errors */
} error_codes;


/**
 * @enum error_types
 * @brief Defines the category of error.
 *
 * Used to distinguish between system, internal, and external errors.
 */
typedef enum error_types {
    SYSTEM_ERROR,   /**< System-level error (e.g., memory, OS issues) */
    INTERNAL_ERROR, /**< Internal assembler logic error (bugs, invalid states) */
    EXTERNAL_ERROR  /**< User-facing error in the assembly source code */
} error_types;


/**
 * @enum print_params
 * @brief Indicates in which stage of the assembler the error occurred.
 */
typedef enum print_params {
    AM_FILE_LINE_AND_FILE_NAME,     /**< Error occurred during the first pass */
    SECOND_PASS_SET_LINE_AND_FILE_NAME,    /**<Error occurred during the second pass */
    AS_FILE_LINE_AND_FILE_NAME , /**<Error occurred during preprocessing */
    FILE_NAME_NO_LINE,
    NO_FILE_NAME_NO_LINE   /**<Error occurred file loading and no file name to print */

} print_params;


/**
 * @struct error
 * @brief Represents a single error definition.
 *
 * Stores the error’s stage (position), its specific code,
 * and a human-readable description.
 */
typedef struct error {
    print_params params;  /**< Stage where the error occurred */
    error_codes error_code;   /**< Specific error code */
    char *description;        /**< Human-readable description of the error */
} error;


/**
 * @brief Prints an internal assembler error and terminates the program.
 *
 * Used for unrecoverable internal logic errors (e.g., null pointers,
 * invalid state) that indicate a bug in the assembler itself.
 *
 * @param code       Error code identifying the specific internal error.
 * @param func_name  Name of the function where the error occurred.
 *
 * @note This function does not return. It calls exit(1).
 */
void print_internal_error(internal_error_code code, char* func_name);


/**
 * @brief Prints a system-level error and terminates the program.
 *
 * Used for failures caused by the underlying system (e.g.,
 * memory allocation issues, invalid bit-field ranges).
 *
 * @param code  Error code identifying the specific system error.
 *
 * @note This function does not return. It calls exit(1).
 */
void print_system_error(system_error_code code);


/**
 * @brief Prints an external assembler error (user-facing).
 *
 * Reports semantic or syntactic errors in the user’s assembly source file,
 * including file name, line number, and error description.
 * Unlike internal/system errors, external errors do not immediately
 * terminate the program, allowing continued processing of other files.
 *
 * @param code       Error code identifying the external error.
 */
void print_external_error(external_error_code code);


/**
 * @brief Set the global assembler context for error handling.
 *
 * This function assigns the provided assembler context pointer
 * to a global variable used by the error-reporting system.
 * It allows error handlers (e.g., print_internal_error, print_system_error)
 * to access the current assembler state and free allocated resources
 * before program termination.
 *
 * @param Context Pointer to the active assembler_context instance.
 *                Must not be NULL.
 *
 * @note This should be called at the start of processing each input file,
 *       and cleared/reset (if implemented) after the file processing ends.
 */
void set_error_context(assembler_context* Context);


#endif
