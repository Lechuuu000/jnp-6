#ifndef JNP_6_OOASM_H
#define JNP_6_OOASM_H

#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <vector>

const static size_t MAX_ID_LENGTH = 10;

// konwersja z const char* do std:: string zabezpieczająca przed bardzo długim identyfikatorem
static std::string convert_to_string(const char* text) {
    if(text == nullptr) {
        return "";
    }
    // zabezpieczam się przed bardzo długim id
    for(int i=0; i<=MAX_ID_LENGTH; i++) {
        if (text[i] == '\0')
            return std::string(text);
    }
    return std::string(text, text + MAX_ID_LENGTH + 1);
}

class EndOfProgramException : public std::exception {
  public:
    const char* what() const noexcept override {
        return "Asking for an instruction after reaching the end of the program is not allowed!";
    }
};

class OutOfBoundsException : public std::exception {
  public:
    const char* what() const noexcept override {
        return "Trying to access address outside of available memory!";
    }
};

class MemoryOverflowException : public std::exception {
  public:
    const char* what() const noexcept override {
        return "Memory limit has been exceeded!";
    }
};

class WrongVarNameException : public std::exception {
  public:
    const char* what() const noexcept override {
        return "Variable with given name does not exist!";
    }
};

class InvalidIdentifierException : public std::exception {
  public:
    const char* what() const noexcept override {
        return "Provided variable name does not follow language rules (1-10 characters)!";
    }
};

class Memory {
  public:
    Memory(size_t mem_size) : memory_array(mem_size, 0), var_addresses() {}
    void declare_variable(const std::string& id) {
        if (id.empty() || id.length() > MAX_ID_LENGTH)
            throw InvalidIdentifierException();
        if(next_address >= memory_array.size())
            throw MemoryOverflowException();

        var_addresses.insert({id, next_address});
    }
    int64_t get_address(const std::string& id) {
        auto it = var_addresses.find(id);
        if(it == var_addresses.end())
            throw WrongVarNameException();

        return it->second;
    }
    int64_t get_value(int64_t address) {}
    void set_variable(int64_t address, int64_t value) {

    }
    void dump(std::ostream& stream) {
        for (auto i : memory_array)
            stream << i << " ";
    }

  private:
    std::vector<int64_t> memory_array;
    std::unordered_map<std::string, size_t> var_addresses;
    size_t next_address = 0;
};

class Flags {
    bool ZF = false, SF = false;

  public:
    void set(int64_t result) {
        ZF = result == 0;
        SF = result < 0;
    }
    bool is_zero() const noexcept {
        return ZF;
    }
    bool is_signed() const noexcept {
        return SF;
    }
};

class Instruction {
  public:
    virtual void pre_evaluate(Memory&) const {}
    virtual void evaluate(Memory&, Flags&) const = 0;
};

class RValue {
  public:
    // todo być może trzeba zmienić tę metodę
    virtual int64_t value(Memory& memory) const noexcept = 0;
};

class num : public RValue {
  public:
    num(int64_t value) : m_value(value) {}
    int64_t value(Memory& memory) const noexcept override {
        return m_value;
    }

  private:
    const int64_t m_value;
};

class lea : public RValue {
    std::string id;
  public:
    lea(const char* text) : id(convert_to_string(text)) {}
    int64_t value(Memory &memory) const noexcept override {
        return memory.get_value(id);
    }

};

class LValue : public RValue {
  public:
    virtual void set(Memory& memory, const RValue&) = 0;
};

// todo do poprawy
class mem : public LValue {
    std::unique_ptr<RValue> rval_ptr;

  public:
    explicit mem(RValue&& rval) {}
    int64_t value(Memory& memory) const noexcept override {

    }
    void set(Memory& memory, const RValue& rval) override {

    }
};

class mov : public Instruction {};

class data : public Instruction {
  private:
    std::string id;
  public:
    data(const char* text) : id(convert_to_string(text)) {}
    void pre_evaluate(Memory& memory) const override {
        memory.declare_variable(id);
    }
    void evaluate(Memory& memory, Flags& flags) const override {}
};


class program {
  private:
    std::vector<Instruction> instructions;
    mutable size_t index_of_next = 0;

  public:
    program(std::initializer_list<Instruction> instrs) : instructions(instrs) {}
    const Instruction& get_next_instruction() {
        if (!has_next_instruction())
            throw EndOfProgramException();
        return instructions[index_of_next++];
    }
    bool has_next_instruction() const {
        return index_of_next < instructions.size();
    }
    void go_to_start() {
        index_of_next = 0;
    }
};

#endif // JNP_6_OOASM_H
