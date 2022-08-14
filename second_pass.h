#ifndef _SECOND_PASS_H
#define _SECOND_PASS_H

/**
 * second_pass is a function that starts the second checking of the source assembly code, it 
 * will check the entry instruction and updated the label used in commands, if it encouters an error
 * it will keep going to find more errors.
 * @param fp is a file pointer to the source code.
 * @param file_info_pointer is a pointer to the file struct that holdes vital information and linekd list.
 * @return bool TRUE if source code is writen good and no problems encounterd, FALSE if problems encounterd.
 */
bool second_pass(FILE *fp, file_info * file_info_pointer);

#endif

