
#ifndef _GLOBALS_H
#define _GLOBALS_H

#define NUMBER_OF_DATATYPE 3
#define NUMBER_OF_INSTRUCTIONS 5
#define SPACE_FOR_EXTENSION 5
#define MEMORY_LOCATION_START 100
#define MAX_MACHINE_NUMBER 31
#define MAX_MACHINE_LENGTH 2
#define MAX_LINE_LENGTH 80
#define MAX_LABEL_LENGTH 30
#define MAX_COMMAND_LENGTH 7
#define MAX_INSTRUCT_LENGTH 8
#define MAX_REGISTER_LENGTH 4
#define MAX_NUMBER_OF_LINES 255
#define MIN_SIGNED_NUM -128
#define MAX_SIGNED_NUM 127
#define MAX_MASK 127
#define MIN_MASK 128
#define MINUS_MASK 0xff
#define NUMBER_OF_REGISTERS 9
#define NUMBER_OF_COMMANDS 16

/*a bool type enum.*/
typedef enum boolean { FALSE, TRUE } bool;

/*enum for the macro error the may encounter.*/
typedef enum macro_errors{

	SAME_MACRO_NAME = -2,
	
	FILE_ERROR = -1,
	
	SAVED_ASSEMBLY_NAME = -3
	
} macro_errors;

/*struct of macro for the linked list, all data needed to use in macro.*/
typedef struct macro_node{
	
	char * macro_name;
	
	char * macro_data;
	
	unsigned int macro_size;
	
	struct macro_node * next;

} macro_node;

/*struct fo a coomand for the linked list of commands.*/
typedef struct command_node{
	
	unsigned int data;
	
	struct command_node * next;
	
} command_node;

/*enum for the a.r.e in the op code and commands.*/
typedef enum ARE{
	
	ABSOLUTE,
	
	EXTERNAL,
	
	RELOCATABLE
	
}ARE;

/*union that will be saved the daa and used to translate to 32 base numbers.*/
typedef union machine_language{
	
	struct{
		unsigned int first:  5;
		unsigned int second: 5;
	}split;
	
	unsigned int combined;
} machine_language;

/*union of data, registers, opcode, its for translating it to a single int data.*/
typedef union command_data{

	struct{
		unsigned int are: 2;
		unsigned int data: 8;
	} split_data;
	struct{
		unsigned int are: 2;
		unsigned int register_destination: 4;
		unsigned int register_source: 4;
	} split_register;
	struct{
	  	unsigned int are: 2;
	  	unsigned int destination: 2; 
		unsigned int source: 2;
		unsigned int opcode: 4;
	} split_opcode;
	
	unsigned int combined_data;
		
} command_data;

/*enum of sort type that had been used in the opcode.*/
typedef enum command_sort_type{
	
	SORT_TYPE_IMIDIATE = 0,
	
	SORT_TYPE_LABEL = 1,
	
	SORT_TYPE_STRUCT = 2,
	
	SORT_TYPE_REGISTER = 3
	
} command_sort_type;

/*enum nubers for register.*/
typedef enum register_type{
	
	IS_R0 = 1, IS_R1, IS_R2, IS_R3, IS_R4,
	
	IS_R5, IS_R6, IS_R7, IS_PSW,
	
	NAN = 0
	
} register_type;

/*enum number for commands.*/
typedef enum command_type{

	
	IS_MOV = 1, IS_CMP, IS_ADD, IS_SUB, IS_NOT, IS_CLR,
	
	IS_LEA, IS_INC, IS_DEC, IS_JMP, IS_BNE, IS_GET, 
	
	IS_PRN, IS_JSR, IS_RTS, IS_HLT
	
	
} command_type;

/*a struct for the linked list od datas.*/
typedef struct data_node
{
	unsigned int data;
	
	struct data_node *next;
	
} data_node;

/*enum fo type of data is used*/
typedef enum data_type{
	
	IS_DATA = 1,
	
	IS_STRING ,
	
	IS_STRUCT 
	
} data_type;
	
/*enum for the label types there is.*/
typedef enum label_type{
	
	LABEL_IS_DATA,
	
	LABEL_IS_ENTRY,
	
	LABEL_IS_EXTERN,
	
	LABEL_IS_COMMAND
	
} label_type;

/*struct for the linked list of labels.*/
typedef struct label_node{
	
	char * label_name;
	
	unsigned int label_addres;
	
	label_type type;
	
	bool label_is_struct;
	
	struct label_node * next;
	
}label_node;

/*struct for the linked list of extern use.*/
typedef struct extern_node{

	char * label_name;

	unsigned int addres;
	
	struct extern_node * next;
	
} extern_node;


/*the main struct that holdes all the lists and state of file.*/
typedef struct file_info{

	char * file_name;
	
	int IC;
	
	int DC;
	
	int line_number;
	
	label_node * list_head_label;
	
	data_node * list_head_data;
	
	data_node * list_tail_data;
	
	command_node * list_head_command;
	
	command_node * list_tail_command;
	
	extern_node * list_head_extern;
	
	extern_node * list_tail_extern;
	
	bool is_file_good;
	
	bool is_file_entry;
	
	bool is_file_extern;
	
} file_info;


/*enum for error type that could be found in the assembly.*/
typedef enum error_type{

	LINE_TOO_LONG,
	
	BAD_LABEL_NAME,
	
	SAME_LABEL_NAME,
	
	ERROR_READING_FILE,
	
	OVER_SIZE_LIMIT,
	
	WRONG_LABEL_USE,
	
	NOT_A_NUMBER,
	
	NOT_A_STRING,
	
	TOO_MUCH_ARGUMENTS,
	
	UNRECOGNIZED_CODE,
	
	WRONG_USE_CODE,
	
	WRONG_USE_REGISTER,
	
	WRONG_USE_STRUCT,
	
	ENTRY_EXTERN_DUP,
	
	ENTRY_ENTRY_DUP,
	
	LABEL_NOT_FOUND,
	
	SAVED_LABEL_NAME,
	
	STRING_TO_LONG,
	
	WRONG_USE_LABEL
	
} error_type;


#endif
