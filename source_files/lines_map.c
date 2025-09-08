
#include "lines_map.h"
#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "sys_memory.h"

/**
 * @file lines_map.c
 * @brief Line mapping utilities for tracking original vs. preprocessed line numbers.
 *
 * This module maintains a mapping between original source file lines (.as) and
 * their corresponding lines in the preprocessed file (.am).
 * It is used to provide accurate error reporting during the assembler passes.
 *
 * The mapping is implemented as a linked list of `lines_LUT` nodes.
 *
 * @author Ivgeny Tokarzhevsky
 * @date 01/09/2025
 */


boolean add_lines_to_map(int original_line, int new_line, lines_map_ptr *lines_map) {

    lines_map_ptr temp;
    lines_map_ptr new_map_lines;

    /*allocate memory for new node*/
    new_map_lines = (lines_map_ptr)handle_malloc(sizeof(lines_LUT));

    if(!lines_map) {
        print_internal_error(ERROR_CODE_25,"add_lines_to_map");
        return false;
    }

    /*set value into the new node*/
    new_map_lines->orign_line_num = original_line;
    new_map_lines->new_line_num = new_line;
    new_map_lines->next =NULL;

    /*insert the new node at the end of the list*/
    if (*lines_map == NULL) {
        *lines_map = new_map_lines;
        return true;
    }
    else {
        temp = *lines_map;
        while (temp->next != NULL) {
            temp = temp->next;
        }

        temp->next = new_map_lines;
        return true;
    }

}


int get_origin_file_line(int new_line, lines_map_ptr lines_map) {


    /*search for the new line num and return the new line's original line number*/
    while (lines_map != NULL) {
        if (lines_map->new_line_num == new_line) {
            return lines_map->orign_line_num;
        }
        lines_map = lines_map->next;
    }

    return -1;
}

void free_lines_map(lines_map_ptr *lines_map) {

    lines_map_ptr temp = NULL;

    /*free the list*/
    while (*lines_map != NULL) {
        temp = *lines_map;
        *lines_map = (*lines_map)->next;
        safe_free((void**)&temp);
    }
}

void print_lines_map(lines_map_ptr lines_map) {
    /*print the lines map*/
    while (lines_map != NULL) {
        printf("origin line(.as file): %d\t.am file line: %d\n",lines_map->orign_line_num,lines_map->new_line_num);
        lines_map = lines_map->next;
    }
}



void remove_last_line(lines_map_ptr *lines_map) {
    lines_map_ptr temp;
    lines_map_ptr prev;

    if (!lines_map || !*lines_map) {
        /* list is empty */
        return;
    }

    /* if only one node */
    if ((*lines_map)->next == NULL) {
        safe_free((void**)lines_map);
        *lines_map = NULL;
        return;
    }

    /* otherwise, iterate to the second last */
    temp = *lines_map;
    prev = NULL;
    while (temp->next != NULL) {
        prev = temp;
        temp = temp->next;
    }

    /* temp is the last node, prev is the one before it */
    if (prev != NULL) {
        prev->next = NULL;
    }
    safe_free((void**)&temp);
}