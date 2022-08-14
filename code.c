#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "code.h"
#include "globals.h"


/**
 * Function that use malloc ato allocate and then checks it if allocated currectly.
 * @param size is the size to allocate.
 * @return a void pointer so every kind could use this function.
 */
void * allocate_and_check(unsigned int size)
{
	void * pointer;
	
	pointer = (void *) malloc(size);
	
	if(pointer == NULL)
	{
		fprintf(stderr,"Failed to allocate memory, program terminated.\n");
		
		exit(1);
	}
	
	return pointer;
}

/**
 * Function that checks if the given string is a register.
 * @param string the string that will be checked if its a name of a register.
 * @return register_type returns 0 if or NAN to indicate that its not a register, returns a number other then 
 * 0 that indicates what kind of register it is.
 */
register_type is_register(const char * string)
{
	register int i;
	
	register_type type = NAN;/*set to 0 */
	
	static char const registers_name[][MAX_REGISTER_LENGTH] = {"r0", "r1", "r2", "r3", "r4",
											 "r5", "r6", "r7", "PSW"};
	
	for(i = 0; i < NUMBER_OF_REGISTERS; i++)
	{
		if(!strcmp(string, registers_name[i]))
		{
			type = i + 1;/*+1 so this func could be used for boolian.*/
		}
	}
	
	return type;
}

/**
 * Function that checks the given string if it is a command of instruction, returns the number 
 * that indicates the the instruction if it is or 0 if its not.
 * @param string rhe string to be checked.
 * @return data_type enum that indicates what instruction the string holdes.
 */
data_type is_data(const char * string)
{
	int i;
	
	data_type type = NAN;
	
	static char const data_types[][MAX_INSTRUCT_LENGTH] = {".data", ".string", ".struct"};
	
	for(i = 0; i < NUMBER_OF_DATATYPE ; i++)
	{
		if(!strcmp(data_types[i], string))
		{
			type = (i + 1); /*+1 so this func could be used for boolian.*/
		}
	}
	
	return type;
}

/**
 * Function that checks if the given string is .entry, 
 * @param string is the string to be cheked.
 * @return TRUE if the string is .entry, FALSE if its not.
 */
bool is_entry(const char * string)
{
	static char const entry_name[] = ".entry" ;
	
	bool flag = FALSE;
	
	if(!strcmp(string,entry_name))
	{
		flag = TRUE;
	}
	
	return flag;
}

/**
 * Function that checkes if the given string is .extern.
 * @param string is the string to be checked.
 * @return TRUE if the string is .extern, FALSE if its not.
 */
bool is_extern(const char * string)
{
	static char const  extern_name[] = ".extern";
	
	bool flag = FALSE;
	
	if(!strcmp(string, extern_name))
	{
		flag = TRUE;
	}
	
	return flag;
}

/**
 * Function that checkes if the given string is a command, returns a number indicating with enum 
 * what command  the string is, 0 if there wasnt a match.
 * @param string the string to be checked.
 * @return command_type an enum tyoe that indicates what command was matched.
 */
command_type is_command(const char * string)
{
	register int i;
	
	command_type type = NAN;
	/*will be used allot.*/
	static char const command_names[][MAX_COMMAND_LENGTH] = {
							  "mov", "cmp", "add", "sub", "not",
							  "clr", "lea", "inc", "dec", "jmp",
							  "bne", "get", "prn", "jsr",		
							  "rts", "hlt"};					
	
	for(i = 0; i < NUMBER_OF_COMMANDS; i++)
	{
		if(!strcmp(string, command_names[i]))
		{
			/*+1 so this func could be used for boolian.*/
			type = ++i;
			
			return type;
		}
	}
	
	return type;
}

/**
 * Function that checks the string if its an assembly saved wird for instruction, returns 
 * not  a 0 if there was a match.
 * @pararm string is the string to be compared with.
 * @return number indicating if there was a match or not.
 */
int name_of_data(const char * string)
{
	unsigned int i;
	
	unsigned int type = NAN;
	
	static char const data_types[][MAX_INSTRUCT_LENGTH] = {"data", "string", "struct", "entry", "extern"};
	
	for(i = 0; i < NUMBER_OF_INSTRUCTIONS ; i++)
	{
		if(!strcmp(data_types[i], string))
		{
			type = (i + 1); /*+1 so this func could be used for boolian.*/
		}
	}
	
	return type;
}

/**
 * Function that checks the string if its the word macro.
 * @param string the word to check.
 * @return bool TRUE if its the word macro, FALSE if its not.
 */
bool is_word_macro(const char * string)
{
	static char const macro[] = "macro";
	
	if(!strcmp(macro, string))
	{
		return TRUE;
	}
	
	return FALSE;
}

/**
 * Function that save a new node the the linked list of commands.
 * @param file_info_pointer is a pointer to a struct where the head list and tail list pointer.
 * @param current_data is the data to save in the new node.
 * @returrn the new node that had been linked to the list, the program will use only the opcode 
 * node to update it as the checking proceed.
 */
command_node * save_command_data(file_info *file_info_pointer, unsigned int current_data)
{
	/*this pointers will help with positioning of the node in the list.*/
	command_node * temp_node, * current_node;
	
	current_node = file_info_pointer->list_tail_command;
	
	
	temp_node = (command_node *) malloc(sizeof(command_node));
	if(temp_node == NULL)
	{
		fprintf(stderr, "Failed to allocate memory, program terminated.");
			
		exit(1);
	}
	
	if(current_node == NULL)/*if no instruction yet saved.*/
	{
		current_node = temp_node;	
		
		current_node->data = current_data;
		
		current_node->next = NULL;
		
		/*adjusting main pointers of list*/
		file_info_pointer->list_head_command = current_node;
		
		file_info_pointer->list_tail_command = current_node;
		
		file_info_pointer->IC++;
		
		return current_node; 
	}
	
	temp_node->data = current_data;
	
	temp_node->next = NULL;
	
	current_node->next = temp_node;
	
	file_info_pointer->list_tail_command = temp_node;
	
	file_info_pointer->IC++;
	
	return temp_node;
}

/**
 * Function that runs on the line pointed by line_pointer and collects the number and puts it in the 
 * number veriable. stops when not a number.
 * @param line_pointer is  the pointer that points on a line  of code.
 * @param number is a pointer to and int varieable and will store there the number.
 * @return bool TRUE if there was a curret use of number in the source code, FALSE if the number isnt writen 
 * good.
 */
bool is_number(char **line_pointer,int *number)
{
	int sign = 1, temp;
	
	bool flag = FALSE;
	
	if(**line_pointer == '-' || **line_pointer == '+')
	{
		if(**line_pointer == '-')
		{
			sign = -1;
		}
		
		(*line_pointer)++;
	}
	
	/*first check to set flag true if number*/
	if(isdigit(**line_pointer))
	{
		temp = (**line_pointer - '0');
		
		(*line_pointer)++;
		
		flag = TRUE;
	}
	
	/*while still number*/
	while(flag && NOT_WHITE(*line_pointer) && NOT_END_OF_LINE(*line_pointer) && **line_pointer != ',')
	{
		if(isdigit(**line_pointer))
		{
			temp *= 10;
			
			temp += (**line_pointer - '0');
			
			(*line_pointer)++;
			
		}
		else
		{
			flag = FALSE;
		}
	}
	
	/*save the number to pointer*/
	*number = sign * temp;
	
	/*return true if good number (" -10 "|| "+9," || "9\n")*/
	return flag;
}

/**
 * Function that saves the number encounterd in instruction of type data in the linked list of data.
 * @param file_info_pointer the main struct that holds the data linked list.
 * @param number is the number to save.
 */
void save_data_number(file_info *file_info_pointer, int number)
{
	data_node * current_node, * temp_node;
	
	
	current_node = file_info_pointer->list_tail_data;
	
	
	/*changing number to fit 8 bit */
	if(number > MAX_SIGNED_NUM)
	{
		number = MAX_MASK;
	}
	else if(number < MIN_SIGNED_NUM)
	{
		number = MIN_MASK;
	}
	else if(number < 0)
	{
		number &= MINUS_MASK;
	}
	
	temp_node = (data_node *) allocate_and_check(sizeof(data_node));
	
	/*if first node set head and tail.*/
	if(current_node == NULL)
	{
		current_node = temp_node;
		
		current_node->data = number;
		
		file_info_pointer->list_head_data = current_node;
		
		file_info_pointer->list_tail_data = current_node;
		
		current_node->next = NULL;
		
		file_info_pointer->DC++;
		
		return;
	}

	
	temp_node->data = number;
	
	temp_node->next = NULL;
	
	current_node->next = temp_node;
	
	file_info_pointer->list_tail_data = temp_node;
	
	file_info_pointer->DC++;
}

/**
 * Function that checks the line if the arguments of said instruction is correct string use 
 * and send back with the given double pointer the string.
 * @param line_pointer is a pointer to a pointer of the code line.
 * @param pp_string is a double pointer to where the new string will be.
 * @return bool TRUE if a good string ecounterd, FALSE if not good.
 */
bool is_string(char ** line_pointer, char ** pp_string)
{
	/*arry to save new string*/
	char arr_string[MAX_LINE_LENGTH];
	
	int i;
	
	bool flag = TRUE;
	
	if(**line_pointer == '"')
	{
		(*line_pointer)++;
		
		/*collecting string.*/
		for(i = 0; NOT_END_OF_LINE(*line_pointer) && **line_pointer != '"' && i < MAX_LINE_LENGTH; i++)
		{
			arr_string[i] = **line_pointer;
			
			(*line_pointer)++;
		}
		
		if(**line_pointer == '"')
		{
			arr_string[i] = '\0';
			
			*pp_string = (char *) allocate_and_check(sizeof(char) * i);
			
			strcpy(*pp_string, arr_string);
			
			(*line_pointer)++;
		}
		else
		{
			flag = FALSE;
		}
	}
	else
	{
		flag = FALSE;
	}
	
	return flag;
}

/**
 * Function that saves a new node to the linked list of data strings. makes n + 1 nodes 
 * n is thee length of string and after saves 0.
 * @param file_info_pointer had the head and tail of the linked list.
 * @param string is the string to be saved.
 */
void save_data_string(file_info * file_info_pointer, char * string)
{
	data_node * current_node, * temp_node;
	
	
	temp_node = (data_node *) allocate_and_check(sizeof(data_node));
	
	current_node = file_info_pointer->list_tail_data;
	
	/*if first node set head and tail.*/
	if(current_node == NULL)
	{
		current_node = temp_node;
		
		current_node->data = (unsigned int) *string++;
		
		current_node->next = NULL;
		
		file_info_pointer->list_head_data = current_node;
		
		file_info_pointer->list_tail_data = current_node;
		
		file_info_pointer->DC++;
	}
	
	temp_node = (data_node *) allocate_and_check(sizeof(data_node));
	
	while(*string != '\0')
	{
		temp_node->data = (int) *string++;
		
		current_node->next = temp_node;
		
		current_node = temp_node;
		
		file_info_pointer->DC++;
		
		temp_node = (data_node *) allocate_and_check(sizeof(data_node));
		
	}
	
	temp_node->data = '\0';
	
	temp_node->next =NULL;
	
	current_node->next = temp_node;
	
	file_info_pointer->list_tail_data = temp_node;
	
	file_info_pointer->DC++;

}

/**
 * Function is use where there need to be a label and the bool  should be set to FALSE, if its
 * the first string of the code line then set it to TRUE so the function will wont indicate an error
 * if its not a label .
 * @param file_info_pointer for the error handaling.
 * @param string is the string to check if label or good label.
 * @param first_word is a bool to tell the function if its a first word in the line or not.
 * @return bool TRUE if its a label and a correct one, FALSE if its not a label.
 */
bool is_label(file_info * file_info_pointer, char * string, bool first_word)
{
	int i = 0;
	
	bool flag = TRUE;
	
	if(first_word && string[strlen(string) - 1] == ':')
	{
		string[strlen(string) - 1] = '\0';
	}
	/*if there is no ":" and first word, then it could be instruction so return false.*/
	else if(first_word)
	{
		flag = FALSE;
		
		return flag;
	}
	
	/*makes the assumption that is spposed to be a label and checks if its good.*/
	if(isalpha(string[i++]))
	{
		/*loop to check all the chars */
		for(;string[i] != '\0' && flag; i++)
		{
			if(!isalnum(string[i]))
			{
				flag = FALSE;
				
				print_error(file_info_pointer, BAD_LABEL_NAME);
				
				return flag;
			}
		}
		if(i <= MAX_LABEL_LENGTH + 1)
		{
			/*checks if name is not a saved name of the assembly(r0 || mov)*/
			if((is_command(string) || is_register(string) || name_of_data(string) || is_word_macro(string)))
			{
				flag = FALSE;
			
				print_error(file_info_pointer, SAVED_LABEL_NAME);
			}
		}
		else
		{
			flag = FALSE;
			
			print_error(file_info_pointer, BAD_LABEL_NAME);
		}
	}/*end of if alpha*/
	else
	{
		/*maybe a saved name of assembly ('.') or just bad start of name*/
		flag = FALSE;
		
		print_error(file_info_pointer, BAD_LABEL_NAME);
	}

	return flag;
}

/**
 * Function to save a node of label to a linked list, it makes sure that the new label name 
 * isnt been declared already if yes it will do an error.
 * @param file_info_pointer where the linked list is at.
 * @param string is the new label name.
 * @param type is the label type.
 * @return a pointer to  the new node for update.
 */
label_node * save_label(file_info * file_info_pointer, char * string, label_type type)
{
	label_node * temp_node, * current_node, * place_holder_node;
	
	
	temp_node = (label_node *) allocate_and_check(sizeof(label_node));

	temp_node->label_name = (char *) allocate_and_check(sizeof(char) * (MAX_LABEL_LENGTH + 1));
	
	
	/*updating node.*/
	strcpy(temp_node->label_name, string);
	
	temp_node->type = type;
	
	temp_node->next = NULL;
	
	if(type != LABEL_IS_EXTERN && type != LABEL_IS_COMMAND)
	{
		temp_node->label_addres = file_info_pointer->DC;
	}
	else if(type == LABEL_IS_COMMAND)
	{
		temp_node->label_addres = file_info_pointer->IC;
	}
	else if(type == LABEL_IS_EXTERN)
	{
		temp_node->label_addres = NAN;
	}
		
		
	current_node = file_info_pointer->list_head_label;
	
	/*first node allocation and head allining*/
	if(current_node == NULL)
	{
		file_info_pointer->list_head_label = temp_node;
		
		return temp_node;
	}
	
	/*loop till end of chain, checks if name is taken in every loop.*/
	while(current_node != NULL)
	{
		if(!strcmp(string, current_node->label_name))
		{
			
			print_error(file_info_pointer, SAME_LABEL_NAME);
			
			free(temp_node->label_name);
			
			free(temp_node);
			
			return NULL;
		}
		
		place_holder_node = current_node;
		
		current_node = place_holder_node->next;
		
	}
	
	place_holder_node->next = temp_node;
	
	return temp_node;
}

/**
 * Function that collects a string till encounter white space or end of line.
 * @param line_pointer a double pointer to the line of code.
 * @return pointer to the new string that been collected.
 */
char * collect_string(char ** line_pointer)
{
	int i = 0;
	char * string;
	
	string = (char *) allocate_and_check(sizeof(char)*(MAX_LINE_LENGTH + 1));
	
	
	while(NOT_END_OF_LINE(*line_pointer) && NOT_WHITE(*line_pointer) && i < MAX_LINE_LENGTH)
	{
		string[i++] = **line_pointer;
		
		(*line_pointer)++;
	}
	
	string[i] ='\0';
	
	return string;
}

/**
 * Function same as collect_string function but stops to ',' too.
 * @param line_pointer double pointer to the code line to collect the string from.
 * @return pointer to a string that had been collected.
 */
char * collect_command_argument(char ** line_pointer)
{
	int i = 0;
	
	char * string;
	
	string = (char *) allocate_and_check(sizeof(char)*(MAX_LINE_LENGTH + 1));
	
	
	while(NOT_END_OF_LINE(*line_pointer) && NOT_WHITE(*line_pointer) && **line_pointer != ',' && i < MAX_LINE_LENGTH)
	{
		string[i++] = **line_pointer;
		
		(*line_pointer)++;
	}
	
	string[i] ='\0';
	
	return string;
}

/**
 * Function that saves a node to a linked list that saves the addres of where was the extern 
 * label was used.
 * @param file_info_pointer holds the head and tail of the linked list of used externs.
 * @param name string of the name of label.
 * @param ic the addres of where it was used.
 */
void save_extern_use(file_info * file_info_pointer, char * name,  unsigned int ic)
{
	extern_node * current_node, * temp_node;	
	
	temp_node = (extern_node *) allocate_and_check(sizeof(extern_node));				
	
	temp_node->label_name = (char *) allocate_and_check(sizeof(char) * (MAX_LABEL_LENGTH + 1));
	
	/*updating the data of the node.*/
	strcpy(temp_node->label_name, name);	
	
	temp_node->addres = ic;
	
	temp_node->next = NULL;
	
	/*adjasting node to list.*/
	current_node = file_info_pointer->list_tail_extern;
	
	if(current_node == NULL)
	{
		file_info_pointer->list_head_extern = temp_node;
		
		file_info_pointer->list_tail_extern = temp_node;
	}
	else
	{
		current_node->next = temp_node;
		
		file_info_pointer->list_tail_extern = temp_node;
	}

}				

/**
 * Function that sets the state of file to FALES meaning that its not a good writen code,
 * and prints to the stderr the error and where it was encounterd.
 * @param file_info_pointer pointer for struct that saves the data for the error handaling.
 * @param error_num is the enum and every one has its own print.
 */
void print_error(file_info * file_info_pointer, error_type error_num)
{
	file_info_pointer->is_file_good = FALSE;
	
	switch (error_num)
	{
		case LINE_TOO_LONG:
		
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. Line is over maximum length: %d chars.\n",
			file_info_pointer->file_name, file_info_pointer->line_number, MAX_LINE_LENGTH);
			
			break;
			
		case BAD_LABEL_NAME:
		
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. Bad label name.\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
			
			break;
			
		case SAME_LABEL_NAME:
		
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. Label name is taken.\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
			
			break;
			
		case ERROR_READING_FILE:
		
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. Error encounterd stoping file process.\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
			
			break;
		
		case OVER_SIZE_LIMIT:
		
			fprintf(stderr, "\nErorr: File: \"%s\". Source file is over the max memory usable: %d bytes.\n",
			file_info_pointer->file_name, MAX_NUMBER_OF_LINES);
			
			break;
		
		case WRONG_LABEL_USE:
		
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. Label isnt usable here.\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
			
			break;
		
		case NOT_A_NUMBER:
		
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. expexcted a number.\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
			
			break;
		
		case NOT_A_STRING:
		
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. expected a string.\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
			
			break;
		
		case TOO_MUCH_ARGUMENTS:
		
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. Too much arguments.\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
			
			break;
		
		case UNRECOGNIZED_CODE:
		
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. Code isnt recognized.\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
			
			break;
		
		case WRONG_USE_CODE:
		
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. Wrong use of code.\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
			
			break;
		
		case WRONG_USE_REGISTER:
		
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. Wrong use of register.\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
			
			break;
		
		case WRONG_USE_STRUCT:
		
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. Wrong use of struct.\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
			
			break;
		
		case ENTRY_EXTERN_DUP:
		
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. Entry and extern use same label name.\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
			
			break;
			
		case ENTRY_ENTRY_DUP:
			
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. Duplicate entry label declarations.\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
			
			break;
			
		case LABEL_NOT_FOUND:
		
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. Label was'nt declared.\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
			
			break;
			
		case SAVED_LABEL_NAME:
		
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. Saved assembler name used.\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
			
			break;
			
		case STRING_TO_LONG:
		
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. Argument is too long.\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
			
			break;
			
		case WRONG_USE_LABEL:
			
			fprintf(stderr, "\nErorr: File: \"%s\" Line: %d. Wrong use of label.\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
			
			
	}
	
	/*strstr to check if they meant command or something else. maybe if i have time.*/
}






