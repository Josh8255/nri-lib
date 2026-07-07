#include "nri.h"
#include <conio.h>

//Counts down to zero from adress 8
int countdown[32] = {0x08, 0xA4, 0xE3, 0x81, 0xFF, 0x00, 0x00, 0x00, 
                     0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

int main() {
    NRI* nri = new NRI();

    nri->set_memory(countdown);

    int count = 0;
    while(nri->get_mode() != HALT) {
        getch();
        std::cout<<nri->get_program_register()<<std::endl;
        std::cout<<nri->get_accumulator()<<std::endl;
        for(int i = 0; i < 16; i++) { nri->clock_cycle(); }
    }

    return 0;
}