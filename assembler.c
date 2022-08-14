#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "preprocessor.h"
#include "first_pass.h"
#include "second_pass.h"
#include "code.h"

/**
 * Processes a single assembly source file, will send errors if found to stderr.
 * @param filename The filename, without it's extension.
 */
static void process_file(char *fileName);

/**
 * Entry point - 10bit assembler.
 */
int main(int argc, char **argv)
{
	int i;
	
	if(argc == 1)
	{
		printf("\nTo use the assembler add an assembly source file name to this command.\n");
		return 0;
	}
	
	for(i = 1; i < argc; i++)
	{
		process_file(argv[i]);
	}
	
	return 0;
}

/**
 * Processes a linked list with assembly commands and data, makes a file with the file name and puts 
 * the machine language in the file that was interpret.
 * @param fp The File to put the machine language.
 * @param file_info_pointer a pointer to struct of file info that holdes the linked list to be interpret.
 */
static void make_file_object(FILE * fp, file_info * file_info_pointer);

/**
 * Processes a linked list that hold label data, makes a file with the file name and puts 
 * the label name that is been declared entry in the file and after it the addres.
 * @param fp The File to put the label name and addres.
 * @param file_info_pointer a pointer to struct of file info that holdes the linked list to be processed.
 */
static void make_file_entry(FILE * fp, file_info * file_info_pointer);

/**
 * Processes a linked list that hold label data, makes a file with the file name and puts every time
 * the label name that is declared extern had been used in the file and after it the addres it was used..
 * @param fp The File to put the label name and addres.
 * @param file_info_pointer a pointer to struct of file info that holdes the linked list to be processed.
 */
static void make_file_extern(FILE * fp, file_info * file_info_pointer);

/**
 * Frees all the linked lists that had been allocated.
 * @param pointer a pointer to struct of file info that holdes the linked list to be interpret.
 */
static void free_file_info(file_info * pointer);

/**
 * Processes a single assembly source file, will send errors if found to stderr.
 * @param filename The filename, without it's extension.
 */
static void process_file(char *file_name)
{
	FILE *fp;
	
	char * full_file_name;
	
	file_info * file_info_pointer;
	
	
	file_info_pointer = (file_info *) calloc(1, sizeof(file_info));/*allot of things need to be zero thats why calloc.*/
	
	full_file_name = (char *) allocate_and_check(sizeof(char) * (strlen(file_name)+SPACE_FOR_EXTENSION));
	
	
	strcpy(full_file_name, file_name);
	
	strcat(full_file_name, ".as");
	
	fp = fopen(full_file_name, "r");
	
	if(fp == NULL)
	{
		printf("Error: file \"%s\" is inaccessible for reading.\n", full_file_name);
		
		free(full_file_name);
		
		free(file_info_pointer);
		
		return;
	}
	
	if(!preprocessor(fp, file_name))
	{
		return;
	}
	
	
	
	strcpy(full_file_name, file_name);
	
	strcat(full_file_name, ".am");
	
	fp = fopen(full_file_name, "r");
	
	if(fp == NULL)
	{
		printf("Error: file \"%s\" is inaccessible for reading.\n", full_file_name);
		
		free(full_file_name);
		
		free(file_info_pointer);
		
		return;
	}
	
	file_info_pointer->file_name = full_file_name;
	
	/*if no error do second part.*/
	if(first_pass(fp, file_info_pointer))
	{
		rewind(fp);
		
		if(second_pass(fp, file_info_pointer))
		{
			/*starting procedure to make .ob file.*/
			strcpy(full_file_name, file_name);
			
			strcat(full_file_name, ".ob");
			
			fp = fopen(full_file_name, "w");
			if(fp == NULL)
			{
				printf("Error: file \"%s\" is inaccessible for writting.\n", full_file_name);	
			}
			
			make_file_object(fp, file_info_pointer);
				
			if(file_info_pointer->is_file_entry)
			{
				/*starting procedure to make .ent file.*/
				strcpy(full_file_name, file_name);
			
				strcat(full_file_name, ".ent");
		
				fp = fopen(full_file_name, "w");
				if(fp == NULL)
				{
					printf("Error: file \"%s\" is inaccessible for writting.\n", full_file_name);	
				}
		
				make_file_entry(fp, file_info_pointer);
			}
			
			if(file_info_pointer->is_file_extern)
			{	
				/*starting procedure to make .ent file.*/
				strcpy(full_file_name, file_name);
			
				strcat(full_file_name, ".ext");
			
				fp = fopen(full_file_name, "w");
				if(fp == NULL)
				{
					printf("Error: file \"%s\" is inaccessible for writting.\n", full_file_name);	
				}
			
				make_file_extern(fp, file_info_pointer);
			}
		}
	}
	
	/*no matter what making sure free all alocated data.*/
	free_file_info(file_info_pointer);
	
	return;
}

/**
 * Frees all the linked lists that had been allocated.
 * @param pointer a pointer to struct of file info that holdes the linked list to be interpret.
 */
static void free_file_info(file_info * pointer)
{
	void * temp;
	
	free(pointer->file_name);
	
	while(pointer->list_head_label != NULL)
	{
		free(pointer->list_head_label->label_name);
		
		temp = pointer->list_head_label;
		
		pointer->list_head_label = pointer->list_head_label->next;
		
		free((label_node *) temp);
	}
	
	while(pointer->list_head_data != NULL)
	{
		temp = pointer->list_head_data;
		
		pointer->list_head_data = pointer->list_head_data->next;
		
		free((data_node *) temp);
	}
	
	while(pointer->list_head_command != NULL)
	{
		temp = pointer->list_head_command;
		
		pointer->list_head_command = pointer->list_head_command->next;
		
		free((command_node *) temp);
	}
	
	while(pointer->list_head_extern != NULL)
	{
		free(pointer->list_head_extern->label_name);
		
		temp = pointer->list_head_extern;
		
		pointer->list_head_extern = pointer->list_head_extern->next;
		
		free((extern_node *) temp);
	}
}

/**
 * Function that translates 10 bit word of a binary data to a 32 based data.
 *
 * @param pointer a pointer to array of chars that will holde interpreted 32 base data, needs to be 3 char length.
 * @param data a union that holds the data to interpret.
 */
static void interpeter(char * pointer, machine_language data);

/**
 * Processes a linked list that hold label data, makes a file with the file name and puts every time
 * the label name that is declared extern had been used in the file and after it the addres it was used..
 * @param fp The File to put the label name and addres.
 * @param file_info_pointer a pointer to struct of file info that holdes the linked list to be processed.
 */
static void make_file_extern(FILE * fp, file_info * file_info_pointer)
{
	char addres[MAX_MACHINE_LENGTH + 1];
	
	extern_node * current_node;
	
	machine_language bit = {0};
	
	
	current_node = file_info_pointer->list_head_extern;
	
	while(current_node != NULL)
	{
		bit.combined = current_node->addres + MEMORY_LOCATION_START;
		
		interpeter(addres, bit);
		
		fprintf(fp, "%s %s\n", current_node->label_name, addres);
		
		current_node = current_node->next;
	}
}

/**
 * Processes a linked list that hold label data, makes a file with the file name and puts 
 * the label name that is been declared entry in the file and after it the addres.
 * @param fp The File to put the label name and addres.
 * @param file_info_pointer a pointer to struct of file info that holdes the linked list to be processed.
 */
static void make_file_entry(FILE * fp, file_info * file_info_pointer)
{
	char addres[MAX_MACHINE_LENGTH + 1];	
	
	label_node * current_node;
	
	machine_language bit = {0};
	
	
	current_node = file_info_pointer->list_head_label;
	
	while(current_node != NULL)
	{
		if(current_node->type == LABEL_IS_ENTRY)
		{
			bit.combined = current_node->label_addres + MEMORY_LOCATION_START;
		
			interpeter(addres, bit);
			
			fprintf(fp, "%s %s\n", current_node->label_name, addres);
		}
		
		current_node = current_node->next;
	}
}

/**
 * Processes a linked list with assembly commands and data, makes a file with the file name and puts 
 * the machine language in the file that was interpret.
 * @param fp The File to put the machine language.
 * @param file_info_pointer a pointer to struct of file info that holdes the linked list to be interpret.
 */
static void make_file_object(FILE * fp, file_info * file_info_pointer)
{
	/*two machine arrays for the prints.*/
	char first[MAX_MACHINE_LENGTH + 1], second[MAX_MACHINE_LENGTH + 1];
	
	unsigned int i;
	/*nodes that will beused to fetch the data.*/
	command_node * command_node;
	
	data_node * instruct_node;

	machine_language bit_first = {0}, bit_second = {0};
	
	
	bit_first.combined = file_info_pointer->IC;
	
	bit_second.combined = file_info_pointer->DC;
	
	interpeter(first, bit_first);
	
	interpeter(second, bit_second);
	
	
	/*this setups the title numbers.*/
	if(bit_first.combined <= MAX_MACHINE_NUMBER)
	{
		first[0] = first[1];
		first[1] = '\0';
	}
	if(bit_second.combined <= MAX_MACHINE_NUMBER)
	{
		second[0] = second[1];
		second[1] = '\0';
	}
	/*print title.*/
	fprintf(fp, "%s %s\n",first, second);
	
	
	command_node = file_info_pointer->list_head_command;
	
	i = MEMORY_LOCATION_START;
	
	while(command_node != NULL)
	{
		/*process to compute the addres and code.*/
		bit_first.combined = i;
		
		bit_second.combined = command_node->data;
		
		interpeter(first, bit_first);
		
		interpeter(second, bit_second);
		
		fprintf(fp, "%s %s\n",first, second);
		
		i++;
		
		command_node = command_node->next;
	}
	
	
	instruct_node = file_info_pointer->list_head_data;
	
	
	while(instruct_node != NULL)
	{
		/*same as before just for the instructions.*/
		bit_first.combined = i;
		
		bit_second.combined = instruct_node->data;
		
		interpeter(first, bit_first);
		
		interpeter(second, bit_second);
		
		fprintf(fp, "%s %s\n",first, second);
		
		i++;
		
		instruct_node = instruct_node->next;
	}
}

/**
 * Function that translates 10 bit word of a binary data to a 32 based data.
 * @param pointer a pointer to array of chars that will holde interpreted 32 base data, needs to be 3 char length.
 * @param data a union that holds the data to interpret.
 */
static void interpeter(char * pointer, machine_language data)
{
	static const char machine_number[] = "!@#$%^&*<>abcdefghijklmnopqrstuv";
	
	
	*pointer++ = machine_number[data.split.second];
	
	
	*pointer++ = machine_number[data.split.first];
	
	*pointer = '\0';
}