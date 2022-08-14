#ifndef _FIRST_PASS_H
#define _FIRST_PASS_H

/**
 * Function that gets file pointer and statrts checking every line for code in assembly every line will get spaicale care
 * and will be sent to the fiting function.
 * @param fp is the pointer to file that needs to be read from and processed.
 * @param dile_indo_pointer is the pointer to a struct that holdes all the importand data and linked list.
 * @return bool TRUE if the source file is a good assembly code file, FALSE if an error found.
 */
bool first_pass(FILE *fp, file_info * file_info_pointer);

#endif

