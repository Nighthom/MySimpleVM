#include "vm.h"

VM::VM(char *opcode) {
    // 메모리 공간 확보(4GB)
    m_Memory = (WORD *)malloc(0x99999999);
    if (m_Memory == nullptr)
        return;
    memset((void *)m_Memory, 0, 0x99999999);

    // 파일헤더 읽기
    header = (FILE_HEADER *)opcode;
    // ENTRYPOINT에 OPCODE 복사
    memcpy(m_Memory + ENTRYPOINT, opcode + FILEHEADER_SIZE, header->code_size);
    // DATA 섹션 쓰기
    memcpy(m_Memory + DATA_IMAGEBASE,
                 opcode + FILEHEADER_SIZE + header->code_size, header->data_size);

    // operation Table 초기화
    operation[ADD] = new Add();
    operation[SUB] = new Sub();
    operation[MUL] = new Mul();
    operation[DIV] = new Div();
    operation[ADDR] = new AddR();
    operation[SUBR] = new SubR();
    operation[MULR] = new MulR();
    operation[DIVR] = new DivR();
    operation[PRINTSTR] = new PrintStr();
    operation[JMP] = new Jmp();
    operation[CMP] = new Cmp();
    operation[PUSHR] = new PushR();
    operation[MOVR] = new MovR();
    operation[MOV] = new Mov();
    operation[CALL] = new Call();
    operation[RET] = new Ret();
    operation[PUSH] = new Push();
    operation[POP] = new Pop();
    operation[MOVS] = new MovS();
    operation[PRINTINT] = new PrintInt();
    operation[JEQ] = new Jeq();
    operation[JL] = new Jl();
    operation[JG] = new Jg();
    operation[JLE] = new Jle();
    operation[JGE] = new Jge();

    // 스택 생성
    ebp = STACK_IMAGEBASE;
    esp = STACK_IMAGEBASE;

    // EIP를 EP로 설정
    eip = ENTRYPOINT;

    char *str = (char *)&m_Memory[DATA_IMAGEBASE];
    strcpy_s(str, 20, "Hello World!");
}

VM::~VM() {
    // 확보한 메모리 공간 삭제
    free(m_Memory);
    for (int i = 0; i < OPERATION_COUNT; i++)
        delete (operation[i]);
}

DWORD VM::execute() {
    OPCODE *opcode = (OPCODE *)(m_Memory + eip);                            // opcode fetch
    DWORD dest = opcode->dest;
    DWORD source = opcode->source;
    operation[opcode->operation]->run(dest, source, this);        // opcode execute
    eip += OPCODE_SIZE;
    return 0;
}

DWORD VM::runLoop() {
    while (m_Memory[eip]) { // 현재 메모리 블록의 eip값이 0이 나올때까지 반복한다.
        execute();
    }
    return 0;
}

DWORD *VM::getPointer(DWORD address) { 
        return (DWORD *)&(m_Memory[address]);                    // VM의 메모리 주소를 실제 메모리 주소로 변환
}


// 메모리에 직접 접근하는 instruction의 경우, 
// dest에 해당하는 값이 레지스터의 크기보다 작을 때 
// 해당 dest 값에 해당하는 register로 취급한다. 

// 레지스터와 source의 사칙연산을 정의한다. add eax, 7과 같은 연산.
DWORD Add::run(DWORD dest, DWORD source, VM *vm) { 
    return vm->m_Registers[dest] += source;
}

DWORD Sub::run(DWORD dest, DWORD source, VM *vm) {
    return vm->m_Registers[dest] -= source;
}

DWORD Mul::run(DWORD dest, DWORD source, VM *vm) {
    return vm->m_Registers[dest] *= source;
}

DWORD Div::run(DWORD dest, DWORD source, VM *vm) {
    return vm->m_Registers[dest] /= source;
}

// dest가 레지스터인 경우는 해당 레지스터가 가리키는 문자열을, 
// 메모리 주소를 가리킬 경우 해당 메모리 주소가 가리키는 문자열을 출력한다.
DWORD PrintStr::run(DWORD dest, DWORD source, VM *vm) {
    if (dest < REGISTER_SIZE)
        dest = vm->m_Registers[dest];
    DWORD *destination = vm->getPointer(dest);
    const char *string = (const char *)destination;
    puts(string);
    return 0;
}

// 대상이 레지스터인 경우는 레지스터의 값을 출력하고, 
// 대상이 메모리 주소인 경우 해당 메모리 주소에 담긴 정수 값을 출력한다.
DWORD PrintInt::run(DWORD dest, DWORD source, VM *vm) {
    if (dest < REGISTER_SIZE) { // 레지스터인 경우
        printf("%d\n", vm->m_Registers[dest]);
        return 0;
    }
    DWORD *destination = vm->getPointer(dest);
    printf("%d", *destination);
    return 0;
}

// Jump연산
DWORD Jmp::run(DWORD dest, DWORD source, VM *vm) {
    vm->eip = dest - OPCODE_SIZE; // Jmp연산은 EIP를 정확히 해당 위치로 이동시켜야
                                  // 하는데 execute 이후 EIP값에 옵코드 크기를
                                  // 더하므로 옵코드 크기를 미리 빼놓는다.
    return 0;
}

// Compare 연산. 조건에 따라 ECONDITION의 값을 설정한다. 
// ECONDITION이 0일 경우 dest, source의 값은 동일
// ECONDITION이 양수일 경우 dest의 값이 더 큼
// ECONDITION이 음수일 경우 dest의 값이 더 작음
DWORD Cmp::run(DWORD dest, DWORD source, VM *vm) { // 레지스터와 메모리값 비교
    DWORD *src = vm->getPointer(source);
    return vm->m_Registers[ECONDITION] = vm->m_Registers[dest] - *src;
}


// EBP + source, EBP - source와 같은 경우 정의. 즉, 로컬 변수나 매개 변수에 접근하는
// 동작을 제어한다. 
DWORD MovS::run(DWORD dest, DWORD source, VM *vm) { // mov eax, ebp+source와 같은 꼴의 경우를 정의함
    int *src = (int*)vm->getPointer(vm->ebp + source);
    if (dest < REGISTER_SIZE) // dest의 값이 레지스터일 경우
        vm->m_Registers[dest] = *src;
    else { // dest의 값이 메모리 범위 내일 경우
        DWORD *destination = vm->getPointer(dest);
        *destination = *src;
    }
    return 0;
}

// source의 값이 register인 경우 사용하는 명령어. ex) mov eax, ebx
DWORD MovR::run(DWORD dest, DWORD source, VM *vm) { // source가 레지스터일 경우
    if (dest < REGISTER_SIZE) // dest의 값이 레지스터일 경우
        vm->m_Registers[dest] = vm->m_Registers[source];
    else { // dest의 값이 메모리 범위 내일 경우
        DWORD *destination = vm->getPointer(dest);
        *destination = vm->m_Registers[source];
    }
    return 0;
}


// source에 있는 값이 단순 값인 경우. ex) mov eax, 7 or mov 0x12345678, 8 등등..
// data 섹션에 있는 전역 변수에 직접 접근할 때 활용할 수 있다. 
DWORD Mov::run(DWORD dest, DWORD source, VM *vm) {
    if (dest < REGISTER_SIZE) // dest의 값이 레지스터일 경우
        vm->m_Registers[dest] = source;
    else { // dest의 값이 메모리 범위 내일 경우
        DWORD *destination = vm->getPointer(dest);
        *destination = source;
    }

    return 0;
}

// 스택에 push하는 연산. source 값은 사용하지 않는다. 
// 해당 연산의 결과로 esp가 4 줄어든다.
DWORD Push::run(DWORD dest, DWORD source, VM *vm) {
    vm->esp -= 4;
    DWORD *pointer = vm->getPointer(vm->esp);
    *pointer = dest;
    return 0;
}

// 스택에 push하는 연산. 해당 연산은 레지스터를 push한다.
DWORD PushR::run(DWORD dest, DWORD source, VM *vm) {
    DWORD destination = vm->m_Registers[dest];
    Push().run(destination, source, vm);
    return 0;
}


// dest에 해당하는 메모리 주소로 pop하는 연산.
// 레지스터의 경우 레지스터로 pop한다. 
DWORD Pop::run(DWORD dest, DWORD source, VM *vm) {
    DWORD *stack = vm->getPointer(vm->esp);
    if (dest < REGISTER_SIZE)
        vm->m_Registers[dest] = *stack;
    else {
        DWORD *destination = vm->getPointer(dest);
        *destination = *stack;
    }

    vm->esp += 4;
    return 0;
}

// 함수 call 연산자. dest에 해당하는 주소의 
// 함수를 call한다. 현재 eip 주소 + opcode_size를 스택에 push하고 
// 함수 주소로 Jump한다.
DWORD Call::run(DWORD dest, DWORD source, VM *vm) {
    Push().run(vm->eip + OPCODE_SIZE, source, vm);
    Jmp().run(dest, source, vm);
    return 0;
}

// 함수 return 연산자. 
// 현재 esp의 값을 ebp로 복원하고, 현재 ebp에 caller의 
// ebp를 복원한 다음 함수 호출 이전 EIP 레지스터로
// 복귀한다. 
DWORD Ret::run(DWORD dest, DWORD source, VM *vm) {
    MovR().run(ESP, EBP, vm);
    Pop().run(EBP, source, vm);
    Pop().run(EIP, source, vm);
    vm->eip -= OPCODE_SIZE;
    return 0;
}

// dest, source가 모두 레지스터인 경우에서의 
// 사칙연산을 정의한다. 
DWORD AddR::run(DWORD dest, DWORD source, VM *vm) {
    vm->m_Registers[dest] += vm->m_Registers[source];
    return 0;
}

DWORD SubR::run(DWORD dest, DWORD source, VM *vm) {
    vm->m_Registers[dest] -= vm->m_Registers[source];
    return 0;
}

DWORD MulR::run(DWORD dest, DWORD source, VM *vm) {
    vm->m_Registers[dest] *= vm->m_Registers[source];
    return 0;
}

DWORD DivR::run(DWORD dest, DWORD source, VM *vm) {
    vm->m_Registers[dest] -= vm->m_Registers[source];
    return 0;
}


// 조건 분기. econdition의 값이 0이면(앞선 cmp연산의 결과가 
// dest==source 경우) 점프. 
DWORD Jeq::run(DWORD dest, DWORD source, VM *vm) {
    if (vm->econdition == 0)
        Jmp().run(dest, source, vm);
    return 0;
}

// 조건 분기. econdition의 값이 음수(앞선 cmp연산의 결과가 
// dest>source인 경우) 점프.
DWORD Jl::run(DWORD dest, DWORD source, VM *vm) {
    if (vm->econdition < 0)
        Jmp().run(dest, source, vm);
    return 0;
}


// 조건 분기. econdition의 값이 양수(앞선 cmp연산의 결과가
// dest>source일 경우) 점프
DWORD Jg::run(DWORD dest, DWORD source, VM *vm) {
    if (vm->econdition > 0)
        Jmp().run(dest, source, vm);
    return 0;
}

// 조건 분기. econdition의 값이 음수이거나 0일 경우(앞선 cmp연산의 결과가
// dest<=source일 경우) 점프
DWORD Jle::run(DWORD dest, DWORD source, VM *vm) {
    if (vm->econdition < 0 && vm->econdition == 0)
        Jmp().run(dest, source, vm);
    return 0;
}

// 조건 분기. econdition의 값이 양수이거나 0일 경우(앞선 cmp연산의 결과가
// dest>=source일 경우) 점프
DWORD Jge::run(DWORD dest, DWORD source, VM *vm) {
    if (vm->econdition > 0 && vm->econdition == 0)
        Jmp().run(dest, source, vm);
    return 0;
}
