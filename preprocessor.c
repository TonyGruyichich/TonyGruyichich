#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "code.h"

/**
 * Function that copys word by word from a file to a file, stops when endcounterd '\n' or EOF
 * will send FALSE if error hase acoured.
 * @param fp_new a pointer to a file pointer that will be put in the words from old file.
 * @param fp_old a pointer to a file pointer that will be fetched from the words for the new file.
 * @return bool TRUE if socceeded FALSE if error encounterd.
 */
static bool file_line_copy(FILE ** fp_new, FILE ** fp_old);


/**
 * Specifec function to collect string from a file and uses a integer from the scope of the calling function to
 * give the calling function an indication of what the last char was fetched from the file. will collect a string
 * that hase no white space between the charecters and will stop if encounterd a white space or end of line.
 * @param fp file pointer of the file read from.
 * @param file_char a pointer to an integer, to use as the char holder.
 * @return string of un interapted by white space chars.
 */
static char * file_collect_string(FILE ** fp, int * file_char);

/**
 * Function that will write the string saved in the linked list macro node that maches the name.
 * @param fp_pointer is a pointer to afile pointer here the string will be wriiten.
 * @param head_node a pointer to the head of the linked list.
 * @param name  a string that holdes the name that will be searched in the linked list.
 * @return bool TRUE if found the node and writen all the data, FALSE if didnt found a matching name or erro.
 */
static bool macro_line_write(FILE ** fp_pointer, macro_node * current_pointer, char * name);

/**
 * Process to start checking if the line of file is a declaration of a macro and if is valid will save it in a linked list.
 * @param fp_pointer a pointer to a file pointer that spoucedly has found a declaration of a macro.
 * @param list_head_macro_pointer a pointer to a linked list head that may or may not be added a new node.
 * @param file_char_pointer is a pointer to an integer that will be used as a char holder and indicator to what happend , 
 * it will be used with enums that is in the globals header they are called macro_errors.
 * @return bool TRUE if line is a declaratiion and the process to save that macro has been succesful, FALSE
 * if the line is not a declaration to a macro or there was an error.
 */
static bool process_declared_macro(FILE ** fp_pointer, macro_node ** list_head_macro_pointer, int * file_char_pointer);

/**
 * Function to check if the string is a save macro in the linked list.
 * @param current_node a pointer to the head of the list.
 * @param string is the string of chars that need to be checked.
 * @return bool TRUE if there was a match, FALSE if there is no such macro declared alredy.
 */
static bool check_saved_macro(macro_node * current_node, char * string);

/**
 * Function to free all allocated list.
 * @param pointer will get the head of the list and start freeing from there.
 */
static void free_macro(macro_node * pointer);

/**
 * A small functio that similar to the one used for the first and second pass, has great potential to make this preprocessor 
 * a smart processor that will help creat better macros... print what kind of error has been found or encounterd during 
 * the preprocessing.
 * @param type is an enum of type macro_error check the globals.
 * @param name is a string that holds the name of the file bing processed.
 */
static void preprocessor_error(macro_errors type, char * name);

/**
 * A preprocessor for the assmbler. it uses fgetc for delacate filtering.
 * @param fp_old is a pointer to afile that needs to be checked and preprocessed.
 * @param file_name is for the name of the file that is bing processed.
 * @return bool TRUE if no errors, FALSE if encounterd errors.
 */
bool preprocessor(FILE * fp_old, char * file_name)
{
	FILE * fp_new;
	
	fpos_t  position;
	
	macro_node * list_head_macro;
	
	char * string; /*will save a string to check for macro.*/
	
	char * full_file_name;
	
	int  file_char;
	
	
	list_head_macro = NULL;
	
	full_file_name = (char *)  allocate_and_check(strlen(file_name) * sizeof(char) + SPACE_FOR_EXTENSION);
	
	strcpy(full_file_name, file_name);
	
	strcat(full_file_name, ".am");
	
	fp_new = fopen(full_file_name, "w");
	if(fp_new == NULL)
	{
		printf("Error: file \"%s\" is inaccessible for writting.\n", full_file_name);
		
		return FALSE;
	}
	
	/*loops iteration is every line, ends when eof..*/
	while(file_char != EOF)
	{
		fgetpos(fp_old, &position);/*save position to return th fp if a line is deamed to bee copyed.*/
		
		MOVE_FILE_POINTER_NOT_WHITE(file_char, fp_old)
		
		if(NOT_END_OF_FILE_LINE(file_char))
		{
			string = file_collect_string(&fp_old, &file_char);
			
			if(string == NULL)
			{
				if(ferror(fp_old))
				{
					preprocessor_error(FILE_ERROR, file_name);
					
					fclose(fp_new);
				
					return FALSE;
				}
				else/*deamed worthy to copy to the new file.*/
				{
					fsetpos(fp_old, &position);
					
					if(!file_line_copy(&fp_new, &fp_old))
					{
						preprocessor_error(FILE_ERROR, file_name);
						
						fclose(fp_new);
						
						return FALSE;
					}
				}
			}
			else/*first string that need to be checked.*/
			{
				if(check_saved_macro(list_head_macro, string))
				{
					
					if(NOT_END_OF_FILE_LINE(file_char))
					{
						MOVE_FILE_POINTER_NOT_WHITE(file_char, fp_old)
					}
					
					if(file_char == '\n' || (file_char == EOF && !ferror(fp_old)))
					{
						
						if(!macro_line_write(&fp_new, list_head_macro, string))/*found a macro name and copying the data of the macro.*/
						{
							preprocessor_error(FILE_ERROR, file_name);
							
							fclose(fp_new);
							
							return FALSE;
						}
					}
					else if(ferror(fp_old))
					{
						preprocessor_error(FILE_ERROR, file_name);
						
						fclose(fp_new);
						
						return FALSE;
					}
					else /*theres another word in the line will be copyed as is to the new file.*/
					{
						fsetpos(fp_old, &position);
						
						if(!file_line_copy(&fp_new, &fp_old))
						{
							preprocessor_error(FILE_ERROR, file_name);
							
							fclose(fp_new);
							
							return FALSE;
						}
					}
					
				}/*if the string is macro and there is more in the line will start macro declaration process*/
				else if((file_char == ' ' || file_char == '\t') && is_word_macro(string))
				{
					free(string);
					
					if(!process_declared_macro(&fp_old, &list_head_macro, &file_char))
					{
						if(ferror(fp_old))
						{
							preprocessor_error(FILE_ERROR, file_name);
							
							fclose(fp_new);
							
							return FALSE;
						}
						else if(file_char == SAME_MACRO_NAME || file_char == SAVED_ASSEMBLY_NAME )/*check the resone for the FALSE return enters only if error*/
						{
							preprocessor_error(file_char, file_name);
							
							fsetpos(fp_old, &position);
					
							if(!file_line_copy(&fp_new, &fp_old))
							{
								preprocessor_error(FILE_ERROR, file_name);
						
								fclose(fp_new);
				
								return FALSE;
							}
							
							fclose(fp_new);
							
							return FALSE;
						}
					}
				}
				else/*not a macro copy line to file.*/
				{
					
					free(string);
					
					fsetpos(fp_old, &position);
					
					if(!file_line_copy(&fp_new, &fp_old))
					{
						preprocessor_error(FILE_ERROR, file_name);
						
						fclose(fp_new);
				
						return FALSE;
					}
				}
			}
		}
		else if(ferror(fp_old))
		{
			preprocessor_error(FILE_ERROR, file_name);
			
			fclose(fp_new);
			
			return FALSE;
		}
		else
		{
			fsetpos(fp_old, &position);
			
			if(!file_line_copy(&fp_new, &fp_old))
			{
				preprocessor_error(FILE_ERROR, file_name);
				
				fclose(fp_new);
				
				return FALSE;
			}
		}
	}
	
	fclose(fp_new);
	
	free_macro(list_head_macro);

	return TRUE;
	
}

/**
 * A small functio that similar to the one used for the first and second pass, has great potential to make this preprocessor 
 * a smart processor that will help creat better macros... print what kind of error has been found or encounterd during 
 * the preprocessing.
 * @param type is an enum of type macro_error check the globals.
 * @param name is a string that holds the name of the file bing processed.
 */
static void preprocessor_error(macro_errors type, char * name)
{
	switch(type){
		
		case SAME_MACRO_NAME:
			
			fprintf(stderr, "Preprocessor Error: File : \"%s.as\" stoped, name for macro already used.\n"
							"Check made file named: \"%s.am\" to see where the error was found.\n",
							name, name);
			break;
			
		case SAVED_ASSEMBLY_NAME:
			
			fprintf(stderr, "Preprocessor Error: File : \"%s.as\" stoped, saved assembly name used.\n"
							"Check made file named: \"%s.am\" to see where the error was found.\n",
							name, name);
			break;	
		
		case FILE_ERROR:
		
			fprintf(stderr, "Preprocessor Error: File : \"%s.as\" stoped, encounterd a file operation error.\n"
							"Check made file named: \"%s.am\" to see where the error was encounterd.\n",
							name, name);
	}	
}

/**
 * Function to free all allocated list.
 * @param pointer will get the head of the list and start freeing from there.
 */
static void free_macro(macro_node * pointer)
{
	macro_node * temp;
	
	while(pointer != NULL)
	{
		free(pointer->macro_name);
		
		free(pointer->macro_data);
		
		temp = pointer;
		
		pointer = pointer->next;
		
		free(temp);
	}
}

/**
 * Function to check if the string is a save macro in the linked list.
 * @param current_node a pointer to the head of the list.
 * @param string is the string of chars that need to be checked.
 * @return bool TRUE if there was a match, FALSE if there is no such macro declared alredy.
 */
static bool check_saved_macro(macro_node * current_node, char * string)
{
	while(current_node != NULL)
	{
		if(!strcmp(current_node->macro_name, string))
		{
			return TRUE;
		}
		
		current_node = current_node->next;
	}
	
	return FALSE;
}

/**
 * Function that will write the string saved in the linked list macro node that maches the name.
 * @param fp_pointer is a pointer to afile pointer here the string will be wriiten.
 * @param head_node a pointer to the head of the linked list.
 * @param name  a string that holdes the name that will be searched in the linked list.
 * @return bool TRUE if found the node and writen all the data, FALSE if didnt found a matching name or erro.
 */
static bool macro_line_write(FILE ** fp_pointer, macro_node * head_node, char * name)
{
	int i ;
	
	macro_node * current_node;
	
	current_node = head_node;
	
	while(current_node != NULL && strcmp(current_node->macro_name, name))
	{
		current_node = current_node->next;
	}
	
	if(current_node != NULL)
	{
		i = fwrite(current_node->macro_data, sizeof(char), current_node->macro_size, *fp_pointer);
	
		if(i == current_node->macro_size)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	
}

/**
 *Function that saves the macro and puts it in the linked list of macros.
 * @param list_head_pointer a pointer to the head of the list to put in the macro.
 * @param mae is th name of the macro to save.
 * @param data is the string of data.
 * @param size is the size of the data.
 */
static void save_macro(macro_node ** list_head_pointer, char * name, char * data, int size);

/**
 * Function to check if the declared macro name is not a save assembly word.
 * @param string is the name of the declared macro.
 * @return bool TRUE if the name is not a saved name, FALSE if the name is a saved name.
 */
static bool check_macro_naming(char * name);


static char * my_fread(int size, FILE ** fp_pointer);

/**
 * Process to start checking if the line of file is a declaration of a macro and if is valid will save it in a linked list.
 * @param fp_pointer a pointer to a file pointer that spoucedly has found a declaration of a macro.
 * @param list_head_macro_pointer a pointer to a linked list head that may or may not be added a new node.
 * @param file_char_pointer is a pointer to an integer that will be used as a char holder and indicator to what happend , 
 * it will be used with enums that is in the globals header they are called macro_errors.
 * @return bool TRUE if line is a declaratiion and the process to save that macro has been succesful, FALSE
 * if the line is not a declaration to a macro or there was an error.
 */
static bool process_declared_macro(FILE ** fp_pointer, macro_node ** list_head_macro_pointer, int * file_char_pointer)
{
	static const char endmacro[] = "ndmacro"; /* e missing for easy filter.*/	
	
	register int length_counter;
	
	int i, sum_of_length;
	
	bool find_endmacro, flag_to_collect, first_string; /*flags for indication in the bif loop.*/
	
	char * macro_name, * string;/*string will hold the data of the macro.*/
	
	fpos_t position;
	
	
	MOVE_FILE_POINTER_NOT_WHITE(*file_char_pointer, *fp_pointer)
	
	if(!NOT_END_OF_FILE_LINE(*file_char_pointer))
	{
		return FALSE;
	}
	
	/*if name of macro is too long it wont accept it as a macro.*/
	macro_name = file_collect_string(fp_pointer, file_char_pointer);
	
	if(macro_name == NULL)
	{
		free(macro_name);
		
		return FALSE;
	}
	
	if(NOT_END_OF_FILE_LINE(*file_char_pointer))
	{
		MOVE_FILE_POINTER_NOT_WHITE(*file_char_pointer, *fp_pointer)
	}
	
	if(NOT_END_OF_FILE_LINE(*file_char_pointer))
	{
		free(macro_name);
		
		return FALSE;
	}
	
	if(!check_macro_naming(macro_name))
	{
		*file_char_pointer = SAVED_ASSEMBLY_NAME;
		
		free(macro_name);
		
		return FALSE;
	}
	
	else if(*file_char_pointer == EOF)
	{
		free(macro_name);
		
		return TRUE;
	}
	/*if macro name is taken will return false and set file_char_pointer to indicate it.*/
	if(check_saved_macro(*list_head_macro_pointer, macro_name))
	{
		*file_char_pointer = SAME_MACRO_NAME;
		
		free(macro_name);
		
		return FALSE;
	}
	
	
	flag_to_collect = TRUE;
	
	first_string = TRUE;
	
	length_counter = 0;
	
	sum_of_length = 0;
	
	fgetpos(*fp_pointer, &position);
	
	/*runs with fp till end line of valid endmacro or eof, counts the length for later use..*/
	while(flag_to_collect)
	{
		*file_char_pointer = fgetc(*fp_pointer);
		
		length_counter++;
		
		if(*file_char_pointer != ' ' && *file_char_pointer != '\t')
		{
			if(first_string && *file_char_pointer == 'e')
			{
				first_string = FALSE;
				
				find_endmacro = TRUE;
				
				i = 0;
				
				while(find_endmacro && i < strlen(endmacro) )
				{
					*file_char_pointer = fgetc(*fp_pointer);
					
					length_counter++;
					
					if(*file_char_pointer != endmacro[i++])
					{
						find_endmacro = FALSE;
					}
				}
				
				
				if(find_endmacro)/*found the word endmacro run to end to see if valid line.*/
				{
					while((*file_char_pointer = fgetc(*fp_pointer)) == ' ' || *file_char_pointer == '\t')
					{
						length_counter++;
					}
					
					length_counter++;
					
					if(NOT_END_OF_FILE_LINE(*file_char_pointer))
					{
						find_endmacro = FALSE;
					}
					else/*found valid endmacro stop the loop search.*/
					{
						flag_to_collect = FALSE;
					}
				}
			}
			
			first_string = FALSE;
			
			/*check if still not found endmacro then add to sum the length of line.*/
			if(!NOT_END_OF_FILE_LINE(*file_char_pointer)  && flag_to_collect)
			{
				first_string = TRUE;
				
				if(*file_char_pointer != EOF)/*if eof means no endmacro, check even if said no need to worry about this.*/
				{
					sum_of_length += length_counter;
				
					length_counter = 0;
				}
				else
				{
					flag_to_collect = FALSE;
				}
			}
		}
	}
	
	
	if(*file_char_pointer == EOF)
	{
		if(ferror(*fp_pointer))
		{
			free(macro_name);
			
			return FALSE;
		}
		else
		{
			free(macro_name);
			
			return TRUE;
		}
	}
	
	/*now setpos to take back pointer and then collect twith freed and the sum.*/
	fsetpos(*fp_pointer, &position);
	
	string = my_fread(sum_of_length , fp_pointer);
	
	if(string != NULL)
	{
		/*saves the allocated things so no need to free now.*/
		save_macro(list_head_macro_pointer, macro_name, string, sum_of_length);
		
		/*move the pointer after the line of endmacro.*/
		while(length_counter)
		{
			*file_char_pointer = fgetc(*fp_pointer);
			
			length_counter--;
		}
		
		if(ferror(*fp_pointer))
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else 
	{
		free(macro_name);
		
		return FALSE;
	}
}

/**
 * Function that reads n number of chars from the file.
 * @param size is the number of char wanted to be read.
 * @param fp_pointer is the pointer to the file pointer from where to read.
 * @return pointer to string that has been allocated and holds the wanted read string, if NULL means error.
 */
static char * my_fread(int size, FILE ** fp_pointer)
{
	char * string, temp_char;
	
	unsigned int i;
	
	string = (char *) allocate_and_check(size + 1);
	
	i = 0;
	
	while(size && temp_char != EOF)
	{
		temp_char = fgetc(*fp_pointer);
		
		string[i] = temp_char;
		
		size--;
		
		i++;
	}
	
	if(ferror(*fp_pointer))
	{
		return NULL;
	}
	
	string[i] = '\0';
	
	return string;
}

/**
 * Function to check if the declared macro name is not a save assembly word.
 * @param string is the name of the declared macro.
 * @return bool TRUE if the name is not a saved name, FALSE if the name is a saved name.
 */
static bool check_macro_naming(char * string)
{
	if((is_command(string) || is_register(string) || name_of_data(string) || is_word_macro(string)))
	{
		return FALSE;
	}
	
	return TRUE;
}


/**
 *Function that saves the macro and puts it in the linked list of macros.
 * @param list_head_pointer a pointer to the head of the list to put in the macro.
 * @param mae is th name of the macro to save.
 * @param data is the string of data.
 * @param size is the size of the data.
 */
static void save_macro(macro_node ** list_head_pointer, char * name, char * data, int size)
{
	macro_node * current_node, * temp_node;
	
	current_node = (macro_node *) allocate_and_check(sizeof(macro_node));
	
	current_node->macro_name = name;
	
	current_node->macro_data = data;
	
	current_node->macro_size = size;
	
	current_node->next = NULL;
	
	temp_node = *list_head_pointer;
	
	if(temp_node == NULL)
	{
		*list_head_pointer = current_node;
	}
	else
	{
		while(temp_node->next != NULL)
		{
			temp_node = temp_node->next;
		}
		
		temp_node->next = current_node;
	}
}


/**
 * Specifec function to collect string from a file and uses a integer from the scope of the calling function to
 * give the calling function an indication of what the last char was fetched from the file. will collect a string
 * that hase no white space between the charecters and will stop if encounterd a white space or end of line.
 * @param fp file pointer of the file read from.
 * @param file_char a pointer to an integer, to use as the char holder.
 * @return string of un interapted by white space chars.
 */
static char * file_collect_string(FILE ** fp, int * file_char)
{
	char * string;
	
	unsigned int i = 0;
	
	string = (char *) allocate_and_check(sizeof(char) * MAX_LINE_LENGTH + 1);
	
	string[i++] = *file_char;
	
	while((*file_char = fgetc(*fp)) != EOF && *file_char != '\n' && 
			*file_char != ' ' && *file_char != '\t')
	{
		string[i] = *file_char;
		
		i++;
		
		if(i > MAX_LINE_LENGTH)
		{
			free(string);
			
			return NULL;
		}
	}
	
	if(ferror(*fp))
	{
		free(string);
		
		return NULL;
	}
	
	string[i] = '\0';
	
	return string;
}


/**
 * Function that copys word by word from a file to a file, stops when endcounterd '\n' or EOF
 * will send FALSE if error hase acoured.
 * @param fp_new a pointer to a file pointer that will be put in the words from old file.
 * @param fp_old a pointer to a file pointer that will be fetched from the words for the new file.
 * @return bool TRUE if socceeded FALSE if error encounterd.
 */
static bool file_line_copy(FILE ** fp_new, FILE ** fp_old)
{
	int ch;
	
	while((ch = fgetc(*fp_old)) != EOF && ch != '\n')
	{
		if(fputc(ch, *fp_new) == EOF)
		{
			return FALSE;
		}
	}
	
	if(ferror(*fp_old))
	{
		return FALSE;
	}
	
	if(ch == '\n')/*needs to put this char too*/
	{
		if(fputc(ch, *fp_new) == EOF)
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

















