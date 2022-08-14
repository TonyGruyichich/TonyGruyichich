#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"
#include "code.h"

/**
 * Function that gets a string that is an entry and checks if the arguments are label that have been declared
 * in the sorce file.
 * @param file_info_pointer is the main struct that holdes the linked list of labels.
 * @param line_pointer is the pointer to the current line bing checked for entry labels.
 */
static void process_entry_labels(file_info * file_info_pointer, char ** line_pointer);

/**
 * Function that is called whrn in the source file a command is encounterd and need to be processed 
 * to find if there was a label used and will check if said label is a declared label, 
 * it uses the made data in the linked list of commands of the specific line opcode and checks if 
 * there is a sort of label or a sort of struct.
 * @param file_info_pointer holds the linked list of command data and used for more things like error handaling..
 * @param list_command_pointer is a pointer to a pointer of current command bing checked.
 * @param line_pointer is a pointer to a pointer of string  of the line of command code in the file.
 * @param type is the type of command used.
 */
static void process_command_labels(file_info * file_info_pointer, command_node ** line_counter_pointer, 
									char ** line_pointer, command_type type);

/**
 * second_pass is a function that starts the second checking of the source assembly code, it 
 * will check the entry instruction and updated the label used in commands, if it encouters an error
 * it will keep going to find more errors.
 * @param fp is a file pointer to the source code.
 * @param file_info_pointer is a pointer to the file struct that holdes vital information and linekd list.
 * @return bool TRUE if source code is writen good and no problems encounterd, FALSE if problems encounterd.
 */
bool second_pass(FILE * fp, file_info * file_info_pointer)
{
	char array_line[MAX_LINE_LENGTH +1];
	
	char *line_pointer, * string_pointer;
	
	int * line_counter_pointer, type;
	
	command_node * list_command_pointer;
	
	
	list_command_pointer = file_info_pointer->list_head_command;
	
	line_counter_pointer = &file_info_pointer->line_number;
	
	*line_counter_pointer = 1;
	
	line_pointer = fgets(array_line, MAX_LINE_LENGTH +1, fp);
	
	/*loops till end of file.*/
	while(line_pointer != NULL)
	{
		if(*line_pointer != ';')
		{
			MOVE_TO_NOT_WHITE(line_pointer)
			
			if(NOT_END_OF_LINE(line_pointer))
			{
				string_pointer = collect_string(&line_pointer);
				
				if(string_pointer[strlen(string_pointer) -1] != ':')/*checks if label.*/
				{
					if(is_entry(string_pointer))
					{
						process_entry_labels(file_info_pointer, &line_pointer);
					}
					else if((type = is_command(string_pointer)))
					{
						process_command_labels(file_info_pointer, &list_command_pointer, &line_pointer, type);
					}
					
					free(string_pointer);
				}
				else /*skips label and checks what is after.*/
				{
					free(string_pointer);
					
					MOVE_TO_NOT_WHITE(line_pointer)
					
					if(NOT_END_OF_LINE(line_pointer))
					{
						string_pointer = collect_string(&line_pointer);
						
						if(is_entry(string_pointer))
						{
							process_entry_labels(file_info_pointer, &line_pointer);
						}
						else if((type = is_command(string_pointer)))
						{
							process_command_labels(file_info_pointer, &list_command_pointer, &line_pointer, type);
						}
						
						free(string_pointer);
					}
				}
				/*if all 80 char are writen then needs get one to jump line in source.*/
				if(*line_pointer == '\0')
				{
					fgetc(fp);
					
					if(ferror(fp))
					{
						print_error(file_info_pointer, ERROR_READING_FILE);
		
						return FALSE;
					}
				}
				
			}
		}
		/*gets next line.*/
		line_pointer = fgets(array_line, MAX_LINE_LENGTH + 1, fp);
		
		(*line_counter_pointer)++;
	}
	
	/*check if fgets or fgetc had an error.*/
	if(ferror(fp))
	{
		print_error(file_info_pointer, ERROR_READING_FILE);
		
		return FALSE;
	}
	
	return file_info_pointer->is_file_good;
}

/**
 * Function that gets a string that is an entry and checks if the arguments are label that have been declared
 * in the sorce file.
 * @param file_info_pointer is the main struct that holdes the linked list of labels.
 * @param line_pointer is the pointer to the current line bing checked for entry labels.
 */
static void process_entry_labels(file_info * file_info_pointer, char ** line_pointer)
{
	char * argument_string;
	
	bool found_label;
	
	label_node * current_node;
	
	/*runs till end of line then returns.*/
	while(TRUE)
	{
		MOVE_TO_NOT_WHITE(*line_pointer)
		
		argument_string = collect_command_argument(line_pointer);
		
		current_node = file_info_pointer->list_head_label;
		
		/*loop to find label with same name.*/
		while(current_node != NULL)
		{
			found_label = FALSE;
			
			if(!strcmp(argument_string, current_node->label_name))
			{
				if(current_node->type == LABEL_IS_EXTERN)
				{
					print_error(file_info_pointer, ENTRY_EXTERN_DUP);
				}
				else if(current_node->type == LABEL_IS_ENTRY)
				{
					print_error(file_info_pointer, ENTRY_ENTRY_DUP);
				}
				else
				{
					current_node->type = LABEL_IS_ENTRY;
				}
				
				found_label = TRUE;
				
				current_node = NULL;
			}
			else
			{
				current_node = current_node->next;
			}
		}
		
		if(!found_label)
		{
			print_error(file_info_pointer, LABEL_NOT_FOUND);
		}
		
		free(argument_string);
		
		MOVE_TO_NOT_WHITE(*line_pointer)
		
		/*no more labesls to check.*/
		if(!NOT_END_OF_LINE(*line_pointer))
		{
			return;
		}
			
		(*line_pointer)++;
	}
}

/**
 * Function that is called whrn in the source file a command is encounterd and need to be processed 
 * to find if there was a label used and will check if said label is a declared label, 
 * it uses the made data in the linked list of commands of the specific line opcode and checks if 
 * there is a sort of label or a sort of struct.
 * @param file_info_pointer holds the linked list of command data and used for more things like error handaling..
 * @param list_command_pointer is a pointer to a pointer of current command bing checked.
 * @param line_pointer is a pointer to a pointer of string  of the line of command code in the file.
 * @param type is the type of command used.
 */
static void process_command_labels(file_info * file_info_pointer, command_node ** list_command_pointer, 
									char ** line_pointer, command_type type)
{
	/*unins to use bit fields.*/
	command_data command_opcode = {0}, label_data = {0};
	
	label_node * current_label_node;
	
	char * argument_string;
	
	static unsigned int ic;
	
	/*puting the opcode in*/
	command_opcode.combined_data = (*list_command_pointer)->data;
	
	MOVE_TO_NOT_WHITE(*line_pointer)
	
	switch(type){
		case IS_MOV:
		case IS_CMP:
		case IS_ADD:
		case IS_SUB:
		case IS_LEA:
			/*command with two arguments checking for label*/
			
			argument_string = collect_command_argument(line_pointer);
				
			(*list_command_pointer) = (*list_command_pointer)->next;/*pointing on command node with data.*/
			
			ic++; /*countin place of instruction.*/
			
			/*checks if label.*/
			if(command_opcode.split_opcode.source == SORT_TYPE_LABEL ||
			 command_opcode.split_opcode.source == SORT_TYPE_STRUCT)
			{
				/*cuts the label from the struct.*/
				if( command_opcode.split_opcode.source == SORT_TYPE_STRUCT)
				{
					argument_string[strlen(argument_string) -2] = '\0';
				}
				
				current_label_node = file_info_pointer->list_head_label;
					
				/*loop to find the saved label.*/
				while(current_label_node != NULL && strcmp(current_label_node->label_name, argument_string))
				{
					current_label_node = current_label_node->next;
				}
				
				/*check if not end of list.*/
				if(current_label_node != NULL)
				{
					/*checking if writen label to struct and didnt add number in source.*/
					if(current_label_node->label_is_struct)
					{
						if((command_opcode.split_opcode.source != SORT_TYPE_STRUCT) &&
						 current_label_node->type != LABEL_IS_EXTERN)
						{
							print_error(file_info_pointer, WRONG_USE_STRUCT);
						}
					}
					else if(command_opcode.split_opcode.source == SORT_TYPE_STRUCT)
					{
						print_error(file_info_pointer, WRONG_USE_LABEL);
					}
					
					if(current_label_node->type == LABEL_IS_EXTERN)
					{
						save_extern_use(file_info_pointer, argument_string, ic);
					}
					
					/*save in bit 2-9 the addres*/
					label_data.split_data.data = current_label_node->label_addres;
					
					/*check what type of ARE to save in bit 0-1.*/
					if(current_label_node->type == LABEL_IS_EXTERN)
					{
						label_data.split_data.are = EXTERNAL;
					}
					else
					{
						label_data.split_data.are = RELOCATABLE;
					}
					
					/*updates the addres.*/
					(*list_command_pointer)->data = label_data.combined_data;
				}
				else
				{
					print_error(file_info_pointer, LABEL_NOT_FOUND);
				}
				
				/*if struct needs to jump another node.*/
				if(command_opcode.split_opcode.source == SORT_TYPE_STRUCT)
				{
					(*list_command_pointer) = (*list_command_pointer)->next;
					
					ic++;
				}
			}
				
				
			free(argument_string);
			
			/*need to check if both are registers. 2 use one node.*/
			if(!(command_opcode.split_opcode.source == SORT_TYPE_REGISTER && 
			command_opcode.split_opcode.destination == SORT_TYPE_REGISTER))
			{
				(*list_command_pointer) = (*list_command_pointer)->next;
				
				ic++;
			}
			
				
			/*seconde argument label check.*/
				
			MOVE_TO_NOT_WHITE(*line_pointer)
				
			/*jumps the ',' between the arguments.*/
			(*line_pointer)++; 
				
			MOVE_TO_NOT_WHITE(*line_pointer)
				
			argument_string = collect_command_argument(line_pointer);
			
			
			/*same as before just the destination argument.*/	
			if(command_opcode.split_opcode.destination == SORT_TYPE_LABEL ||
			 command_opcode.split_opcode.destination == SORT_TYPE_STRUCT)
			{
				if(command_opcode.split_opcode.destination == SORT_TYPE_STRUCT)
				{
					argument_string[strlen(argument_string) -2] = '\0';
				}
				
				current_label_node = file_info_pointer->list_head_label;
					
				/*loop to find the saved label.*/
				while(current_label_node != NULL && strcmp(current_label_node->label_name, argument_string))
				{
					current_label_node = current_label_node->next;
				}
				
				/*check if not end of list.*/
				if(current_label_node != NULL)
				{
					/*checking if writen label to struct and didnt add number in source.*/
					if(current_label_node->label_is_struct)
					{
						if((command_opcode.split_opcode.destination != SORT_TYPE_STRUCT) &&
						 current_label_node->type != LABEL_IS_EXTERN)
						{
							print_error(file_info_pointer, WRONG_USE_STRUCT);
						}
					}
					else if(command_opcode.split_opcode.destination == SORT_TYPE_STRUCT)
					{
						print_error(file_info_pointer, WRONG_USE_LABEL);
					}
					
					if(current_label_node->type == LABEL_IS_EXTERN)
					{
						save_extern_use(file_info_pointer, argument_string, ic);
					}
					
					/*save in bit 2-9 the addres*/
					label_data.split_data.data = current_label_node->label_addres;
					
					/*check what type of ARE to save in bit 0-1.*/
					if(current_label_node->type == LABEL_IS_EXTERN)
					{
						label_data.split_data.are = EXTERNAL;
					}
					else
					{
						label_data.split_data.are = RELOCATABLE;
					}
						
					/*updates the addres.*/
					(*list_command_pointer)->data = label_data.combined_data;
				}
				else
				{
					print_error(file_info_pointer, LABEL_NOT_FOUND);
				}
				
				/*if struct needs to jump another node.*/
				if(command_opcode.split_opcode.destination == SORT_TYPE_STRUCT)
				{
					(*list_command_pointer) = (*list_command_pointer)->next;
					
					ic++;
				}
			}
			
			free(argument_string);
			
			/*looks at the next opcode node.*/
			(*list_command_pointer) = (*list_command_pointer)->next;
			
			ic++;
			
			/*sets the pointer to the end of the array.*/
			MOVE_TO_NOT_WHITE(*line_pointer)
			
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
				
			argument_string = collect_command_argument(line_pointer);
			
			(*list_command_pointer) = (*list_command_pointer)->next;
			
			ic++;
			
			/*same as before just the destination argument.*/	
			if(command_opcode.split_opcode.destination == SORT_TYPE_LABEL || 
			command_opcode.split_opcode.destination == SORT_TYPE_STRUCT)
			{
				if(command_opcode.split_opcode.destination == SORT_TYPE_STRUCT)
				{
					argument_string[strlen(argument_string) -2] = '\0';
				}
				
				current_label_node = file_info_pointer->list_head_label;
					
				/*loop to find the saved label.*/
				while(current_label_node != NULL && strcmp(current_label_node->label_name, argument_string))
				{
					current_label_node = current_label_node->next;
				}
				
				/*check if not end of list.*/
				if(current_label_node != NULL)
				{
					/*checking if writen label to struct and didnt add number in source.*/
					if(current_label_node->label_is_struct)
					{
						if((command_opcode.split_opcode.destination != SORT_TYPE_STRUCT) &&
						 current_label_node->type != LABEL_IS_EXTERN)
						{
							print_error(file_info_pointer, WRONG_USE_STRUCT);
						}
					}
					else if(command_opcode.split_opcode.destination == SORT_TYPE_STRUCT)
					{
						print_error(file_info_pointer, WRONG_USE_LABEL);
					}
					
					/*adding a location of used external label.*/
					if(current_label_node->type == LABEL_IS_EXTERN)
					{
						save_extern_use(file_info_pointer, argument_string, ic);
					}
					
					/*save in bit 2-9 the addres*/
					label_data.split_data.data = current_label_node->label_addres;
					
					/*check what type of ARE to save in bit 0-1.*/
					if(current_label_node->type == LABEL_IS_EXTERN)
					{
						label_data.split_data.are = EXTERNAL;
					}
					else
					{
						label_data.split_data.are = RELOCATABLE;
					}
						
					/*updates the addres.*/
					(*list_command_pointer)->data = label_data.combined_data;
				}
				else
				{
					print_error(file_info_pointer, LABEL_NOT_FOUND);
				}
				
				/*if struct needs to jump another node.*/
				if(command_opcode.split_opcode.destination == SORT_TYPE_STRUCT)
				{
					(*list_command_pointer) = (*list_command_pointer)->next;
					
					ic++;
				}
			}
			
			free(argument_string);
			
			/*looks at the next opcode node.*/
			(*list_command_pointer) = (*list_command_pointer)->next;
			
			ic++;
			
			/*sets the pointer to the end of the array.*/
			MOVE_TO_NOT_WHITE(*line_pointer)
			
			break;
				
		default:
				/*command with no arguments look at the next command node with opcode.*/
				(*list_command_pointer) = (*list_command_pointer)->next;
				
				ic++;
				
	}/*end of switch*/
}














