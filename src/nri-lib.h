/*
  nri.h - Library for emulating the NRI 832
  Created 2026-7-12
*/

#ifndef nri_h
#define nri_h

const unsigned int MEM_SIZE = 32;

enum Mode {
    HALT,
    FETCH,
    EXECUTE
};

class NRI
{
    public:
        NRI();

        void clock_cycle();

        void load();
        void start();
        void reset();
        
        void set_memory(int new_data, int address);
        void set_accumulator(int new_accumulator);

        unsigned int get_accumulator();
        unsigned int get_instruction_register();
        unsigned int get_program_register();

        bool is_overflow();
        bool is_executing();
    
    private:
        const unsigned int MAX_COUNT = 8;
        const unsigned int RAM_BEGINS = 16;

        Mode mode;

        unsigned int accumulator = 0;
        unsigned int register_b = 0;
        unsigned int register_e = 0;

        unsigned int instruction_register = 0;
        unsigned int memory_address_register = 0;
        unsigned int program_register = 0;

        unsigned int timing_counter = 0;

        unsigned int memory[MEM_SIZE];

        bool overflow = false;

        void fetch();
        void execute();

        //OPCODES

        void LDA();

        void STA();

        void ADD();

        void SUB();

        void JMP();

        void JOM();

        void JOZ();

        void RAB();

        void SHA();

        void AND();

        void DCA();

        void CMA();

        void RAE();

        void HLT();

        //Utilities
        unsigned int safe_add(unsigned int a, unsigned int b);
        unsigned int safe_sub(unsigned int a, unsigned int b);

        unsigned int ones_complement(unsigned int x);

};

#endif
