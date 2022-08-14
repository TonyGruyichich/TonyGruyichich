
#ifndef _CODE_H
#define _CODE_H


#include "globals.h"

/*macro that moves the pointer of line to a char who is not wite space.*/
#define MOVE_TO_NOT_WHITE(line_pointer) \
		for(;(*(line_pointer)) == ' ' || (*(line_pointer)) == '\t';(line_pointer)++);
		
/*macro that checks with a pointer to a line if it looking at the end of line.*/
#define NOT_END_OF_LINE(line_pointer) \
		((*(line_pointer)) != '\n' && (*(line_pointer)) != '\0')
	
/*macro the uses the pointer to a line to check if its looking at a white space.*/	
#define NOT_WHITE(line_pointer)\
		((*(line_pointer)) != (' ') && (*(line_pointer)) != ('\t'))
		
/*macro that moves the file pointer to a not white char. and saves the last char in ch.*/
#define MOVE_FILE_POINTER_NOT_WHITE(ch , fp)\
		for(;((ch) = fgetc((fp))) != EOF && ((ch) == ' ' || (ch) == '\t');)\
			;
/*macro that checks a char if its end of line or file.*/
#define NOT_END_OF_FILE_LINE(ch)\
		((ch) != EOF && (ch) != '\n')
		


/**
 * Function that use malloc ato allocate and then checks it if allocated currectly.
 * @param size is the size to allocate.
 * @return a void pointer so every kind could use this function.
 */
void * allocate_and_check(unsigned int size);

/**
 * Function that checks if the given string is a register.
 * @param string the string that will be checked if its a name of a register.
 * @return register_type returns 0 if or NAN to indicate that its not a register, returns a number other then 
 * 0 that indicates what kind of register it is.
 */
register_type is_register(const char * string);

/**
 * Function that checks the given string if it is a command of instruction, returns the number 
 * that indicates the the instruction if it is or 0 if its not.
 * @param string rhe string to be checked.
 * @return data_type enum that indicates what instruction the string holdes.
 */
data_type is_data(const char * string);

/**
 * Function that checks if the given string is .entry, 
 * @param string is the string to be cheked.
 * @return TRUE if the string is .entry, FALSE if its not.
 */
bool is_entry(const char * string);

/**
 * Function that checkes if the given string is .extern.
 * @param string is the string to be checked.
 * @return TRUE if the string is .extern, FALSE if its not.
 */
bool is_extern(const char * string);

/**
 * Function that checks the string if its an assembly saved wird for instruction, returns 
 * not  a 0 if there was a match.
 * @pararm string is the string to be compared with.
 * @return number indicating if there was a match or not.
 */
int name_of_data(const char * string);

/**
 * Function that checks the string if its the word macro.
 * @param string the word to check.
 * @return bool TRUE if its the word macro, FALSE if its not.
 */
bool is_word_macro(const char * string);

/**
 * Function that checkes if the given string is a command, returns a number indicating with enum 
 * what command  the string is, 0 if there wasnt a match.
 * @param string the string to be checked.
 * @return command_type an enum tyoe that indicates what command was matched.
 */
command_type is_command(const char * string);

/**
 * Function that save a new node the the linked list of commands.
 * @param file_info_pointer is a pointer to a struct where the head list and tail list pointer.
 * @param current_data is the data to save in the new node.
 * @returrn the new node that had been linked to the list, the program will use only the opcode 
 * node to update it as the checking proceed.
 */
command_node * save_command_data(file_info *file_info_pointer, unsigned int current_data);

/**
 * Function that runs on the line pointed by line_pointer and collects the number and puts it in the 
 * number veriable. stops when not a number.
 * @param line_pointer is  the pointer that points on a line  of code.
 * @param number is a pointer to and int varieable and will store there the number.
 * @return bool TRUE if there was a curret use of number in the source code, FALSE if the number isnt writen 
 * good.
 */
bool is_number(char **line_pointer,int *number);

/**
 * Function that saves the number encounterd in instruction of type data in the linked list of data.
 * @param file_info_pointer the main struct that holds the data linked list.
 * @param number is the number to save.
 */
void save_data_number(file_info *file_info_pointer, int number);

/**
 * Function that checks the line if the arguments of said instruction is correct string use 
 * and send back with the given double pointer the string.
 * @param line_pointer is a pointer to a pointer of the code line.
 * @param pp_string is a double pointer to where the new string will be.
 * @return bool TRUE if a good string ecounterd, FALSE if not good.
 */
bool is_string(char ** line_pointer, char ** pp_string);

/**
 * Function that saves a new node to the linked list of data strings. makes n + 1 nodes 
 * n is thee length of string and after saves 0.
 * @param file_info_pointer had the head and tail of the linked list.
 * @param string is the string to be saved.
 */
void save_data_string(file_info * file_info_pointer, char * string);

/**
 * Function is use where there need to be a label and the bool  should be set to FALSE, if its
 * the first string of the code line then set it to TRUE so the function will wont indicate an error
 * if its not a label .
 * @param file_info_pointer for the error handaling.
 * @param string is the string to check if label or good label.
 * @param first_word is a bool to tell the function if its a first word in the line or not.
 * @return bool TRUE if its a label and a correct one, FALSE if its not a label.
 */
bool is_label(file_info * file_info_pointer, char * string, bool first_word);

/**
 * Function to save a node of label to a linked list, it makes sure that the new label name 
 * isnt been declared already if yes it will do an error.
 * @param file_info_pointer where the linked list is at.
 * @param string is the new label name.
 * @param type is the label type.
 * @return a pointer to  the new node for update.
 */
label_node * save_label(file_info * file_info_pointer, char * string, label_type type);

/**
 * Function that collects a string till encounter white space or end of line.
 * @param line_pointer a double pointer to the line of code.
 * @return pointer to the new string that been collected.
 */
char * collect_string(char ** line_pointer);

/**
 * Function same as collect_string function but stops to ',' too.
 * @param line_pointer double pointer to the code line to collect the string from.
 * @return pointer to a string that had been collected.
 */
char * collect_command_argument(char ** line_pointer);

/**
 * Function that saves a node to a linked list that saves the addres of where was the extern 
 * label was used.
 * @param file_info_pointer holds the head and tail of the linked list of used externs.
 * @param name string of the name of label.
 * @param ic the addres of where it was used.
 */
void save_extern_use(file_info * file_info_pointer, char * name, unsigned int ic);

/**
 * Function that sets the state of file to FALES meaning that its not a good writen code,
 * and prints to the stderr the error and where it was encounterd.
 * @param file_info_pointer pointer for struct that saves the data for the error handaling.
 * @param error_num is the enum and every one has its own print.
 */
void print_error(file_info * file_info_pointer, error_type error_num);


#endif
