#include "ooasm.h"
#include "computer.h"
#include <string>
#include <sstream>
#include <cassert>
#include <iostream>

// Funkcja z rafała.
namespace {
std::string memory_dump(Computer const& computer) {
    std::stringstream ss;
    computer.memory_dump(ss);
    // std::cout << ss.str() << "\n";
    return ss.str();
}
}

int main()
{
    auto lang_move = program({
                                 mov(mem(num(0)), num(42))});

    auto lang_data = program({
                                 inc(mem(lea(("a")))),
                                 data(("a"), num(0)),
                                 data(("b"), num(2)),
                                 data(("c"), num(3))});

    auto lang_operations = program({
                                       data(("a"), num(4)),
                                       data(("b"), num(3)),
                                       data(("c"), num(2)),
                                       data(("d"), num(1)),
                                       add(mem(lea(("a"))), mem(lea(("c")))),
                                       sub(mem(lea(("b"))), mem(lea(("d")))),
                                       mov(mem(lea(("c"))), num(0)),
                                       mov(mem(lea(("d"))), num(0))});

    auto lang_helloworld = program({
                                       mov(mem(mem(num(10))), num('h')),
                                       inc(mem(num(10))),
                                       mov(mem(mem(num(10))), num('e')),
                                       inc(mem(num(10))),
                                       mov(mem(mem(num(10))), num('l')),
                                       inc(mem(num(10))),
                                       mov(mem(mem(num(10))), num('l')),
                                       inc(mem(num(10))),
                                       mov(mem(mem(num(10))), num('o')),
                                       inc(mem(num(10))),
                                       mov(mem(mem(num(10))), num(' ')),
                                       inc(mem(num(10))),
                                       mov(mem(mem(num(10))), num('w')),
                                       inc(mem(num(10))),
                                       mov(mem(mem(num(10))), num('o')),
                                       inc(mem(num(10))),
                                       mov(mem(mem(num(10))), num('r')),
                                       inc(mem(num(10))),
                                       mov(mem(mem(num(10))), num('l')),
                                       inc(mem(num(10))),
                                       mov(mem(mem(num(10))), num('d'))});

    Computer computer1(1);
    computer1.boot(lang_move);
    assert(memory_dump(computer1) == "42 ");

    Computer computer2(3);
    computer2.boot(lang_data);
    assert(memory_dump(computer2) == "1 2 3 ");

    Computer computer3(4);
    computer3.boot(lang_operations);
    auto str2 = memory_dump(computer3);
    assert(str2 == "6 2 0 0 ");

    Computer computer4(11);
    computer4.boot(lang_helloworld);
    auto str = memory_dump(computer4);
    std::cout << str << std::endl;
    assert(str == "104 101 108 108 111 32 119 111 114 108 100 ");

    try {
        Computer computer5(10);
        computer5.boot(lang_helloworld);
        std::cout << 1 << "\n";

        assert(false);
    }
    catch(...)
    {
        assert(true);
    }


    try
    {
        std::cout << 2 << "\n";
        auto lang_id = program({data((""), num(4))});
        Computer computer6(10);
        computer6.boot(lang_id);
        assert(false);
    }
    catch(...)
    {
        assert(true);
    }

    try
    {
        std::cout << 3 << "\n";
        auto lang_long_id = program({data(("1234567890p"), num(4))});
        Computer computer7(10);
        computer7.boot(lang_long_id);
        assert(false);
    }
    catch(...)
    {
        assert(true);
    }

    auto wrong_id = program({add(mem(lea(("a"))),
                                 mem(lea(("c")))),
                             data(("2137"), num(4))});
    try {
        std::cout << 4 << "\n";

        Computer computer8(10);
        computer8.boot(wrong_id);
        assert(false);
    }
    catch(...)
    {
        assert(true);
    }

    auto twicer = program({
                              inc(mem(lea(("a")))),
                              data(("a"), num(0))});

    Computer computer9(1);
    computer9.boot(twicer);
    computer9.boot(twicer);
    assert(memory_dump(computer9) == "1 ");
}
