#include "vm.h"

VM::VM(char *opcode) {
    // �޸� ���� Ȯ��(4GB)
    m_Memory = (WORD *)malloc(0x99999999);
    if (m_Memory == nullptr)
        return;
    memset((void *)m_Memory, 0, 0x99999999);

    // ������� �б�
    header = (FILE_HEADER *)opcode;
    // ENTRYPOINT�� OPCODE ����
    memcpy(m_Memory + ENTRYPOINT, opcode + FILEHEADER_SIZE, header->code_size);
    // DATA ���� ����
    memcpy(m_Memory + DATA_IMAGEBASE,
                 opcode + FILEHEADER_SIZE + header->code_size, header->data_size);

    // operation Table �ʱ�ȭ
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

    // ���� ����
    ebp = STACK_IMAGEBASE;
    esp = STACK_IMAGEBASE;

    // EIP�� EP�� ����
    eip = ENTRYPOINT;

    char *str = (char *)&m_Memory[DATA_IMAGEBASE];
    strcpy_s(str, 20, "Hello World!");
}

VM::~VM() {
    // Ȯ���� �޸� ���� ����
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
    while (m_Memory[eip]) { // ���� �޸� ����� eip���� 0�� ���ö����� �ݺ��Ѵ�.
        execute();
    }
    return 0;
}

DWORD *VM::getPointer(DWORD address) { 
        return (DWORD *)&(m_Memory[address]);                    // VM�� �޸� �ּҸ� ���� �޸� �ּҷ� ��ȯ
}


// �޸𸮿� ���� �����ϴ� instruction�� ���, 
// dest�� �ش��ϴ� ���� ���������� ũ�⺸�� ���� �� 
// �ش� dest ���� �ش��ϴ� register�� ����Ѵ�. 

// �������Ϳ� source�� ��Ģ������ �����Ѵ�. add eax, 7�� ���� ����.
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

// dest�� ���������� ���� �ش� �������Ͱ� ����Ű�� ���ڿ���, 
// �޸� �ּҸ� ����ų ��� �ش� �޸� �ּҰ� ����Ű�� ���ڿ��� ����Ѵ�.
DWORD PrintStr::run(DWORD dest, DWORD source, VM *vm) {
    if (dest < REGISTER_SIZE)
        dest = vm->m_Registers[dest];
    DWORD *destination = vm->getPointer(dest);
    const char *string = (const char *)destination;
    puts(string);
    return 0;
}

// ����� ���������� ���� ���������� ���� ����ϰ�, 
// ����� �޸� �ּ��� ��� �ش� �޸� �ּҿ� ��� ���� ���� ����Ѵ�.
DWORD PrintInt::run(DWORD dest, DWORD source, VM *vm) {
    if (dest < REGISTER_SIZE) { // ���������� ���
        printf("%d\n", vm->m_Registers[dest]);
        return 0;
    }
    DWORD *destination = vm->getPointer(dest);
    printf("%d", *destination);
    return 0;
}

// Jump����
DWORD Jmp::run(DWORD dest, DWORD source, VM *vm) {
    vm->eip = dest - OPCODE_SIZE; // Jmp������ EIP�� ��Ȯ�� �ش� ��ġ�� �̵����Ѿ�
                                  // �ϴµ� execute ���� EIP���� ���ڵ� ũ�⸦
                                  // ���ϹǷ� ���ڵ� ũ�⸦ �̸� �����´�.
    return 0;
}

// Compare ����. ���ǿ� ���� ECONDITION�� ���� �����Ѵ�. 
// ECONDITION�� 0�� ��� dest, source�� ���� ����
// ECONDITION�� ����� ��� dest�� ���� �� ŭ
// ECONDITION�� ������ ��� dest�� ���� �� ����
DWORD Cmp::run(DWORD dest, DWORD source, VM *vm) { // �������Ϳ� �޸𸮰� ��
    DWORD *src = vm->getPointer(source);
    return vm->m_Registers[ECONDITION] = vm->m_Registers[dest] - *src;
}


// EBP + source, EBP - source�� ���� ��� ����. ��, ���� ������ �Ű� ������ �����ϴ�
// ������ �����Ѵ�. 
DWORD MovS::run(DWORD dest, DWORD source, VM *vm) { // mov eax, ebp+source�� ���� ���� ��츦 ������
    int *src = (int*)vm->getPointer(vm->ebp + source);
    if (dest < REGISTER_SIZE) // dest�� ���� ���������� ���
        vm->m_Registers[dest] = *src;
    else { // dest�� ���� �޸� ���� ���� ���
        DWORD *destination = vm->getPointer(dest);
        *destination = *src;
    }
    return 0;
}

// source�� ���� register�� ��� ����ϴ� ��ɾ�. ex) mov eax, ebx
DWORD MovR::run(DWORD dest, DWORD source, VM *vm) { // source�� ���������� ���
    if (dest < REGISTER_SIZE) // dest�� ���� ���������� ���
        vm->m_Registers[dest] = vm->m_Registers[source];
    else { // dest�� ���� �޸� ���� ���� ���
        DWORD *destination = vm->getPointer(dest);
        *destination = vm->m_Registers[source];
    }
    return 0;
}


// source�� �ִ� ���� �ܼ� ���� ���. ex) mov eax, 7 or mov 0x12345678, 8 ���..
// data ���ǿ� �ִ� ���� ������ ���� ������ �� Ȱ���� �� �ִ�. 
DWORD Mov::run(DWORD dest, DWORD source, VM *vm) {
    if (dest < REGISTER_SIZE) // dest�� ���� ���������� ���
        vm->m_Registers[dest] = source;
    else { // dest�� ���� �޸� ���� ���� ���
        DWORD *destination = vm->getPointer(dest);
        *destination = source;
    }

    return 0;
}

// ���ÿ� push�ϴ� ����. source ���� ������� �ʴ´�. 
// �ش� ������ ����� esp�� 4 �پ���.
DWORD Push::run(DWORD dest, DWORD source, VM *vm) {
    vm->esp -= 4;
    DWORD *pointer = vm->getPointer(vm->esp);
    *pointer = dest;
    return 0;
}

// ���ÿ� push�ϴ� ����. �ش� ������ �������͸� push�Ѵ�.
DWORD PushR::run(DWORD dest, DWORD source, VM *vm) {
    DWORD destination = vm->m_Registers[dest];
    Push().run(destination, source, vm);
    return 0;
}


// dest�� �ش��ϴ� �޸� �ּҷ� pop�ϴ� ����.
// ���������� ��� �������ͷ� pop�Ѵ�. 
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

// �Լ� call ������. dest�� �ش��ϴ� �ּ��� 
// �Լ��� call�Ѵ�. ���� eip �ּ� + opcode_size�� ���ÿ� push�ϰ� 
// �Լ� �ּҷ� Jump�Ѵ�.
DWORD Call::run(DWORD dest, DWORD source, VM *vm) {
    Push().run(vm->eip + OPCODE_SIZE, source, vm);
    Jmp().run(dest, source, vm);
    return 0;
}

// �Լ� return ������. 
// ���� esp�� ���� ebp�� �����ϰ�, ���� ebp�� caller�� 
// ebp�� ������ ���� �Լ� ȣ�� ���� EIP �������ͷ�
// �����Ѵ�. 
DWORD Ret::run(DWORD dest, DWORD source, VM *vm) {
    MovR().run(ESP, EBP, vm);
    Pop().run(EBP, source, vm);
    Pop().run(EIP, source, vm);
    vm->eip -= OPCODE_SIZE;
    return 0;
}

// dest, source�� ��� ���������� ��쿡���� 
// ��Ģ������ �����Ѵ�. 
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


// ���� �б�. econdition�� ���� 0�̸�(�ռ� cmp������ ����� 
// dest==source ���) ����. 
DWORD Jeq::run(DWORD dest, DWORD source, VM *vm) {
    if (vm->econdition == 0)
        Jmp().run(dest, source, vm);
    return 0;
}

// ���� �б�. econdition�� ���� ����(�ռ� cmp������ ����� 
// dest>source�� ���) ����.
DWORD Jl::run(DWORD dest, DWORD source, VM *vm) {
    if (vm->econdition < 0)
        Jmp().run(dest, source, vm);
    return 0;
}


// ���� �б�. econdition�� ���� ���(�ռ� cmp������ �����
// dest>source�� ���) ����
DWORD Jg::run(DWORD dest, DWORD source, VM *vm) {
    if (vm->econdition > 0)
        Jmp().run(dest, source, vm);
    return 0;
}

// ���� �б�. econdition�� ���� �����̰ų� 0�� ���(�ռ� cmp������ �����
// dest<=source�� ���) ����
DWORD Jle::run(DWORD dest, DWORD source, VM *vm) {
    if (vm->econdition < 0 && vm->econdition == 0)
        Jmp().run(dest, source, vm);
    return 0;
}

// ���� �б�. econdition�� ���� ����̰ų� 0�� ���(�ռ� cmp������ �����
// dest>=source�� ���) ����
DWORD Jge::run(DWORD dest, DWORD source, VM *vm) {
    if (vm->econdition > 0 && vm->econdition == 0)
        Jmp().run(dest, source, vm);
    return 0;
}
