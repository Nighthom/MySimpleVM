# MySimpleVM
간단한 bytecode로 돌아가는 VM입니다. 32bit 메모리 주소를 사용하고, heap을 활용해서 
메모리를 해당 VM에 할당합니다.

# 문법
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
