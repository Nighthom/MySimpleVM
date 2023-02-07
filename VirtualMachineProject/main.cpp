#include "vm.h"
#include "filemanager.h"
#include <stdio.h>
#include <memory.h>

int main() {
	char str[100] = { 0 };
	printf("VM에 넣을 OpCode의 경로를 입력하세요. \n");
	printf("입력 : ");
	scanf_s("%s", &str, 100);
	FileManager OpFile(str, "r");

	char* opcode = OpFile.readData();
		
	VM vm = VM(opcode);
	vm.runLoop();
	return 0;
}

