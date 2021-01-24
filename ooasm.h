#ifndef JNP_6_OOASM_H
#define JNP_6_OOASM_H

#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

const static size_t MAX_ID_LENGTH = 10;

// conversion from const char* to std::string, avoiding copying very long identifiers
static std::string convert_to_string(const char* text) {
    if (text == nullptr) {
        return "";
    }
    for (int i = 0; i <= MAX_ID_LENGTH; i++) {
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
  private:
    void validate_address(int64_t address) const {
        if (address < 0 || address >= memory_array.size())
            throw OutOfBoundsException();
    }

  public:
    Memory(size_t mem_size) : memory_array(mem_size, 0), var_addresses() {}
    void declare_variable(const std::string& id, int64_t value) {
        if (id.empty() || id.length() > MAX_ID_LENGTH)
            throw InvalidIdentifierException();
        if (next_address >= memory_array.size())
            throw MemoryOverflowException();

        memory_array[next_address] = value;
        var_addresses.insert({id, next_address++});
    }
    int64_t get_address(const std::string& id) const{
        auto it = var_addresses.find(id);
        if (it == var_addresses.end())
            throw WrongVarNameException();

        return it->second;
    }
    int64_t get_value(int64_t address) const {
        validate_address(address);
        return memory_array[address];
    }
    void set_variable(int64_t address, int64_t value) {
        validate_address(address);
        memory_array[address] = value;
    }
    void dump(std::ostream& stream) const {
        for (auto i : memory_array)
            stream << i << " ";
    }
    void reset() {
        for (auto& i : memory_array)
            i = 0;
        var_addresses.clear();
        next_address = 0;
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

class RValue {
  public:
    virtual int64_t value(Memory& memory) const noexcept = 0;
    virtual ~RValue() = default;
};

class LValue : public RValue {
  public:
    virtual int64_t get_address(Memory& memory) const = 0;
    virtual ~LValue() = default;
};

class Num : public RValue {
  public:
    Num(int64_t value) : m_value(value) {}
    int64_t value(Memory& memory) const noexcept override {
        return m_value;
    }
  private:
    const int64_t m_value;
};

std::unique_ptr<Num> num(int64_t value) {
    return std::make_unique<Num>(value);
}


class Lea : public RValue {
    std::string id;
    Lea(std::string&& text) : id(text) {}

  public:
    explicit Lea(const char* text) : id(convert_to_string(text)) {}
    int64_t value(Memory& memory) const noexcept override {
        return memory.get_address(id);
    }
};

std::unique_ptr<Lea> lea(const char* text) {
    return std::make_unique<Lea>(text);
}

class Mem : public LValue {
    std::unique_ptr<RValue> addr_ptr;
  public:
    Mem(std::unique_ptr<RValue>&& ptr) : addr_ptr(std::move(ptr)) {}
    Mem(Mem&& m) = delete;
    int64_t value(Memory& memory) const noexcept override {
        return memory.get_value(addr_ptr->value(memory));
    }
    int64_t get_address(Memory& memory) const override {
        return addr_ptr->value(memory);
    }
};

std::unique_ptr<Mem> mem(std::unique_ptr<RValue> ptr) {
    return std::make_unique<Mem>(std::move(ptr));
}


class Instruction {
  public:
    virtual void pre_evaluate(Memory&) const {}
    virtual void evaluate(Memory&, Flags&) const = 0;
    virtual std::unique_ptr<Instruction> give_ownership() = 0;
    virtual ~Instruction() = default;
};



class ArithmeticOperation : public Instruction {
  protected:
    std::unique_ptr<LValue> arg1_ptr;
    std::unique_ptr<RValue> arg2_ptr;
    ArithmeticOperation(std::unique_ptr<LValue> lval, std::unique_ptr<RValue> rval)
        : arg1_ptr(std::move(lval)), arg2_ptr(std::move(rval)) {}
    ArithmeticOperation(ArithmeticOperation&& op)
        : arg1_ptr(std::move(op.arg1_ptr)), arg2_ptr(std::move(op.arg2_ptr)) {}

  public:
    virtual int64_t compute(int64_t, int64_t) const = 0;
    void evaluate(Memory& memory, Flags& flags) const override {
        auto result = compute(arg1_ptr->value(memory), arg2_ptr->value(memory));
        flags.set(result);
        memory.set_variable(arg1_ptr->get_address(memory), result);
    }
    virtual ~ArithmeticOperation() = default;
};

class add : public ArithmeticOperation {
  public:
    add(std::unique_ptr<LValue> lval, std::unique_ptr<RValue> rval)
        : ArithmeticOperation(std::move(lval), std::move(rval)) {}
    // add(add&& a) = default;
    int64_t compute(int64_t arg1, int64_t arg2) const override {
        return arg1 + arg2;
    }
    std::unique_ptr<Instruction> give_ownership() override {
        return std::make_unique<add>(std::move(*this));
    }
};

class sub : public ArithmeticOperation {
  public:
    sub(std::unique_ptr<LValue> lval, std::unique_ptr<RValue> rval)
        : ArithmeticOperation(std::move(lval), std::move(rval)) {}
    int64_t compute(int64_t arg1, int64_t arg2) const override {
        return arg1 - arg2;
    }
    std::unique_ptr<Instruction> give_ownership() override {
        return std::make_unique<sub>(std::move(*this));
    }
};

class inc : public add {
  public:
    explicit inc(std::unique_ptr<LValue> lval) : add(std::move(lval), num(1)) {}
};

class dec : public sub {
  public:
    explicit dec(std::unique_ptr<LValue> lval) : sub(std::move(lval), num(1)) {}
};

class Assignment : public Instruction {
  protected:
    std::unique_ptr<LValue> arg_ptr;
    Assignment(std::unique_ptr<LValue> lval) : arg_ptr(std::move(lval)) {}
    Assignment(Assignment&& a) : arg_ptr(std::move(a.arg_ptr)) {}
};

class mov : public Assignment {
    std::unique_ptr<RValue> val_ptr;

  public:
    mov(std::unique_ptr<LValue> lval, std::unique_ptr<RValue> rval)
        : Assignment(std::move(lval)), val_ptr(std::move(rval)) {}
    void evaluate(Memory& memory, Flags& flags) const override {
        memory.set_variable(arg_ptr->get_address(memory), val_ptr->value(memory));
    }
    std::unique_ptr<Instruction> give_ownership() override {
        return std::make_unique<mov>(std::move(*this));
    }
};

class one : public Assignment {
  public:
    one(std::unique_ptr<LValue> lval) : Assignment(std::move(lval)) {}
    void evaluate(Memory& memory, Flags& flags) const override {
        memory.set_variable(arg_ptr->get_address(memory), 1);
    }
    std::unique_ptr<Instruction> give_ownership() override {
        return std::make_unique<one>(std::move(*this));
    }
};

class ones : public one {
  public:
    ones(std::unique_ptr<LValue> lval) : one(std::move(lval)) {}
    void evaluate(Memory& memory, Flags& flags) const override {
        if (flags.is_signed())
            one::evaluate(memory, flags);
    }
    std::unique_ptr<Instruction> give_ownership() override {
        return std::make_unique<ones>(std::move(*this));
    }
};

class onez : public one {
  public:
    onez(std::unique_ptr<LValue>  lval) : one(std::move(lval)) {}
    void evaluate(Memory& memory, Flags& flags) const override {
        if (flags.is_zero())
            one::evaluate(memory, flags);
    }
    std::unique_ptr<Instruction> give_ownership() override {
        return std::make_unique<onez>(std::move(*this));
    }
};

class data : public Instruction {
  private:
    std::string id;
    std::unique_ptr<RValue> rval_ptr;

  public:
    data(const char* text, std::unique_ptr<RValue> rval)
        : id(convert_to_string(text)), rval_ptr(std::move(rval)) {}
    void pre_evaluate(Memory& memory) const override {
        memory.declare_variable(id, rval_ptr->value(memory));
    }
    void evaluate(Memory& memory, Flags& flags) const override {}
    std::unique_ptr<Instruction> give_ownership() override {
        return std::make_unique<data>(std::move(*this));
    }
};

class InstructionPtr {
    std::shared_ptr<Instruction> ptr; // unique_ptr causes problems with std::initializer_list
  public:
    InstructionPtr(Instruction&& instr) : ptr(instr.give_ownership()) {}
    const Instruction& get() {
        return *ptr;
    }
};

class program {
  private:
    std::vector<InstructionPtr> instructions;
    size_t index_of_next = 0;

  public:
    program(std::vector<InstructionPtr>&& vec) : instructions(std::move(vec)) {}
    const Instruction& get_next_instruction() {
        if (!has_next_instruction())
            throw EndOfProgramException();
        return instructions[index_of_next++].get();
    }
    bool has_next_instruction() const {
        return index_of_next < instructions.size();
    }
    void go_to_start() {
        index_of_next = 0;
    }
};

#endif // JNP_6_OOASM_H
