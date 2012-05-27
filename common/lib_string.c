#include <stdlib.h>

#include <my_ctype.h>
#include "lib_string.h"

//-------------------------------------------------------------------------------
//	static variable declaration
//-------------------------------------------------------------------------------
static int8s_t COM_STACK[COMMAND_STACK_SIZE][64];
static int8s_t Stack_Ptr = 0;

//-------------------------------------------------------------------------------
void init_lib_string()
{
	int32u_t i;
	int8s_t string[6][16], *token[6];
	
	for(i=0;i<6;i++) token[i]=string[i];
}
//-------------------------------------------------------------------------------
int16u_t tokenize(int8s_t **token, int8s_t *string)
{
    int16u_t si=0, di=0, num=0;
    int8s_t temp;
    
    while((temp=string[si++])!='\0')
	{
        if(temp==CHAR_SPACE || temp==CHAR_TAB)
		{
            *(*(token+num)+di)='\0';
            num++;
            di=0;
        }
		else
		{
            *(*(token+num)+di++)=temp;
        }
    }
    *(*(token+num)+di)='\0';
    num++;
    
    return num;
}

//-------------------------------------------------------------------------------
void int2str(int8s_t *str, int32s_t value, int32s_t radix)
{
	int8s_t DigitMap[]="0123456789abcdef", tmpChar;
	int32s_t a, len;

	int8s_t *start=str;
	int32s_t quotient=value;

	do{
		*str=DigitMap[(int32s_t)(quotient%radix)];
		str++;
		quotient/=radix;
	}while(quotient);
	
	*str='\0';
	str=start;
	len=strlen(str);
	
	for(a=0;a<len/2;a++){
		tmpChar=str[a];
		str[a]=str[len-1-a];
		str[len-1-a]=tmpChar;
	}
}
//-------------------------------------------------------------------------------
int32u_t str2int(int8s_t *str, int32u_t radix)
{
	int32u_t result = 0,value;

	radix = 10;
	if (*str == '0') {
		str++;
		if ((*str == 'x') && isxdigit(str[1])) 
		{
			radix = 16;
			str++;
		}
		else
		{
			radix = 10;
		}
	}

	while ( isxdigit(*str) && (value = isdigit(*str)? *str-'0' : (islower(*str)? Toupper(*str) : *str)-'A'+10) < radix )
   	{
		result = result*radix + value;
		str++;
	}

	return result;
}

void com_stack_push(int8s_t* command)
{
    strcpy(COM_STACK[Stack_Ptr], command);

	if(Stack_Ptr == COMMAND_STACK_SIZE-1)
		Stack_Ptr = 0; // Circulation
	else
		Stack_Ptr++;
}

void com_stack_pop(int8s_t* Dest, int8s_t KeyID)
{
	if(KeyID == CHAR_UP)
	{
		if(Stack_Ptr-1 < 0) // Circulation
		{	
			Stack_Ptr = COMMAND_STACK_SIZE-1;
			strcpy(Dest, COM_STACK[Stack_Ptr]);					
		}
		else
		{
			Stack_Ptr--;
			strcpy(Dest, COM_STACK[Stack_Ptr]);			
		}
	}
	else if(KeyID == CHAR_DOWN)
	{
		if(Stack_Ptr == COMMAND_STACK_SIZE-1) // Circulation
		{	
			Stack_Ptr = 0;
			strcpy(Dest, COM_STACK[Stack_Ptr]);		
		}
		else
		{
			Stack_Ptr++;
			strcpy(Dest, COM_STACK[Stack_Ptr]);			
		}				
	}
	else
	{
	}
}

