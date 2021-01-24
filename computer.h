#ifndef JNP_6_COMPUTER_H
#define JNP_6_COMPUTER_H

#include <iostream>
#include <map>
#include "ooasm.h"

class Computer {
  public:
    explicit Computer(size_t mem_size) : memory(mem_size) , flags(){}
    void boot(program& prog) {
        // variable declarations
        while (prog.has_next_instruction()) {
            const Instruction& instruction = prog.get_next_instruction();
            instruction.pre_evaluate(memory);
        }
        prog.go_to_start();
        // executing program
        while (prog.has_next_instruction()) {
            const Instruction& instruction = prog.get_next_instruction();
            instruction.evaluate(memory, flags);
        }
    }
    void memory_dump(std::ostream& stream) const {
        memory.dump(stream);
    }

  private:
    Memory memory;
    Flags flags;
};

#endif // JNP_6_COMPUTER_H
