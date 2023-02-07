#pragma once
#include <malloc.h>
#include <string.h>
#include <stdio.h>

#define OPERATION_COUNT 0x1A

#define DATA_START		0x00000000
#define SUB				0x00000001
#define MUL				0x00000002
#define DIV				0x00000003
#define PRINTSTR		0x00000004
#define JMP				0x00000005
#define CMP				0x00000006
#define PUSHR			0x00000007
#define MOV				0x00000008
#define CALL			0x00000009
#define MOVR			0x0000000A
#define RET				0x0000000B
#define PUSH			0x0000000C
#define POP				0x0000000D
#define ADD				0x0000000E
#define MOVS			0x0000000F
#define ADDR			0x00000010
#define SUBR			0x00000011
#define MULR			0x00000012
#define DIVR			0x00000013
#define PRINTINT		0x00000014
#define JEQ				0x00000015
#define JL				0x00000016
#define JG				0x00000017
#define JLE				0x00000018
#define JGE				0x00000019

#define ENTRYPOINT		0x00400000
#define STACK_IMAGEBASE 0x70001000
#define STACK_SIZE		0x1000
#define DATA_IMAGEBASE	0x60000000
#define DATA_SIZE		0x10000000

#define REGISTER_SIZE	8
#define EAX				0 
#define EBX				1
#define ECX				2
#define EDX				3
#define EIP				4
#define ESP				5
#define ECONDITION		6
#define EBP				7

#define eax m_Registers[EAX]
#define ebx m_Registers[EBX]
#define ecx m_Registers[ECX]
#define edx m_Registers[EDX]
#define eip m_Registers[EIP]
#define esp m_Registers[ESP]
#define ebp m_Registers[EBP]
#define econdition m_Registers[ECONDITION]

typedef unsigned int DWORD;
typedef unsigned char WORD;

typedef struct _OPCODE {
	DWORD operation;
	DWORD dest, source;
} OPCODE;				//  Total Opcode's size = 12byte


// 파일헤더 정의. 파일헤더는 
#define FILEHEADER_SIZE sizeof(FILE_HEADER)
typedef struct _FILEHEADER {
	DWORD code_size;
	DWORD data_size;
}FILE_HEADER;

#define OPCODE_SIZE sizeof(OPCODE)
class VM;

class Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*) = 0;
};

class VM {					// VM 생성 시 VM이 실행할 OpCode정보를 읽는다. 
public:
	VM(char*);
	~VM();

	DWORD execute();
	virtual DWORD runLoop();
	DWORD* getPointer(DWORD);
	
	FILE_HEADER* header;
	DWORD m_Registers[REGISTER_SIZE] = { 0 };
	WORD* m_Memory;
	DWORD dataCount = 0;
private:
	Operation* operation[OPERATION_COUNT];
};


class Add : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class Sub : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class Mul : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class Div : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class PrintStr : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class Jmp : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class Cmp : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class PushR : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class MovS : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class MovR : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class Mov : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class Call : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class Ret : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class Push : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class Pop : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class AddR : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class SubR : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class MulR : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class DivR : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class PrintInt : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class Jeq : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class Jl : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class Jg : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class Jle : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

class Jge : public Operation {
public:
	virtual DWORD run(DWORD, DWORD, VM*);
};

