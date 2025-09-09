
#ifndef LINES_MAP_H
#define LINES_MAP_H

#include "lines_map.h"
#include "util.h"
#include "typedef.h"



/**
 * @struct lines_LUT
 * @brief Node structure for the line mapping linked list.
 *
 * Each node maps one original line number from the `.as` file
 * to a corresponding line number in the `.am` file.
 *
 * @var lines_LUT::orign_line_num
 *      Original line number in the `.as` file.
 * @var lines_LUT::new_line_num
 *      Corresponding line number in the `.am` file.
 * @var lines_LUT::next
 *      Pointer to the next node in the mapping list.
 */
typedef struct lines_LUT {
    int orign_line_num;
    int new_line_num;
    lines_map_ptr next;
} lines_LUT;


/**
 * @brief Add a new mapping entry between an original source line and its preprocessed line.
 *
 * Creates a new node with the original line number (`original_line`)
 * and the new preprocessed line number (`new_line`) and appends it to the
 * end of the mapping list.
 *
 * @param original_line Original line number in the `.as` file.
 * @param new_line      Corresponding line number in the `.am` file.
 * @param lines_map     Pointer to the head of the mapping list.
 * @return true if successfully added, false if allocation failed or invalid args.
 */
boolean add_lines_to_map(int original_line, int new_line, lines_map_ptr *lines_map);

/**
 * @brief Retrieve the original line number given a preprocessed line number.
 *
 * Iterates over the mapping list and returns the matching original
 * line number if found.
 *
 * @param new_line  Line number in the `.am` file.
 * @param lines_map Head of the mapping list.
 * @return int Original `.as` file line number, or -1 if not found.
 */
int get_origin_file_line(int new_line, lines_map_ptr lines_map);

/**
 * @brief Free the entire line mapping list.
 *
 * Releases all allocated nodes in the mapping list.
 *
 * @param lines_map Pointer to the head of the mapping list.
 */
void free_lines_map(lines_map_ptr *lines_map);

/**
 * @brief Print the entire line mapping for debugging purposes.
 *
 * Displays each mapping in the format:
 * ```
 * origin line(.as file): <num>    .am file line: <num>
 * ```
 *
 * @param lines_map Head of the mapping list.
 */
void print_lines_map(lines_map_ptr lines_map);



/**
 * @brief Remove the last node from the lines map linked list.
 *
 * If the list is empty, nothing happens. If the list contains only one node,
 * it will be freed and the head will be set to NULL.
 *
 * @param lines_map Pointer to the head pointer of the lines map list.
 */
void remove_last_line(lines_map_ptr *lines_map);



#endif
