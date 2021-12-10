// ConsoleApplication1.cpp : Defines the entry point for the console application.
//
// Name: Vivian Martinez
// Class: CDA 3101
// Assignment: Asg 2 ( an assembler of a subset of the MIPS assembly language
// 
// Compile: "g++ -g assem.cpp"
///////////////////////////////////////////////////////////////////////////////////



#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define BUFFERSIZE                80
#define NUMBER_REGISTERS          32
#define NUMBER_DIRECTIVES          4
#define NUMBER_R_INSTRUCTIONS     10
#define NUMBER_I_INSTRUCTIONS      5
#define NUMBER_J_INSTRUCTIONS      1
#define NUMBER_INSTRUCTIONS       16
#define MAX_INSTRUCTIONS         800
#define MAX_LABEL_SIZE            10


typedef struct reg
{
	int  dec_value;
	char name[6];
	char bin_value[6];
} reg;

reg regs[NUMBER_REGISTERS] =
{
	{  0, "$zero", "00000" },
	{  1, "$at",   "00001" },
    {  2, "$v0",   "00010" },
    {  3, "$v1",   "00011" },
    {  4, "$a0",   "00100" },
    {  5, "$a1",   "00101" },
    {  6, "$a2",   "00110" },
    {  7, "$a3",   "00111" },
	{  8, "$t0",   "01000" },
	{  9, "$t1",   "01001" },
	{ 10, "$t2",   "01010" },
	{ 11, "$t3",   "01011" },
	{ 12, "$t4",   "01100" },
	{ 13, "$t5",   "01101" },
	{ 14, "$t6",   "01110" },
	{ 15, "$t7",   "01111" },
	{ 16, "$s0",   "10000" },
	{ 17, "$s1",   "10001" },
	{ 18, "$s2",   "10010" },
	{ 19, "$s3",   "10011" },
	{ 20, "$s4",   "10100" },
	{ 21, "$s5",   "10101" },
	{ 22, "$s6",   "10110" },
	{ 23, "$s7",   "10111" },
    { 24, "$t8",   "11000" },
    { 25, "$t9",   "11001" },
    { 26, "$k0",   "11010" },
    { 27, "$k1",   "11011" },
    { 28, "$gp",   "11100" },
    { 29, "$sp",   "11101" },
    { 30, "$fp",   "11110" },
    { 31, "$ra",   "11111" }
};

typedef struct hex_row
{
    unsigned short dec_val;
    char name[5];
    char char_hex[2];
} hex_row;

hex_row hex_table[] =
{
    {  0, "0000", "0" },
    {  1, "0001", "1" },
    {  2, "0010", "2" },
    {  3, "0011", "3" },
    {  4, "0100", "4" },
    {  5, "0101", "5" },
    {  6, "0110", "6" },
    {  7, "0111", "7" },
    {  8, "1000", "8" },
    {  9, "1001", "9" },
    { 10, "1010", "a" },
    { 11, "1011", "b" },
    { 12, "1100", "c" },
    { 13, "1101", "d" },
    { 14, "1110", "e" },
    { 15, "1111", "f" }
};


typedef struct directive
{
	char         name[10];
	unsigned int value;
} directive;

// value 1 means label is defined in data section
directive directives[NUMBER_DIRECTIVES] =
{
	{ ".text",  0 },
	{ ".data",  0 },
	{ ".space", 1 },
	{ ".word",  1 }
};


typedef struct r_instruction
{
    char name[8];
    char opcode[7];
    char rs[6];
    char rt[6];
    char rd[6];
    char shamt[6];
    char funct[7];
} r_instruction;

r_instruction r_instructions[NUMBER_R_INSTRUCTIONS] =
{
	{ "addu",    "000000", "00000", "00000", "00000", "00000", "100001" },
	{ "and",     "000000", "00000", "00000", "00000", "00000", "100100" },
	{ "div",     "000000", "00000", "00000", "00000", "00000", "011010" },
    { "mfhi",    "000000", "00000", "00000", "00000", "00000", "010000" },
    { "mflo",    "000000", "00000", "00000", "00000", "00000", "010010" },
    { "mult",    "000000", "00000", "00000", "00000", "00000", "011000" },
    { "or",      "000000", "00000", "00000", "00000", "00000", "100101" },
    { "slt",     "000000", "00000", "00000", "00000", "00000", "101010" },
    { "subu",    "000000", "00000", "00000", "00000", "00000", "100011" },
    { "syscall", "000000", "00000", "00000", "00000", "00000", "001100" }
};


typedef struct i_instruction
{
    char name[8];
    char opcode[7];
    char rs[6];
    char rt[6];
    char immediate[17];
} i_instruction;

i_instruction i_instructions[NUMBER_I_INSTRUCTIONS] =
{
	{ "addiu", "001001", "00000", "00000", "0000000000000000" },
    { "beq",   "000100", "00000", "00000", "0000000000000000" },
    { "bne",   "000101", "00000", "00000", "0000000000000000" },
    { "lw",    "100011", "00000", "00000", "0000000000000000" },
    { "sw",    "101011", "00000", "00000", "0000000000000000" }
};


typedef struct j_instruction
{
	char name[8];
	char opcode[7];
	char address[27];
} j_instruction;

j_instruction j_instructions[NUMBER_J_INSTRUCTIONS] =
{
	{ "j", "000010", "00000000000000000000000000" }
};



typedef struct instruction
{
	char  name[8];
	char  type;
    int   args;
	char  syntax[80];
} instruction;

instruction instructions[NUMBER_INSTRUCTIONS] =
{
    { "addiu",   'I', 3, "$rt,$rs,immed"  },
    { "addu",    'R', 3, "$rd,$rs,$rt"    },
    { "and",     'R', 3, "$rd,$rs,$rt"    },
    { "beq",     'I', 1, "label"          },
    { "bne",     'I', 1, "label"          },
    { "div",     'R', 2, "$rs,$rt"        },
    { "j",       'J', 1, "label"          },
    { "lw",      'I', 2, "$rt,immed($rs)" },
    { "mfhi",    'R', 1, "$rd"            },
    { "mflo",    'R', 1, "$rd"            },
    { "mult",    'R', 2, "$rs,$rt"        },
    { "or",      'R', 3, "$rd,$rs,$rt"    },
    { "slt",     'R', 3, "$rd,$rs,$rt"    },
    { "subu",    'R', 3, "$rd,$rs,$rt"    },
    { "sw",      'I', 2, "$rt,immed($rs)" },
    { "syscall", 'R', 0, ""               }
};



//#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(disable:4996)

typedef struct symbol_table
{
	char         label[MAX_LABEL_SIZE];
	unsigned int line;
	unsigned int immediate;
    int          flgData;
    int          value;
} symbol_table;


typedef struct instruction_table
{
	char name[8];
	char operands[92];
} instruction_table;


typedef struct output_table
{
	char address[7];
	char encoding[9];
} output_table;




// declarations
int findDirective(char *buffer);
int GetInstruction(char *name);
int GetRTypeInstruction(char *name);
int GetITypeInstruction(char *name);
int GetJTypeInstruction(char *name);
void CopyRInstruction(r_instruction src, r_instruction *dst);
void CopyIInstruction(i_instruction src, i_instruction *dst);
void CopyJInstruction(j_instruction src, j_instruction *dst);
void GetRegister(char *str, reg *reg1);
void binStrToHexStr(char *src, char *dst);
void findLabel(char *label, symbol_table *src, symbol_table* dst, int n);
int trim_whitespace(char *buff1);



int findDirective(char *buffer)
{
    int ndx = -1;

    for (int k = 0; k < NUMBER_DIRECTIVES; k++)
    {
        if (strstr(buffer, directives[k].name) != NULL)
        {
            ndx = k;
            break;
        }
    }

    return ndx;
}


int GetInstruction(char *name)
{
	int k;

	for (k= 0; k < NUMBER_INSTRUCTIONS; k++)
	{
		if (strcmp(name, instructions[k].name) == 0)
		{
		break;
		}
	}

	return k;
}


int GetRTypeInstruction(char *name)
{
	int k;

	for (k = 0; k < NUMBER_R_INSTRUCTIONS; k++)
	{
		if (strcmp(name, r_instructions[k].name) == 0)
		{
			break;
		}
	}
	return k;
}


int GetITypeInstruction(char *name)
{
	int k;

	for (k = 0; k < NUMBER_I_INSTRUCTIONS; k++)
	{
		if (strcmp(name, i_instructions[k].name) == 0)
		{
			break;
		}
	}
	return k;
}


int GetJTypeInstruction(char *name)
{
	int k;

	for (k = 0; k < NUMBER_J_INSTRUCTIONS; k++)
	{
		if (strcmp(name, j_instructions[k].name) == 0)
		{
			break;
		}
	}
	return k;
}


void CopyRInstruction(r_instruction src, r_instruction *dst)
{
	strcpy(dst->name, src.name);
	strcpy(dst->opcode, src.opcode);
	strcpy(dst->rs, src.rs);
	strcpy(dst->rt, src.rt);
	strcpy(dst->rd, src.rd);
	strcpy(dst->shamt, src.shamt);
	strcpy(dst->funct, src.funct);
}

void CopyIInstruction(i_instruction src, i_instruction *dst)
{
	strcpy(dst->name, src.name);
	strcpy(dst->opcode, src.opcode);
	strcpy(dst->rs, src.rs);
	strcpy(dst->rt, src.rt);
	strcpy(dst->immediate, src.immediate);
}

void CopyJInstruction(j_instruction src, j_instruction *dst)
{
	strcpy(dst->name, src.name);
	strcpy(dst->opcode, src.opcode);
	strcpy(dst->address, src.address);
}


void GetRegister(char *str, reg *reg1)
{
	for (int k = 0; k < NUMBER_REGISTERS; k++)
	{
		if (strcmp(str, regs[k].name) == 0)
		{
			strcpy(reg1->name, regs[k].name);
			reg1->dec_value = regs[k].dec_value;
			strcpy(reg1->bin_value, regs[k].bin_value);
			break;
		}
	}
}


void strToHex(char *src, char *dst)
{
	if (strcmp(src, "0000") == 0)
	{
		strcpy(dst, "0");
	}
	else if (strcmp(src, "0001") == 0)
	{
		strcpy(dst, "1");
	}
	else if (strcmp(src, "0010") == 0)
	{
		strcpy(dst, "2");
	}
	else if (strcmp(src, "0011") == 0)
	{
		strcpy(dst, "3");
	}
	else if (strcmp(src, "0100") == 0)
	{
		strcpy(dst, "4");
	}
	else if (strcmp(src, "0101") == 0)
	{
		strcpy(dst, "5");
	}
	else if (strcmp(src, "0110") == 0)
	{
		strcpy(dst, "6");
	}
	else if (strcmp(src, "0111") == 0)
	{
		strcpy(dst, "7");
	}
	else if (strcmp(src, "1000") == 0)
	{
		strcpy(dst, "8");
	}
	else if (strcmp(src, "1001") == 0)
	{
		strcpy(dst, "9");
	}
	else if (strcmp(src, "1010") == 0)
	{
		strcpy(dst, "a");
	}
	else if (strcmp(src, "1011") == 0)
	{
		strcpy(dst, "b");
	}
	else if (strcmp(src, "1100") == 0)
	{
		strcpy(dst, "c");
	}
	else if (strcmp(src, "1101") == 0)
	{
		strcpy(dst, "d");
	}
	else if (strcmp(src, "1110") == 0)
	{
		strcpy(dst, "e");
	}
	else if (strcmp(src, "1111") == 0)
	{
		strcpy(dst, "f");
	}
}



void binStrToHexStr (char *src, char *dst)
{
	char *pchar1, *pchar2 = NULL;
	char binbuf[5], hexbuf[5];

	pchar1 = src;
	for (int k = 0; k < 8; k++)
	{
		pchar2 = pchar1;
		pchar2 += 4;
		strncpy(binbuf, pchar1, 4);
		binbuf[4] = '\0';

		strToHex(binbuf, hexbuf);

		pchar1 += 4;
		strcat(dst, hexbuf);
	}
}


void decimal_to_binary(int n, char *buff, int numDigits)
{
	int c, d, cnt;
	char *ptr = buff;

	cnt = 0;

	for (c = numDigits - 1; c >= 0; c--)
	{
		d = n >> c;

		if (d & 1)
			*(ptr + cnt) = 1 + '0';
		else
			*(ptr + cnt) = 0 + '0';

		cnt++;
	}
	*(ptr + cnt) = '\0';

}


void findLabel(char *label, symbol_table *src, symbol_table* dst, int n)
{
	for (int k = 0; k < n; k++)
	{
		if (strcmp(src[k].label, label) == 0)
		{
			strcpy(dst->label, src[k].label);
			dst->line = src[k].line;
			dst->immediate = src[k].immediate;
			break;
		}
	}
}

int trim_whitespace(char *buff1)
{
    char buff2[200];
    char *ptr1, *token;

    char delim[3] = " \t";

    // strip \n from input
    if ((ptr1 = strchr(buff1, '\n')) != NULL)
    {
        *ptr1 = '\0';
    }

    if (strlen(buff1) < 2)
    {
        return 0;
    }
    memset(buff2, '\0', sizeof(buff2));

    token = strtok(buff1, delim);

    // first token is null, we only have whitespaces in the input
    if (token == NULL)
    {
        return 0;
    }

    while (token != NULL)
    {
        strcat(buff2, token);
        token = strtok(NULL, delim);
        if (token != NULL)
        {
            if (strcmp(token, "#") == 0)
            {
                break;
            }
            else
            {
                strcat(buff2, ",");
            }
        }
    }

    strcpy(buff1, buff2);
    return 1;
}


int main()
{
	char              buffer[200];
	symbol_table      sym_table[MAX_INSTRUCTIONS];
	instruction_table inst_table[MAX_INSTRUCTIONS];
	output_table      out_table[MAX_INSTRUCTIONS];

	int               k;
	int               ndx, ndx1, ndx2;
	int               int_value;
	size_t            length;
	int               numberOfSymbols = 0;
    int               numberOfWordsOfData = 0;
	int               numberOfInstructions = 0;
	int               numberOfOutputLines = 0;
	char              char_value[32];

	char *pchar1, *pchar2;

	bool inTextSection = false;
	bool inDataSection = false;
	int pc = 0;
	int lineNumber = 0;

	for (k = 0; k < BUFFERSIZE; k++)
	{
		memset(sym_table[k].label, '\0', MAX_LABEL_SIZE);
		sym_table[k].line = 0;
		sym_table[k].immediate = 0;

		memset(inst_table[k].name, '\0', sizeof(inst_table[k].name));
		memset(inst_table[k].operands, '\0', sizeof(inst_table[k].operands));

        memset(out_table[k].address, '\0', sizeof(out_table[k].address));
        memset(out_table[k].encoding, '\0', sizeof(out_table[k].encoding));
	}


    //
	// pass 1
	//
    int skipLine;
    int data_pc = 0;

	while (fgets(buffer, BUFFERSIZE, stdin) != NULL)
	{
        skipLine = trim_whitespace(buffer);
        if (skipLine == 0)
        {
            // skip line
            continue;
        }

		if (lineNumber == 0 && !inTextSection)
		{
			// first line is expected to be .text
			ndx = findDirective(buffer);
			if (ndx == -1)
			{
				printf("file not well formed: .text expected");
				exit(1);
			}
			else
			{
				// get next line
				inTextSection = true;
                lineNumber++;
				continue;
			}
		}

		if (inTextSection)
		{
			// check for directive
			ndx = findDirective(buffer);

			if (ndx == 1)
			{
				// found .data directive
				inDataSection = true;
				inTextSection = false;
				continue;
			}
			else if (ndx == -1)
			{
				// found instruction
				pchar1 = buffer;

				pchar2 = (strchr(pchar1, ':'));
				if (pchar2 != NULL)
				{
					// found a label
					length = pchar2 - pchar1 + 1;
					snprintf(sym_table[numberOfSymbols].label, length, "%s", pchar1);

					// skip colon and comma
					pchar2 += 2;
					pchar1 = pchar2;

					// getting instruction name
					pchar2 = strchr(pchar1, ',');
					length = pchar2 - pchar1 + 1;
					snprintf(inst_table[numberOfInstructions].name, length, "%s", pchar1);

                    // skip comma
                    pchar2++;

					// getting operands
                    pchar2 = strchr(pchar1, ',');

                    // skip comma
                    pchar2++;
					strcpy(inst_table[numberOfInstructions].operands, pchar2);

					pc += 4;
					lineNumber++;
					numberOfInstructions++;
					sym_table[numberOfSymbols].line = lineNumber;
					sym_table[numberOfSymbols].immediate = pc;
					numberOfSymbols++;
				}
				else
				{
					// expected instruction
     				// getting instruction name
                    if (strcmp(pchar1, "syscall") == 0)
                    {
                        strcpy(inst_table[numberOfInstructions].name, pchar1);
                    }
                    else
                    {
                        pchar2 = strchr(pchar1, ',');
                        if (*pchar2 != NULL)
                        {
                            length = pchar2 - pchar1 + 1;
                            snprintf(inst_table[numberOfInstructions].name, length, "%s", pchar1);
                        }
                        else
                        {
                            sprintf(inst_table[numberOfInstructions].name, "%s", pchar1);
                        }

                        // getting operands
                        pchar2++;						// skip comma
                        strcpy(inst_table[numberOfInstructions].operands, pchar2);
                    }

                    pc += 4;
					numberOfInstructions++;
				}
			}
			else
			{
				printf("file not well formed: .data expected");
				exit(1);
			}
		}

    	if (inDataSection)
		{
			// label expected
			pchar2 = (strchr(buffer, ':'));

    		if (pchar2 != NULL)
			{
				// found a label
				pchar1 = buffer;
				if (pchar1 != NULL)
				{
					length = pchar2 - pchar1 + 1;
					snprintf(sym_table[numberOfSymbols].label, length, "%s", pchar1);
					sym_table[numberOfSymbols].line = lineNumber;
					sym_table[numberOfSymbols].immediate = pc;

					// skip colon and tab
					pchar2 += 2;
                    pchar1 = pchar2;

					// check for directive; expecting .space or .word
					ndx = findDirective(pchar2);
					if (ndx == 2 || ndx == 3)
					{
						pchar1 = strchr(pchar2, ',');
						pchar1++;         // skip comma
						strcpy(char_value, pchar1);
						int_value = (int)strtol(char_value, NULL, 10);

						if (ndx == 2)
						{
							// found .space
							// next address will be .space + int_value * 1 (byte)
							// make alignment even if needed
							k = int_value % 2;
							if (k != 0)
							{
								int_value++;
							}
                            sym_table[numberOfSymbols].value = int_value;
                            sym_table[numberOfSymbols].immediate = data_pc;
                            sym_table[numberOfSymbols].flgData = 1;
                            data_pc += int_value;
                            numberOfWordsOfData++;
                        }

						if (ndx == 3)
						{
							// found .word
							// next address will be label + 4
                            sym_table[numberOfSymbols].value = int_value;
                            sym_table[numberOfSymbols].flgData = 1;
							sym_table[numberOfSymbols].immediate = data_pc;
							data_pc += 4;
                            numberOfWordsOfData++;
						}
						numberOfSymbols++;
					}
					else
					{
						printf("file not well formed: directive [.space|.word] expected\n");
						exit(1);
					}
				}
			}
			else
			{
				printf("file not well formed: label expected\n");
				exit(1);
			}
		}

	}


	//
	// pass 2
	//

    int cur_pc;
	reg           reg1;
	r_instruction rtype;
	i_instruction itype;
	j_instruction jtype;
	symbol_table  sym_table_row;

	for (k = 0; k < numberOfInstructions; k++)
	{
		ndx1 = GetInstruction(inst_table[k].name);
		if (instructions[ndx1].type == 'R')
		{
            // processing rtype instructions
            ndx2 = GetRTypeInstruction(instructions[ndx1].name);
            CopyRInstruction(r_instructions[ndx2], &rtype);

			if (instructions[ndx1].args == 3)
			{
				// processing instructions with 3 arguments
                // rd, rs, rt
				pchar1 = pchar2 = inst_table[k].operands;

				// getting rd
				pchar2 = strchr(pchar1, ',');
				length = pchar2 - pchar1 + 1;
				snprintf(char_value, length, "%s", pchar1);
				GetRegister(char_value, &reg1);
				strcpy(rtype.rd, reg1.bin_value);

				// getting rs
				pchar2++;                                  // skip comma
				pchar1 = pchar2;
				pchar2 = strchr(pchar1, ',');
				length = pchar2 - pchar1 + 1;
				snprintf(char_value, length, "%s", pchar1);
				GetRegister(char_value, &reg1);
				strcpy(rtype.rs, reg1.bin_value);

				// getting rt
				pchar2++;                                  // skip comma
				pchar1 = pchar2;
                strcpy(char_value, pchar1);

				pchar1 = strchr(char_value, '$');
				if (pchar1 == NULL)
				{
					// we got a value
					int_value = atoi(char_value);
					decimal_to_binary(int_value, char_value, 5);
					strcpy(rtype.rt, reg1.bin_value);
				}
				else
				{
					// we got a register
					GetRegister(char_value, &reg1);
					strcpy(rtype.rt, reg1.bin_value);
				}

				sprintf(buffer, "%s%s%s%s%s%s", rtype.opcode, rtype.rs, rtype.rt, rtype.rd, rtype.shamt, rtype.funct);
				memset(char_value, '\0', sizeof(char_value));
				binStrToHexStr(buffer, char_value);
				strcpy(out_table[numberOfOutputLines].encoding, char_value);
				int_value = numberOfOutputLines * 4;
				sprintf(char_value, "%06x", int_value);
				strcpy(out_table[numberOfOutputLines].address, char_value);
				numberOfOutputLines++;
			}
			else if (instructions[ndx1].args == 2)
			{
				// processing instructions with 2 arguments
                // rs, rt
				pchar1 = pchar2 = inst_table[k].operands;

				// getting rd
                GetRegister("$zero", &reg1);
                strcpy(rtype.rd, reg1.bin_value);

				// getting rs
				pchar2 = strchr(pchar1, ',');
				length = pchar2 - pchar1 + 1;
				snprintf(char_value, length, "%s", pchar1);
				GetRegister(char_value, &reg1);
				strcpy(rtype.rs, reg1.bin_value);

				// getting rt
				pchar2++;                                  // skip comma
				pchar1 = pchar2;
				pchar2 = strchr(pchar1, '$');

				//length = pchar2 - pchar1 + 1;
				//snprintf(char_value, length, "%s", pchar1);
				//pchar1 = strchr(char_value, '$');
				if (pchar2 == NULL)
				{
					// we got a value
					snprintf(char_value, length, "%s", pchar1);
					int_value = atoi(char_value);
					decimal_to_binary(int_value, char_value, 5);
					strcpy(rtype.rt, char_value);
				}
				else
				{
					// we got a register
					GetRegister(char_value, &reg1);
					strcpy(rtype.rt, reg1.bin_value);
				}

				sprintf(buffer, "%s%s%s%s%s%s", rtype.opcode, rtype.rs, rtype.rt, rtype.rd, rtype.shamt, rtype.funct);
				memset(char_value, '\0', sizeof(char_value));
				binStrToHexStr(buffer, char_value);
				strcpy(out_table[numberOfOutputLines].encoding, char_value);
				int_value = numberOfOutputLines * 4;
				sprintf(char_value, "%06x", int_value);
				strcpy(out_table[numberOfOutputLines].address, char_value);
				numberOfOutputLines++;
			}
			else if (instructions[ndx1].args == 1)
			{
				// processing instructions with 1 argument
				// $rd
				pchar1 = pchar2 = inst_table[k].operands;

				// getting rd
				pchar2 = strchr(pchar1, ',');
				length = pchar2 - pchar1 + 1;
				snprintf(char_value, length, "%s", pchar1);
				GetRegister(char_value, &reg1);
				strcpy(rtype.rd, reg1.bin_value);

				// getting rt
				GetRegister("$zero", &reg1);
				strcpy(rtype.rt, reg1.bin_value);

				// getting shamt
				int_value = 0;
				decimal_to_binary(int_value, char_value, 5);
				strcpy(rtype.shamt, char_value);

				sprintf(buffer, "%s%s%s%s%s%s", rtype.opcode, rtype.rs, rtype.rt, rtype.rd, rtype.shamt, rtype.funct);
				memset(char_value, '\0', sizeof(char_value));
				binStrToHexStr(buffer, char_value);
				strcpy(out_table[numberOfOutputLines].encoding, char_value);
				int_value = numberOfOutputLines * 4;
				sprintf(char_value, "%06x", int_value);
				strcpy(out_table[numberOfOutputLines].address, char_value);
				numberOfOutputLines++;
			}
            else if (instructions[ndx1].args == 0)
            {
                // processing instructions with zero arguments
                pchar1 = pchar2 = inst_table[k].operands;

                // getting rd
                GetRegister("$zero", &reg1);
                strcpy(rtype.rd, reg1.bin_value);

                // getting rt
                GetRegister("$zero", &reg1);
                strcpy(rtype.rt, reg1.bin_value);

                // getting shamt
                int_value = 0;
                decimal_to_binary(int_value, char_value, 5);
                strcpy(rtype.shamt, char_value);

                sprintf(buffer, "%s%s%s%s%s%s", rtype.opcode, rtype.rs, rtype.rt, rtype.rd, rtype.shamt, rtype.funct);
                memset(char_value, '\0', sizeof(char_value));
                binStrToHexStr(buffer, char_value);
                strcpy(out_table[numberOfOutputLines].encoding, char_value);
                int_value = numberOfOutputLines * 4;
                sprintf(char_value, "%06x", int_value);
                strcpy(out_table[numberOfOutputLines].address, char_value);
                numberOfOutputLines++;
            }
		}
		else if (instructions[ndx1].type == 'I')
		{
			// processing itype instructions
			// processing itype instructions
			ndx2 = GetITypeInstruction(instructions[ndx1].name);
			CopyIInstruction(i_instructions[ndx2], &itype);

			if (ndx2 == 0)
			{
				// instruction addui processing
				pchar1 = pchar2 = inst_table[k].operands;

				// getting rt
				pchar2 = strchr(pchar1, ',');
				length = pchar2 - pchar1 + 1;
				snprintf(char_value, length, "%s", pchar1);
				GetRegister(char_value, &reg1);
				strcpy(itype.rt, reg1.bin_value);

				// getting rs
				pchar2++;                                  // skip comma
				pchar1 = pchar2;
				pchar2 = strchr(pchar1, ',');
				length = pchar2 - pchar1 + 1;
				snprintf(char_value, length, "%s", pchar1);
				GetRegister(char_value, &reg1);
				strcpy(itype.rs, reg1.bin_value);

				// getting immediate
				pchar2++;                                  // skip comma
				sprintf(char_value, "%s", pchar2);
				int_value = atoi(char_value);
				decimal_to_binary(int_value, char_value, 16);
				strcpy(itype.immediate, char_value);

				sprintf(buffer, "%s%s%s%s", itype.opcode, itype.rs, itype.rt, itype.immediate);
				memset(char_value, '\0', sizeof(char_value));
				binStrToHexStr(buffer, char_value);
				strcpy(out_table[numberOfOutputLines].encoding, char_value);
				int_value = numberOfOutputLines * 4;
				sprintf(char_value, "%06x", int_value);
				strcpy(out_table[numberOfOutputLines].address, char_value);
				numberOfOutputLines++;
			}
            else if (ndx2 == 1)
            {
                // instruction beq processing: $rs,$rt,label
                pchar1 = pchar2 = inst_table[k].operands;

                // getting rs
                pchar2 = strchr(pchar1, ',');
                length = pchar2 - pchar1 + 1;
                snprintf(char_value, length, "%s", pchar1);
                GetRegister(char_value, &reg1);
                strcpy(itype.rs, reg1.bin_value);

                // getting rt
                pchar2++;                                  // skip comma
                pchar1 = pchar2;
                pchar2 = strchr(pchar1, ',');
                length = pchar2 - pchar1 + 1;
                snprintf(char_value, length, "%s", pchar1);
                GetRegister(char_value, &reg1);
                strcpy(itype.rt, reg1.bin_value);

                // getting label
                pchar2++;                                  // skip comma
                sprintf(char_value, "%s", pchar2);

                // find label in table
                findLabel(char_value, sym_table, &sym_table_row, numberOfSymbols);

                // getting immediate
                cur_pc = 4 * k;
                sym_table_row.immediate -= 4;
                int_value = (sym_table_row.immediate - cur_pc) / 4;
                memset(char_value, '\0', sizeof(char_value));
                decimal_to_binary(int_value, char_value, 16);
                strcpy(itype.immediate, char_value);

                sprintf(buffer, "%s%s%s%s", itype.opcode, itype.rs, itype.rt, itype.immediate);
                memset(char_value, '\0', sizeof(char_value));
                binStrToHexStr(buffer, char_value);
                strcpy(out_table[numberOfOutputLines].encoding, char_value);
                int_value = numberOfOutputLines * 4;
                sprintf(char_value, "%06x", int_value);
                strcpy(out_table[numberOfOutputLines].address, char_value);
                numberOfOutputLines++;
            }
            else if (ndx2 == 2)
			{
				// instruction bne
                pchar1 = pchar2 = inst_table[k].operands;

                // getting rs
                pchar2 = strchr(pchar1, ',');
                length = pchar2 - pchar1 + 1;
                snprintf(char_value, length, "%s", pchar1);
                GetRegister(char_value, &reg1);
                strcpy(itype.rs, reg1.bin_value);

                // getting rt
                pchar2++;                                  // skip comma
                pchar1 = pchar2;
                pchar2 = strchr(pchar1, ',');
                length = pchar2 - pchar1 + 1;
                snprintf(char_value, length, "%s", pchar1);
                GetRegister(char_value, &reg1);
                strcpy(itype.rt, reg1.bin_value);

                // getting label
                pchar2++;                                  // skip comma
                sprintf(char_value, "%s", pchar2);

                // find label in table
                findLabel(char_value, sym_table, &sym_table_row, numberOfSymbols);

                // getting immediate
                cur_pc = 4 * k;
                sym_table_row.immediate -= 4;
                int_value = (sym_table_row.immediate - cur_pc) / 4;
                memset(char_value, '\0', sizeof(char_value));
                decimal_to_binary(int_value, char_value, 16);
                strcpy(itype.immediate, char_value);

                sprintf(buffer, "%s%s%s%s", itype.opcode, itype.rs, itype.rt, itype.immediate);
                memset(char_value, '\0', sizeof(char_value));
                binStrToHexStr(buffer, char_value);
                strcpy(out_table[numberOfOutputLines].encoding, char_value);
                int_value = numberOfOutputLines * 4;
                sprintf(char_value, "%06x", int_value);
                strcpy(out_table[numberOfOutputLines].address, char_value);
                numberOfOutputLines++;
            }
            else if (ndx2 == 3)
            {
                // instruction lw processing: $rt,immed($rs)
                pchar1 = pchar2 = inst_table[k].operands;

                // getting rt
                pchar2 = strchr(pchar1, ',');
                length = pchar2 - pchar1 + 1;
                snprintf(char_value, length, "%s", pchar1);
                GetRegister(char_value, &reg1);
                strcpy(itype.rt, reg1.bin_value);

                // getting immediate
                pchar2++;                                  // skip comma
                pchar1 = pchar2;
                pchar2 = strchr(pchar1, '(');
                length = pchar2 - pchar1 + 1;
                snprintf(char_value, length, "%s", pchar1);
                int_value = atoi(char_value);
                decimal_to_binary(int_value, char_value, 16);
                strcpy(itype.immediate, char_value);

                // getting rs
                pchar2++;                                  // skip comma
                pchar1 = pchar2;
                pchar2 = strchr(pchar1, ')');
                length = pchar2 - pchar1 + 1;
                snprintf(char_value, length, "%s", pchar1);
                GetRegister(char_value, &reg1);
                strcpy(itype.rs, reg1.bin_value);

                sprintf(buffer, "%s%s%s%s", itype.opcode, itype.rs, itype.rt, itype.immediate);
                memset(char_value, '\0', sizeof(char_value));
                binStrToHexStr(buffer, char_value);
                strcpy(out_table[numberOfOutputLines].encoding, char_value);
                int_value = numberOfOutputLines * 4;
                sprintf(char_value, "%06x", int_value);
                strcpy(out_table[numberOfOutputLines].address, char_value);
                numberOfOutputLines++;
            }
            else if (ndx2 == 4)
			{
				// instruction sw processing: $rt,immed($rs)
				pchar1 = pchar2 = inst_table[k].operands;

				// getting rt
				pchar2 = strchr(pchar1, ',');
				length = pchar2 - pchar1 + 1;
				snprintf(char_value, length, "%s", pchar1);
				GetRegister(char_value, &reg1);
				strcpy(itype.rt, reg1.bin_value);

				// getting immediate
				pchar2++;                                  // skip comma
				pchar1 = pchar2;
				pchar2 = strchr(pchar1, '(');
				length = pchar2 - pchar1 + 1;
				snprintf(char_value, length, "%s", pchar1);
				int_value = atoi(char_value);
				decimal_to_binary(int_value, char_value, 16);
				strcpy(itype.immediate, char_value);

				// getting rs
				pchar2++;                                  // skip comma
				pchar1 = pchar2;
				pchar2 = strchr(pchar1, ')');
				length = pchar2 - pchar1 + 1;
				snprintf(char_value, length, "%s", pchar1);
				GetRegister(char_value, &reg1);
				strcpy(itype.rs, reg1.bin_value);

				sprintf(buffer, "%s%s%s%s", itype.opcode, itype.rs, itype.rt, itype.immediate);
				memset(char_value, '\0', sizeof(char_value));
				binStrToHexStr(buffer, char_value);
				strcpy(out_table[numberOfOutputLines].encoding, char_value);
				int_value = numberOfOutputLines * 4;
				sprintf(char_value, "%06x", int_value);
				strcpy(out_table[numberOfOutputLines].address, char_value);
				numberOfOutputLines++;
			}
		}
    	else if (instructions[ndx1].type == 'J')
		{
			// processing jype instructions
			ndx2 = GetJTypeInstruction(instructions[ndx1].name);
			CopyJInstruction(j_instructions[ndx2], &jtype);

			// instruction jtype: $rt,immed($rs)
			pchar1 = pchar2 = inst_table[k].operands;

			// get label
			sprintf(char_value, "%s", pchar1);

			// find label in table
			findLabel(char_value, sym_table, &sym_table_row, numberOfSymbols);

			// getting immediate
            cur_pc = ((k + 1) << 2) >> 28;
			sym_table_row.immediate -= 4;
            int_value = cur_pc | (sym_table_row.immediate >> 2 ) | 00;
			memset(char_value, '\0', sizeof(char_value));
			decimal_to_binary(int_value, char_value, 26);
			strcpy(jtype.address, char_value);

			sprintf(buffer, "%s%s", jtype.opcode, jtype.address);
			memset(char_value, '\0', sizeof(char_value));
			binStrToHexStr(buffer, char_value);
			strcpy(out_table[numberOfOutputLines].encoding, char_value);
			int_value = numberOfOutputLines * 4;
			sprintf(char_value, "%06x", int_value);
			strcpy(out_table[numberOfOutputLines].address, char_value);
			numberOfOutputLines++;
		}
	}
    
    printf("%d %d\n", numberOfInstructions, numberOfWordsOfData);

	for (k = 0; k < numberOfOutputLines; k++)
	{
		printf("%s\n", out_table[k].encoding);
	}

    for (k = 0; k < MAX_INSTRUCTIONS; k++)
    {
        if (sym_table[k].flgData == 1)
        {
            int_value = sym_table[k].value;
            memset(char_value, '\0', sizeof(char_value));
            decimal_to_binary(int_value, buffer, 32);
            memset(char_value, '\0', sizeof(char_value));
            binStrToHexStr(buffer, char_value);

            printf("%s\n", char_value);
        }
    }

	return 0;
}

