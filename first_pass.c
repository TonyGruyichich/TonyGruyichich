#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "code.h"




/**
 * Function that increments the DC the amount incremented is IC.
 * @param current_node is the list head pointer of the label linked list.
 * @param ic is the amount incremented by.
 */
static void list_label_update(label_node * current_ndoe, const unsigned int ic);	

/**
 * Function that processes a line of assembly code.
 * @param file_info_pointer is a pointer to the main file struct that holdes all the linked list and data of the file.
 * @param line_pointer is a pointer to apointer of a string of code.
 * @param first_string is the first string that has been encounterd in the code line.
 */	
static void code_process(file_info * file_info_pointer, char ** line_pointer, char * first_string);

/**
 * Function that gets file pointer and statrts checking every line for code in assembly every line will get spaicale care
 * and will be sent to the fiting function.
 * @param fp is the pointer to file that needs to be read from and processed.
 * @param dile_indo_pointer is the pointer to a struct that holdes all the importand data and linked list.
 * @return bool TRUE if the source file is a good assembly code file, FALSE if an error found.
 */
bool first_pass(FILE *fp, file_info * file_info_pointer)
{	
						/*+1 for '\0'*/
	char current_line[MAX_LINE_LENGTH + 1];
	
	char * line_pointer, * first_string;
	
	/*file_state is will point on the if file is good or bad.*/
	bool * file_state;
	
	int * line_counter;
	
	register int file_char;
	
	
	/*preparing the file state.*/
	file_state = &file_info_pointer->is_file_good;
	
	*file_state = TRUE;
	
	line_counter = &file_info_pointer->line_number;
	
	*line_counter = 1;
	
	line_pointer = fgets(current_line, MAX_LINE_LENGTH + 1, fp);
	
	file_char = 0;
	
	/*loops on every line of source code and counts the line for index of errors.*/
	while( line_pointer != NULL) 
	{
	
		/*if ; its a comment no need to its length or content.*/
		if(*line_pointer != ';')
		{
			MOVE_TO_NOT_WHITE(line_pointer)
			
			/*checking if white line no need length or content*/
			if(NOT_END_OF_LINE(line_pointer))
			{
				first_string = collect_string(&line_pointer);
				
				
				code_process(file_info_pointer, &line_pointer, first_string);
				
				
				free(first_string);
				
				/*check if line may be too long.*/
				if(*line_pointer == '\0')
				{
					
					if((file_char = fgetc(fp)) != '\n' && file_char != EOF)
					{
						/*loop to get to the seconde line and checks if only white.*/	
						while(file_char != '\n' && file_char != EOF)
						{
							if(file_char == ' ' || file_char == '\t')
							{
								file_char = fgetc(fp);
							}
							else/*send an error if not white after 80 length*/
							{
								print_error(file_info_pointer, LINE_TOO_LONG);
								
								file_char = fgetc(fp);
							}
						}
					}
				}
			}
		}
		/*check if end or error from fgetc().*/
		if(file_char != EOF)
		{
			file_info_pointer->line_number++;
				
			line_pointer = fgets(current_line, MAX_LINE_LENGTH + 1, fp);
		}
		else
		{
			line_pointer = NULL;
		}
	}/*while end.*/
	
	/*check for error from fgrts().*/
	if(ferror(fp))
	{
		print_error(file_info_pointer, ERROR_READING_FILE);
		
		return FALSE;
	}
	
	/*checking if the source code isnt to big.*/
	if((file_info_pointer->IC + file_info_pointer->DC + MEMORY_LOCATION_START) > MAX_NUMBER_OF_LINES)
	{
		print_error(file_info_pointer, OVER_SIZE_LIMIT);
		
		return FALSE;
	}
	
	
	/*updating the addres of data labels.*/
	if(*file_state)
	{	
		list_label_update(file_info_pointer->list_head_label, file_info_pointer->IC);
	}
	
	
	return *file_state;
}
	
	
	
	
/**
 * Function that increments the DC the amount incremented is IC.
 * @param current_node is the list head pointer of the label linked list.
 * @param ic is the amount incremented by.
 */
static void list_label_update(label_node * current_node, const unsigned int ic)
{
	while(current_node != NULL)
	{
		
		if(current_node->type == LABEL_IS_DATA)
		{
			
			current_node->label_addres += ic;
		}
		
		current_node = current_node->next;
	}
}

/**
 * Function that get a command and couple of arguments found in the the line of code and the function 
 * will sort and check it to see if its a good assembly command syntax or use.
 * @param file_info_pointer is a pointer to a pointer of the main struct that holdes all the information of the file.
 * @param type is the command type used int sorce line code.
 * @param source_argument_string is the string of the first argument that came after the command.
 * @param destination_argument_string is the string of the second argument that came after the command.
 */
static void procces_command_code(file_info *file_info_pointer, command_type type,
 								char * source_argument_string, char * destination_argument_string);
								

/**
 * Function that processes a line of assembly code.
 * @param file_info_pointer is a pointer to the main file struct that holdes all the linked list and data of the file.
 * @param line_pointer is a pointer to apointer of a string of code.
 * @param first_string is the first string that has been encounterd in the code line.
 */
static void code_process(file_info * file_info_pointer, char ** line_pointer, char * first_string)
{
	/*number is used to store the numbers collected.*/
	int number;
	/*string is used to store the instraction argument_string is for the arguments.*/
	char * string, * source_argument_string, * destination_argument_string;
	/*type will save the instruction or command type and source.*/
	int type;
	
	bool flag_save_label = TRUE;
	
	label_node * pointer_to_label;
	
	pointer_to_label = NULL;
	
	if(first_string[strlen(first_string) - 1] != ':')
	{
		flag_save_label = FALSE;
		
		string = (char *) allocate_and_check(sizeof(char) * MAX_LINE_LENGTH);
		
		strcpy(string, first_string);/*copying the instruction in first_string.*/
		
	}
	else
	{
		if(!is_label(file_info_pointer, first_string, TRUE))
		{
			return;
		}
		
		MOVE_TO_NOT_WHITE(*line_pointer)
	
		if(!NOT_END_OF_LINE(*line_pointer))/*error if end of line.*/
		{
			print_error(file_info_pointer, WRONG_LABEL_USE);
		
			return;
		}
		
		string = collect_string(line_pointer);
	}
	/*start of proccecing the code.*/
	if((type = is_data(string)))
	{
		/*string only needed to check what kind of instruction.*/
		free(string);
		
		if(flag_save_label)
		{
			pointer_to_label = save_label(file_info_pointer, first_string, LABEL_IS_DATA);	

			/*for use in second pass to check if used a number to acces struct.*/
			if(pointer_to_label != NULL)
			{
				pointer_to_label->label_is_struct = FALSE;
			}
		}
		else
		{
			fprintf(stderr,"\nWarning: File: \"%s\" Line: %d. No label to access data.\n",
					file_info_pointer->file_name, file_info_pointer->line_number);
		}
		
		/*procedure to collect all numbers, return if good or error.*/
		if(type == IS_DATA)
		{
			/*while collects all posible numbers, ends when error or end of line.*/
			while(TRUE)
			{
				MOVE_TO_NOT_WHITE(*line_pointer)
				
				if(is_number(line_pointer, &number))
				{
					save_data_number(file_info_pointer, number);
				}
				else
				{
					print_error(file_info_pointer, NOT_A_NUMBER);
					
					return;
				}
				
				MOVE_TO_NOT_WHITE(*line_pointer)
				
				if(NOT_END_OF_LINE(*line_pointer) && **line_pointer != ',')
				{
					print_error(file_info_pointer, NOT_A_NUMBER);
					
					return;
				}
				else if(!NOT_END_OF_LINE(*line_pointer))
				{
					
					return;
				}
				
				/*points to ',' so inc.*/
				(*line_pointer)++;
			}
		}
		/*procedure to collect the number and then a string, returns if error or good.*/
		else if(type == IS_STRUCT)
		{
			if(pointer_to_label != NULL)
			{
				pointer_to_label->label_is_struct = TRUE;
			}
			
			MOVE_TO_NOT_WHITE(*line_pointer)
			
			if(is_number(line_pointer, &number))
			{
				save_data_number(file_info_pointer, number);
			}
			else
			{
				print_error(file_info_pointer, NOT_A_NUMBER);
				
				return;
			}
			
			MOVE_TO_NOT_WHITE(*line_pointer)
			
			if(**line_pointer == ',')
			{
				(*line_pointer)++;
				
			}
			else
			{
				print_error(file_info_pointer, WRONG_USE_STRUCT);
				
				return;
			}
			
			MOVE_TO_NOT_WHITE(*line_pointer)
			
			if(is_string(line_pointer, &source_argument_string))
			{
				save_data_string(file_info_pointer, source_argument_string);
				
				free(source_argument_string);
			}
			else
			{
				print_error(file_info_pointer, NOT_A_STRING);
				
				return;
			}
			
			MOVE_TO_NOT_WHITE(*line_pointer)
			
			if(NOT_END_OF_LINE(*line_pointer))
			{
				print_error(file_info_pointer, TOO_MUCH_ARGUMENTS);
				
				return;
			}
			
			return;
		}
		/*procedure to collect the string. returns if error or good.*/
		else/*is string*/
		{
			MOVE_TO_NOT_WHITE(*line_pointer)
			
			if(is_string(line_pointer, &source_argument_string))
			{
				save_data_string(file_info_pointer, source_argument_string);
				
				free(source_argument_string);
			}
			else
			{
				print_error(file_info_pointer, WRONG_USE_CODE);
				
				return;
			}
			
			MOVE_TO_NOT_WHITE(*line_pointer)
			
			if(NOT_END_OF_LINE(*line_pointer))
			{
				print_error(file_info_pointer, TOO_MUCH_ARGUMENTS);
				
				return;
			}
			
			return;
		}
	}
	else if(is_entry(string))
	{
		free(string);
		
		file_info_pointer->is_file_entry = TRUE;
		
		if(flag_save_label)
		{
			/*dos'nt save's the label, prints a warrning.*/
			fprintf(stderr,"\nWarning: File: \"%s\" Line: %d. Use of label wont work\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
		}
		
		if(!NOT_END_OF_LINE(*line_pointer))
		{
			print_error(file_info_pointer, WRONG_USE_CODE);
				
			return;
		}
		
		/*loop to check all labels. stops when end is reached.*/
		while(NOT_END_OF_LINE(*line_pointer))
		{
			MOVE_TO_NOT_WHITE(*line_pointer)
			
			if(!NOT_END_OF_LINE(*line_pointer))
			{
				print_error(file_info_pointer, WRONG_USE_CODE);
				
				return;
			}
			
			source_argument_string = collect_command_argument(line_pointer);
			
			if(!(NOT_END_OF_LINE(*line_pointer) && NOT_WHITE(*line_pointer)) || **line_pointer == ',')
			{
				if(!is_label(file_info_pointer, source_argument_string, FALSE))
				{
					free(source_argument_string);
				
					return;
				}
			}
			else
			{
				free(source_argument_string);
				
				print_error(file_info_pointer, STRING_TO_LONG);
				
				return;
			}
			
			free(source_argument_string);
			
			MOVE_TO_NOT_WHITE(*line_pointer)
			
			if(**line_pointer != ',' && NOT_END_OF_LINE(*line_pointer))
			{
				print_error(file_info_pointer, WRONG_USE_CODE);
				
				return;
			}
			/*no more labesls to check.*/
			else if(!NOT_END_OF_LINE(*line_pointer))
			{
				return;
			}
			
			(*line_pointer)++;
		}
	}
	else if(is_extern(string))
	{
		free(string);
		
		file_info_pointer->is_file_extern = TRUE;
		
		if(flag_save_label)
		{
			/*donst save label, prints warning.*/
			fprintf(stderr,"\nWarning: File: \"%s\" Line: %d. Use of label wont work\n",
			file_info_pointer->file_name, file_info_pointer->line_number);
		}
		
		if(!NOT_END_OF_LINE(*line_pointer)) /*dclared and no arguments*/
		{
			print_error(file_info_pointer, WRONG_USE_CODE);
				
			return;
		}
		
		while(NOT_END_OF_LINE(*line_pointer))
		{
			MOVE_TO_NOT_WHITE(*line_pointer)
			
			if(!NOT_END_OF_LINE(*line_pointer))
			{
				print_error(file_info_pointer, WRONG_USE_CODE);
				
				return;
			}
			
			source_argument_string = collect_command_argument(line_pointer);
			
			if(!(NOT_END_OF_LINE(*line_pointer) && NOT_WHITE(*line_pointer)) || **line_pointer == ',')/*string to long didnt collect all.*/
			{
				if(is_label(file_info_pointer, source_argument_string, FALSE))
				{
					save_label(file_info_pointer, source_argument_string, LABEL_IS_EXTERN);
				}
				else
				{
					free(source_argument_string);
				
					return;
				}
			}
			else
			{
				free(source_argument_string);
				
				print_error(file_info_pointer, STRING_TO_LONG);
				
				return;
			}
			
			free(source_argument_string);
			
			MOVE_TO_NOT_WHITE(*line_pointer)
			
			if(**line_pointer != ',' && NOT_END_OF_LINE(*line_pointer))
			{
				print_error(file_info_pointer, WRONG_USE_CODE);
				
				return;
			}
			
			/*no more labesls to check.*/
			else if(!NOT_END_OF_LINE(*line_pointer))
			{
				return;
			}
			
			(*line_pointer)++;
		}
	}
	else if((type = is_command(string)))
	{
		
		free(string);
		
		if(flag_save_label)
		{
			pointer_to_label = save_label(file_info_pointer, first_string, LABEL_IS_COMMAND);
			
			if(pointer_to_label != NULL)
			{
				pointer_to_label->label_is_struct = FALSE;
			}
		}
		
		MOVE_TO_NOT_WHITE(*line_pointer)
		
		switch(type){
			case IS_MOV:
			case IS_CMP:
			case IS_ADD:
			case IS_SUB:
			case IS_LEA:
				/*command with two arguments.*/
				
				if(!(NOT_END_OF_LINE(*line_pointer)))
				{
					/*not good needs to be first argument.*/
					print_error(file_info_pointer, WRONG_USE_CODE);
					
					return;
				}
				
				source_argument_string = collect_command_argument(line_pointer);
				
				MOVE_TO_NOT_WHITE(*line_pointer)
				
				if(**line_pointer != ',')
				{
					print_error(file_info_pointer, WRONG_USE_CODE);
					
					free(source_argument_string);
					
					return;
				}
				
				(*line_pointer)++;
				
				MOVE_TO_NOT_WHITE(*line_pointer)
				
				if(!(NOT_END_OF_LINE(*line_pointer)))/*if end not good needs to be second argument.*/
				{
					print_error(file_info_pointer, WRONG_USE_CODE);
					
					free(source_argument_string);
					
					return;
				}
				
				destination_argument_string = collect_command_argument(line_pointer);
				
				MOVE_TO_NOT_WHITE(*line_pointer)
				
				if(NOT_END_OF_LINE(*line_pointer))/*nothing more needs to be in the line.*/
				{
					print_error(file_info_pointer, TOO_MUCH_ARGUMENTS);
					
					free(source_argument_string);
				
					free(destination_argument_string);
					
					return;
				}
				
				procces_command_code(file_info_pointer, type, source_argument_string, destination_argument_string);
				
				free(source_argument_string);
				
				free(destination_argument_string);
				
				break;
				
			case IS_CLR:
			case IS_NOT:
			case IS_INC:
			case IS_DEC:
			case IS_JMP:
			case IS_BNE:
			case IS_GET:
			case IS_PRN:
			case IS_JSR:
				/*one argument command.*/
				
				if(!(NOT_END_OF_LINE(*line_pointer)))
				{
					print_error(file_info_pointer, WRONG_USE_CODE);
					
					return;
				}
				
				destination_argument_string = collect_command_argument(line_pointer);
				
				MOVE_TO_NOT_WHITE(*line_pointer)
				
				if(NOT_END_OF_LINE(*line_pointer))/*nothing more needs to be in the line.*/
				{
					print_error(file_info_pointer, TOO_MUCH_ARGUMENTS);
					
					free(destination_argument_string);
					
					return;
				}
				
				procces_command_code(file_info_pointer, type, NULL, destination_argument_string);
				
				free(destination_argument_string);
				
				break;
				
			default:
				/*command with no arguments.*/
				if(NOT_END_OF_LINE(*line_pointer))/*nothing needs to be in the line.*/
				{
					print_error(file_info_pointer, TOO_MUCH_ARGUMENTS);
					
					return;
				}
				
				procces_command_code(file_info_pointer, type, NULL, NULL);
				
			}/*end of switch*/
	}
	else/*not daataa or instruction */
	{
		free(string);
		
		print_error(file_info_pointer, UNRECOGNIZED_CODE);
		
		return;
	}

}
	
	
	
	
	
	
	








/**
 * Function that get a command and couple of arguments found in the the line of code and the function 
 * will sort and check it to see if its a good assembly command syntax or use.
 * @param file_info_pointer is a pointer to a pointer of the main struct that holdes all the information of the file.
 * @param type is the command type used int sorce line code.
 * @param source_argument_string is the string of the first argument that came after the command.
 * @param destination_argument_string is the string of the second argument that came after the command.
 */
static void procces_command_code(file_info *file_info_pointer, command_type type, char * source_argument_string,
							char * destination_argument_string)
{
	/*instruction will contain the command opcode and arguments types,argument_data will hold the data itself.*/
	command_data instruction = {0}, argument_data = {0};
	
	command_node * list_current_head;
	
	/*the number will be used to help with saving data or for struct number*/
	int number, number_reg;/*number_reg is if there is two reg use then ill use it.*/
	
	/*puting in the 6-9 bits the command opcode.*/
	instruction.split_opcode.opcode = type - 1; /*-1 for the machine interpetation code*/
	
	/*saving the unfinished data to make first node that is the opcode, and pointing to it to modify.*/
	list_current_head = save_command_data(file_info_pointer, instruction.combined_data);
	
	/*first argument procces, if there is one.*/
	if(source_argument_string != NULL)
	{
		/*handles number*/
		if(*source_argument_string == '#')
		{
			if(type == IS_LEA)
			{
				print_error(file_info_pointer, WRONG_USE_CODE);
				
				return;
			}
			
			source_argument_string++;
			
			/*checking and collection number.*/
			if(is_number(&source_argument_string, &number))
			{
				/*changing number to fit 8 bit */
				if(number > MAX_SIGNED_NUM)
				{
					number = MAX_MASK;     
				}
				else if(number < MIN_SIGNED_NUM)
				{
					number = MIN_MASK;
				}
				
				/*!not changing the opcode its set to 0 from the start.!*/
				
				argument_data.split_data.data = number;
				
				instruction.split_opcode.source = SORT_TYPE_IMIDIATE;
				
				save_command_data(file_info_pointer, argument_data.combined_data);
				
			}
			else
			{
				print_error(file_info_pointer, NOT_A_NUMBER);
				
				return;
			}
			
		}
		/*handles registers*/
		else if((number_reg = is_register(source_argument_string)))
		{
			/*checking if its a command that dosnt use registers.*/
			if(type == IS_LEA)
			{
				print_error(file_info_pointer, WRONG_USE_CODE);
				
				return;
			}
			/*checking if the user tryed to use an unusable register for this project.*/
			else if(number_reg == IS_PSW)
			{
				print_error(file_info_pointer, WRONG_USE_REGISTER);
				
				return;
			}
			
			/*all is good saving register.*/
			instruction.split_opcode.source = SORT_TYPE_REGISTER;
			
			argument_data.split_register.register_source = number_reg - 1;
			
			save_command_data(file_info_pointer, argument_data.combined_data);
			
		}
		/*handels struct*/
		else if(strlen(source_argument_string) > 2)
		{
			if(source_argument_string[strlen(source_argument_string) - 2] == '.')
			{
				number = source_argument_string[strlen(source_argument_string) - 1];
				
				number = number - '0';/*changing char value to real value.*/
				
				/*cuting the struct to use the label.*/
				source_argument_string[strlen(source_argument_string) - 2] = '\0';
				
				if(!is_label(file_info_pointer, source_argument_string, FALSE))/*reminder is_label handels errors too.*/
				{
					return;
				}
				
				/*as i understand only optsion in struct after . is 1 for number and 2 for char string.*/
				if(number != 1 && number != 2)
				{
					print_error(file_info_pointer,WRONG_USE_STRUCT);
					
					return;
				}
				
				instruction.split_opcode.source = SORT_TYPE_STRUCT;
				
				argument_data.split_data.data = number;
				
				/*all labels will be taken care of in the seconde pass.*/
				save_command_data(file_info_pointer, NAN);
				
				/*after making a node for the label of struct saving the acces number.*/
				save_command_data(file_info_pointer, argument_data.combined_data);
			}
			else
			{
				if(!is_label(file_info_pointer, source_argument_string, FALSE))
				{
					return;
				}
			
				instruction.split_opcode.source = SORT_TYPE_LABEL;
			
				save_command_data(file_info_pointer, NAN);
			}
		}
		/*only option left is label*/
		else 
		{
			if(!is_label(file_info_pointer, source_argument_string, FALSE))
			{
				return;
			}
			
			instruction.split_opcode.source = SORT_TYPE_LABEL;
			
			save_command_data(file_info_pointer, NAN);
		}
	}
	
	/*second argument checking and proccesing.*/
	
	if(destination_argument_string != NULL)
	{
		/*reseting the argument_data for new use.*/
		argument_data.combined_data = NAN;
		
		/*handles number*/
		if(*destination_argument_string == '#')
		{
			/*only comands that use in destanation imidiates is prn and cmp.*/
			if(type == IS_CMP || type == IS_PRN)
			{
				destination_argument_string++;
				
				/*checking and collection number.*/
				if(is_number(&destination_argument_string, &number))
				{
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
				
					/*#not changing the opcode its set to 0 from the start.#*/
					instruction.split_opcode.destination = SORT_TYPE_IMIDIATE;
					
					argument_data.split_data.data = number;
				    
					save_command_data(file_info_pointer, argument_data.combined_data);
				}
				else
				{
					print_error(file_info_pointer, NOT_A_NUMBER);
						
					return;
				}
			}
			else
			{
				print_error(file_info_pointer, WRONG_USE_CODE);
				
				return;
			}
		}
		/*handles registers*/
		else if((number = is_register(destination_argument_string)))
		{
			/*checking if the user tryed to use an unusable register for this project.*/
			if(number == IS_PSW)
			{
				print_error(file_info_pointer, WRONG_USE_REGISTER);
				
				return;
			}
			
			instruction.split_opcode.destination = SORT_TYPE_REGISTER;
			
			argument_data.split_register.register_destination = number - 1;
			
			/*if two reg instruction then updates the node alredy in the list.*/
			if(instruction.split_opcode.source == SORT_TYPE_REGISTER)
			{
				list_current_head->next->data += argument_data.combined_data;
			}
			else
			{
				save_command_data(file_info_pointer, argument_data.combined_data);
			}
		}
		/*handels struct*/
		else if(strlen(destination_argument_string) > 2)
		{
			if(destination_argument_string[strlen(destination_argument_string) -2] == '.')
			{
				number = destination_argument_string[(strlen(destination_argument_string) - 1)];
			
				number = number - '0';/*changing char value to real value*/
			
				/*cuting the struct to use the label.*/
				destination_argument_string[strlen(destination_argument_string) - 2] = '\0';
			
				if(!is_label(file_info_pointer, destination_argument_string, FALSE))
				{
					return;
				}
				
				if(number != 1 && number != 2)
				{
					print_error(file_info_pointer, WRONG_USE_STRUCT);
					
					return;
				}
				
				instruction.split_opcode.destination = SORT_TYPE_STRUCT;
				
				argument_data.split_data.data = number;
				
				/*all labels will be taken care of in the seconde pass.*/
				save_command_data(file_info_pointer, NAN);
				
				/*after making a node for the label of struct saving the acces number.*/
				save_command_data(file_info_pointer, argument_data.combined_data);
			}
			else
			{
				if(!is_label(file_info_pointer, destination_argument_string, FALSE))
				{
					return;
				}
				
				instruction.split_opcode.destination = SORT_TYPE_LABEL;
				
				save_command_data(file_info_pointer, NAN);
			}
		}
		/*only option left is label*/
		else 
		{
			if(!is_label(file_info_pointer, destination_argument_string, FALSE))
			{
				return;
			}
			
			instruction.split_opcode.destination = SORT_TYPE_LABEL;
			
			save_command_data(file_info_pointer, NAN);
		}
	}
	
	/*after all instruction was read and undarstood update the current head.*/
	list_current_head->data = instruction.combined_data;
	
}














