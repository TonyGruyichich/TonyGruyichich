#ifndef _PREPROCESSOR_H
#define _PREPROCESSOR_H

/**
 * A preprocessor for the assmbler.
 * @param fp_old is a pointer to afile that needs to be checked and preprocessed.
 * @param file_name is for the name of the file that is bing processed.
 * @return bool TRUE if no errors, FALSE if encounterd errors.
 */
bool preprocessor(FILE * fp_old, char * file_name);

#endif

