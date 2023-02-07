# MySimpleVM
간단한 bytecode로 돌아가는 VM입니다. 32bit 메모리 주소를 사용하고, heap을 활용해서 
메모리를 해당 VM에 할당합니다.

# 구조
## 섹션의 위치
EntryPoint의 Imagebase = 0x00400000   
Stack의  Imagebase     = 0x70001000   
Stack의  Size          = 0x1000   
Data의 Imagebase       = 0x60000000   
Data의 Size            = 0x10000000   

EntryPoint는 Code 섹션의 시작 값이자, VM이 바이트코드를 실행할 때 가장 처음 실행할 명령어의    
위치입니다. VM이 프로그램을 실행하면 EIP값을 해당 값으로 초기화합니다.   

Stack은 해당 VM이 사용할 Stack 메모리의 초기 값을 가리킵니다. VM이 프로그램을 실행하면    
ESP, EBP 값을 해당 값으로 초기화합니다. 스택의 최대 크기는 0x1000으로 정의되어 있습니다.   

Data는 상수, 전역변수 등의 값이 담겨있는 곳입니다. VM이 프로그램을 실행하면   
문자열 리터럴과 같은 상수 값을 파일에서 읽어 해당 섹션에 위치시킵니다.    

## 파일 헤더
```C++
typedef struct _FILEHEADER {
	DWORD code_size;
	DWORD data_size;
}FILE_HEADER;
```
8바이트로 구성되어 있으며, 각각 코드 섹션의 크기, 데이터 섹션의 크기를 가리킵니다.    
VM은 프로그램 초기 실행 시 0x8부터 0x8+code_size까지의 값을 실행 코드로 취급하고 EntryPoint에 복사합니다.   
0x9+code_size부터 0x9+code_size+data_size까지는 Data로 취급하고 Data 섹션에 복사합니다.    

## Opcode
```C++
typedef struct _OPCODE {
	DWORD operation;
	DWORD dest, source;
} OPCODE;
```
12바이트로 구성되어 있으며 첫 4바이트는 수행할 operation을 가리키고,    
나머지 8바이트는 각각 연산의 destination, source를 가리킵니다.   
source에 해당하는 값 필요하지 않는 명령어의 경우 source 값을 무시합니다.   
destination와 source가 둘다 필요한 경우 연산의 결과는 destination에 저장됩니다.   

## 레지스터
레지스터의 경우 VM의 멤버 변수 배열로 선언되어 있습니다. 
```C++
#define REGISTER_SIZE 8
#define EAX				0 
#define EBX				1
#define ECX				2
#define EDX				3
#define EIP				4
#define ESP				5
#define ECONDITION			6
#define EBP				7

class VM {
public:
...
    DWORD m_Registers[REGISTER_SIZE] = 0;
...
}
```
