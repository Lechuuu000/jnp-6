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
    // zabezpieczam się przed bardzo długim id
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
    void validate_address(int64_t address) {
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
    int64_t get_address(const std::string& id) {
        auto it = var_addresses.find(id);
        if (it == var_addresses.end())
            throw WrongVarNameException();

        return it->second;
    }
    int64_t get_value(int64_t address) {
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
        for (auto i : memory_array)
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
    virtual std::unique_ptr<RValue> give_ownership() = 0;
    virtual ~RValue() = default;
};

class LValue : public RValue {
  public:
    virtual int64_t get_address(Memory& memory) const = 0;
    virtual std::unique_ptr<LValue> give_lval_ownership() = 0;
    virtual ~LValue() = default;
};

class num : public RValue {
  public:
    num(int64_t value) : m_value(value) {}
    int64_t value(Memory& memory) const noexcept override {
        return m_value;
    }
    std::unique_ptr<RValue> give_ownership() override {
        return std::make_unique<num>(m_value);
    }

  private:
    const int64_t m_value;
};

class lea : public RValue {
    std::string id;
    lea(std::string&& text) : id(text) {}

  public:
    explicit lea(const char* text) : id(convert_to_string(text)) {}
    int64_t value(Memory& memory) const noexcept override {
        return memory.get_address(id);
    }
    std::unique_ptr<RValue> give_ownership() override {
        return std::unique_ptr<lea>(new lea(std::move(id)));
    }
};

class mem : public LValue {
    std::unique_ptr<RValue> addr_ptr;
    mem(std::unique_ptr<RValue>&& ptr) : addr_ptr(std::move(ptr)) {}
  public:
    explicit mem(RValue&& rval) : addr_ptr(rval.give_ownership()) {}
    //mem(mem&& m) = delete;
    int64_t value(Memory& memory) const noexcept override {
        auto val = memory.get_value(addr_ptr->value(memory));
        return val;
    }
    int64_t get_address(Memory& memory) const override {
        return addr_ptr->value(memory);
    }
    std::unique_ptr<RValue> give_ownership() override {

        return std::unique_ptr<mem>(new mem(std::move(addr_ptr)));
        // return std::unique_ptr<mem>(new mem(std::move(addr_ptr)));
    }
    std::unique_ptr<LValue> give_lval_ownership() override {
        // return std::unique_ptr<mem>(new mem(std::move(addr_ptr)));
        //return std::make_unique<mem>( mem(std::move(addr_ptr)));
        return std::unique_ptr<mem>(new mem(std::move(addr_ptr)));

    }
};

class Instruction {
  public:
    virtual void pre_evaluate(Memory&) const {}
    virtual void evaluate(Memory&, Flags&) const = 0;
    virtual std::unique_ptr<Instruction> give_ownership() = 0;
    virtual ~Instruction() = default;
};

class ArithmeticOperation {
  public:
    virtual int64_t compute(int64_t, int64_t) const = 0;
};

class BinaryOperation : public Instruction, public ArithmeticOperation {
  protected:
    std::unique_ptr<LValue> arg1_ptr;
    std::unique_ptr<RValue> arg2_ptr;
    BinaryOperation(LValue&& lval, RValue&& rval)
        : arg1_ptr(lval.give_lval_ownership()), arg2_ptr(rval.give_ownership()) {}
    BinaryOperation(BinaryOperation&& op)
        : arg1_ptr(std::move(op.arg1_ptr)), arg2_ptr(std::move(op.arg2_ptr)) {}

  public:
    void evaluate(Memory& memory, Flags& flags) const override {
        auto result = compute(arg1_ptr->value(memory), arg2_ptr->value(memory));
        flags.set(result);
        memory.set_variable(arg1_ptr->get_address(memory), result);
    }
    virtual ~BinaryOperation() = default;
};

class add : public BinaryOperation {
  public:
    add(LValue&& lval, RValue&& rval) : BinaryOperation(std::move(lval), std::move(rval)) {}
    // add(add&& a) = default;
    int64_t compute(int64_t arg1, int64_t arg2) const override {
        return arg1 + arg2;
    }
    std::unique_ptr<Instruction> give_ownership() override {
        return std::make_unique<add>(std::move(*this));
    }
};

class sub : public BinaryOperation {
  public:
    sub(LValue&& lval, RValue&& rval) : BinaryOperation(std::move(lval), std::move(rval)) {}
    int64_t compute(int64_t arg1, int64_t arg2) const override {
        return arg1 - arg2;
    }
    std::unique_ptr<Instruction> give_ownership() override {
        return std::make_unique<sub>(std::move(*this));
    }
};

class inc : public add {
  public:
    explicit inc(LValue&& lval) : add(std::move(lval), num(1)) {}
};

class dec : public sub {
  public:
    explicit dec(LValue&& lval) : sub(std::move(lval), num(1)) {}
};

class Assignment : public Instruction {
  protected:
    std::unique_ptr<LValue> arg_ptr;
    Assignment(LValue&& lval) : arg_ptr(lval.give_lval_ownership()) {}
    Assignment(Assignment&& a) : arg_ptr(std::move(a.arg_ptr)) {}
};

class mov : public Assignment {
    std::unique_ptr<RValue> val_ptr;

  public:
    mov(LValue&& arg1, RValue&& arg2)
        : Assignment(std::move(arg1)), val_ptr(arg2.give_ownership()) {}
    void evaluate(Memory& memory, Flags& flags) const override {
        memory.set_variable(arg_ptr->get_address(memory), val_ptr->value(memory));
    }
    std::unique_ptr<Instruction> give_ownership() override {
        return std::make_unique<mov>(std::move(*this));
    }
};

class one : public Assignment {
  public:
    one(LValue&& lval) : Assignment(std::move(lval)) {}
    void evaluate(Memory& memory, Flags& flags) const override {
        memory.set_variable(arg_ptr->get_address(memory), 1);
    }
    std::unique_ptr<Instruction> give_ownership() override {
        return std::make_unique<one>(std::move(*this));
    }
};

class ones : public one {
  public:
    ones(LValue&& lval) : one(std::move(lval)) {}
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
    onez(LValue&& lval) : one(std::move(lval)) {}
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
    data(const char* text, RValue&& rval)
        : id(convert_to_string(text)), rval_ptr(rval.give_ownership()) {}
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
