#include "nri.h"

NRI::NRI()
{
    mode = FETCH;
}

void NRI::clock_cycle()
{
    switch(mode) {
        case FETCH:
            fetch();
            break;
        case EXECUTE:
            if( timing_counter == MAX_COUNT-1 ){
                execute();
            }
            break;
        case HALT:
            return;
    }

    //We count up to 8, then switch from fetch to execute and vise versa
    timing_counter += 1;
    if( timing_counter == MAX_COUNT) {
        timing_counter = 0;
        if(mode == FETCH) {
            mode = EXECUTE;
            program_register += 1;
            if (program_register == MEM_SIZE) {
                program_register = 0;
            }
        }
        else if(mode == EXECUTE) {
            mode = FETCH;
        }
    }
}

void NRI::set_memory(int* new_memory) {
    for(int i = 0; i < MEM_SIZE; i++){
        memory[i] = new_memory[i];
    }
}

Mode NRI::get_mode() {
    return mode;
}

unsigned int NRI::get_accumulator() {
    return accumulator;
}

unsigned int NRI::get_program_register() {
    return program_register;
}

unsigned int NRI::get_timing_counter() {
    return timing_counter;
}

void NRI::fetch()
{
    memory_address_register = program_register;
    instruction_register = memory[memory_address_register];
}

void NRI::execute()
{
    int opcode1 = instruction_register>>5;
    int address = instruction_register&0b11111;
    int opcode2 = instruction_register&0b111;

    memory_address_register = address;

    if (opcode1 != 7) {
        switch(opcode1) {
            //LDA
            case 0:
                LDA();
                std::cout<<"LDA"<<std::endl;
                break;
            //STA
            case 1:
                STA();
                std::cout<<"STA"<<std::endl;
                break;
            //ADD
            case 2:
                ADD();
                std::cout<<"ADD"<<std::endl;
                break;
            //SUB
            case 3:
                SUB();
                std::cout<<"SUB"<<std::endl;
                break;
            //JMP
            case 4:
                JMP();
                std::cout<<"JMP"<<std::endl;
                break;
            //JOM
            case 5:
                JOM();
                std::cout<<"JOM"<<std::endl;
                break;
            //JOZ
            case 6:
                JOZ();
                std::cout<<"JOZ"<<std::endl;
                break;
        }
    }
    else {
        switch(opcode2) {
            //RAB
            case 0:
                RAB();
                std::cout<<"RAB"<<std::endl;
                break;
            //SHA
            case 1:
                SHA();
                std::cout<<"SHA"<<std::endl;
                break;
            //AND
            case 2:
                AND();
                std::cout<<"AND"<<std::endl;
                break;
            //DCA
            case 3:
                DCA();
                std::cout<<"DCA"<<std::endl;
                break;
            //CMA
            case 4:
                CMA();
                std::cout<<"CMA"<<std::endl;
                break;
            //RAE
            case 6:
                RAE();
                std::cout<<"RAE"<<std::endl;
                break;
            //HLT
            case 7:
                HLT();
                std::cout<<"HLT"<<std::endl;
                break;
        }
    }
}

void NRI::LDA() {
    accumulator = memory[memory_address_register];
}

void NRI::STA() {
    if (memory_address_register >= RAM_BEGINS) {
        memory[memory_address_register] = accumulator;
    }
}

void NRI::ADD() {
    accumulator = safe_add(accumulator, memory[memory_address_register]);
}

void NRI::SUB() {
    accumulator = safe_sub(accumulator, memory[memory_address_register]);
}

void NRI::JMP() {
    program_register = memory_address_register;
}

void NRI::JOM() {
    if (accumulator > 127) { program_register = memory_address_register; }
}

void NRI::JOZ() {
    if (accumulator == 0) { program_register = memory_address_register; }
}

void NRI::RAB() {
    unsigned int temp = accumulator;
    accumulator = register_b;
    register_b = temp;
}

void NRI::SHA() {
    unsigned int i = memory_address_register>>4;
    if(i == 0){ accumulator = accumulator>>1; }
    else      { accumulator = accumulator<<1; }
}

void NRI::AND() {
    accumulator = accumulator & register_b;
}

void NRI::DCA() {
    accumulator = safe_sub(accumulator, 1);
}

void NRI::CMA() {
    accumulator = ones_complement(accumulator);
}

void NRI::RAE() {
    unsigned int temp = accumulator;
    accumulator = register_e;
    register_e = temp;
}

void NRI::HLT() {
    mode = HALT;
}

unsigned int NRI::safe_add(unsigned int a, unsigned int b) {
    unsigned int result = a + b;
    result = result & 0xFF;
    return result;
}

unsigned int NRI::safe_sub(unsigned int a, unsigned int b) {
    unsigned int result = a - b;
    result = result & 0xFF;
    return result;
}

unsigned int NRI::ones_complement(unsigned int x) {
    unsigned int result = ~x;
    result = result & 0xFF;
    return result;
}