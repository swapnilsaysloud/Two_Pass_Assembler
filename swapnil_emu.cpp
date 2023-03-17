/*
NAME: Swapnil Saraswat
ROLL NO: 2101CS76
COURSE: CS209-CS210 Architechture LAB
INSTRUCTOR NAME: Dr. Jimson Matthew
declaration of authorship: I hereby declare this assemler is made by me as part of the mini project for this course

*/
#include <bits/stdc++.h>
using namespace std;

// Memory with 2^24 addresses
int Memory[1 << 24];
string Mnemonic[] = {"ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub", "shl", "shr", "adj", "a2sp", "sp2a", "call", "return", "brz", "brlz", "br", "HALT"};

// The stack pointer, program counter and the registers
int Stack_Pointer, Program_counter, Register_A, Register_B;

// UTILITY FUNCTIONS
//  Print ISA
void isa()
{
    cout << "OpMachineCode Mnemonic Operand" << '\n';
    cout << "0      ldc      value " << endl;
    cout << "1      adc      value " << endl;
    cout << "2      ldl      value " << endl;
    cout << "3      stl      value " << endl;
    cout << "4      ldnl     value " << endl;
    cout << "5      stnl     value " << endl;
    cout << "6      add            " << endl;
    cout << "7      sub            " << endl;
    cout << "9      shr            " << endl;
    cout << "10     adj      value " << endl;
    cout << "11     a2sp           " << endl;
    cout << "12     sp2a           " << endl;
    cout << "13     call     offset" << endl;
    cout << "14     return         " << endl;
    cout << "15     brz      offset" << endl;
    cout << "16     brlz     offset" << endl;
    cout << "17     br       offset" << endl;
    cout << "18     HALT           " << endl;
}
// Memory Dump until the number of instructions/data provided
void printMemory(int line_num, bool isBefore)
{
    if (isBefore)
        cout << "Memory dump before execution\n";
    else
        cout << "Memory dump after execution\n";
    for (int i = 0; i < line_num; i++)
    {
        if (i % 4 == 0)
            fprintf(stderr, "\n%08x ", i);
        fprintf(stderr, "%08x ", Memory[i]);
    }
    cout << endl;
    return;
}
void ldc(int value)
{
    Register_B = Register_A;
    Register_A = value;
}

void adc(int value)
{
    Register_A += value;
}

void ldl(int offset)
{
    Register_B = Register_A;
    Register_A = Memory[Stack_Pointer + offset];
}

void stl(int offset)
{
    Memory[Stack_Pointer + offset] = Register_A;
    Register_A = Register_B;
}

void ldnl(int offset)
{
    Register_A = Memory[Register_A + offset];
}

void stnl(int offset)
{
    Memory[Register_A + offset] = Register_B;
}

void add()
{
    Register_A += Register_B;
}

void sub()
{
    Register_A = Register_B - Register_A;
}

void shl()
{
    Register_A = Register_B << Register_A;
}

void shr()
{
    Register_A = Register_B >> Register_A;
}

void adj(int value)
{
    Stack_Pointer += value;
}

void a2sp()
{
    Stack_Pointer = Register_A;
    Register_A = Register_B;
}

void sp2a()
{
    Register_B = Register_A;
    Register_A = Stack_Pointer;
}

void call(int offset)
{
    Register_B = Register_A;
    Register_A = Program_counter;
    Program_counter += offset;
}

void RETURN()
{
    Program_counter = Register_A;
    Register_A = Register_B;
}

void brz(int offset)
{
    if (Register_A == 0)
        Program_counter += offset;
}

void brlz(int offset)
{
    if (Register_A < 0)
        Program_counter += offset;
}

void br(int offset)
{
    Program_counter += offset;
}
// Convert 32 bit binary string to number
int binToNumber(char *bin)
{
    int number = 0;
    for (int i = 0; i < 32; i++)
    {
        number *= 2;
        if (bin[i] == '1')
            number += 1;
    }
    return number;
}

// Read machine code into memory
int readBinary(FILE *machine)
{
    char *instruction = (char *)malloc(36);
    int line_num = 0;

    // READ LINES INTO INSTRUCTION STRING WHEN AVAILABLE
    while (fgets(instruction, 36, machine))
    {
        // Store instruction in memory location
        Memory[line_num] = binToNumber(instruction);

        line_num += 1;
    }
    return line_num;
}

// Extract mnemonic code from instruction
int getOperand(int instruction)
{
    return (instruction >> 8);
}
// Extract mnemonic code from instruction
int getMnemonicCode(int instruction)
{
    return ((1 << 8) - 1) & instruction;
}

void printTrace(int opcode, int operand)
{
    fprintf(stderr, "PC=%08x, SP=%08x, A=%08x, B=%08x %s\t%08x\n", Program_counter, Stack_Pointer, Register_A, Register_B, Mnemonic[opcode], operand);
}

int execute_Instruction(int opcode, int operand)
{
    if (opcode == 0)
        ldc(operand);
    if (opcode == 1)
        adc(operand);
    if (opcode == 2)
        ldl(operand);
    if (opcode == 3)
        stl(operand);
    if (opcode == 4)
        ldnl(operand);
    if (opcode == 5)
        stnl(operand);
    if (opcode == 6)
        add();
    if (opcode == 7)
        sub();
    if (opcode == 8)
        shl();
    if (opcode == 9)
        shr();
    if (opcode == 10)
        adj(operand);
    if (opcode == 11)
        a2sp();
    if (opcode == 12)
        sp2a();
    if (opcode == 13)
        call(operand);
    if (opcode == 14)
        RETURN();
    if (opcode == 15)
        brz(operand);
    if (opcode == 16)
        brlz(operand);
    if (opcode == 17)
        br(operand);
    else
        return 0;
    return 1;
}

// Executes the machine code and prints out the trace if required
// RETURN 1: Invalid Opcode
int execute(bool trace)
{
    Stack_Pointer = (1 << 23) - 1;
    int operand = 0, opcode = 0;
    int instruction_num = 0;

    while ((opcode = getMnemonicCode(Memory[Program_counter])) <= 18)
    {

        operand = getOperand(Memory[Program_counter]);

        if (trace)
            printTrace(opcode, operand);
        if (opcode == 18)
            break;
        // Invalid Opcode
        if (opcode > 18)
            return 1;

        Program_counter += 1;
        execute_Instruction(opcode, operand);
        instruction_num += 1;
    }
    cout<<instruction_num<<" instructions executed"<<endl;
    return 0;
}

int main(int argc, char *argv[])
{
    FILE *ObjectFile;

    if (argc < 3)
    {
       cout<<"Please enter a instruction like -trace, -before, -after, -isa "<<endl;
       cout<<"format: ./exe instruction obejctFile.o"<<endl;
        return 1;
    }
    string instr = argv[1];
    ObjectFile = fopen(argv[2], "rb");

    bool trace = (instr.compare("-trace") == 0) ? true : false;
    bool Dump_Before = (instr.compare("-before") == 0) ? true : false;
    bool Dump_after = (instr.compare("-after") == 0) ? true : false;
    bool ISA = (instr.compare("-isa") == 0) ? true : false;

    if (!(trace || Dump_Before || Dump_after || ISA))
    {
        cout<<"Please enter valid instruction"<<endl;
       cout<<"-trace\tshow instruction trace"<<endl;
       cout<<"-before\tshow memory dump before execution"<<endl;
       cout<<"-after\tshow memory dump after execution"<<endl;
       cout<< "-isa\tdisplay ISA"<<endl;
        return 1;
    }

    int instructions_cnt = readBinary(ObjectFile);
    // If -before then print
    if (Dump_Before)
        printMemory(instructions_cnt, 1);
    if (ISA)
        isa();

    int a = execute(trace);
    if(a==1) cout<<"INVALID OPCODE"<<endl;
    if (Dump_after)
        printMemory(instructions_cnt, 0);

    // Close the assembly file
    fclose(ObjectFile);
}