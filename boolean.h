
#ifndef BOOLEAN_H
#define BOOLEAN_H



/**
 * @enum boolean
 * @brief Simple boolean type replacement.
 *
 * Defines a two-valued logical type to improve readability
 * and allow functions to return `true` or `false` instead of `1` or `0`.
 *
 * @note In modern C (C99 and later), it is recommended to use `<stdbool.h>`
 * but in our project we can use only standard libraries.
 *       However, this enum ensures compatibility with older compilers.
 */
typedef enum {
    false, /**< Represents logical false (0). */
    true   /**< Represents logical true  (1). */
} boolean;




#endif
