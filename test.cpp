#include "ooasm.h"
#include "computer.h"
#include <string>
#include <sstream>
#include <cassert>
#include <iostream>

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
/*    auto lang_helloworld = program({
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
                                       mov(mem(mem(num(10))), num('d'))});*/

    std::string str;

    auto lang_helloworld = program({
                                       mov(mem(mem(num(10))), num('h')),
                                       inc(mem(num(10)))
    });
    Computer computer(11);
    computer.boot(lang_helloworld);
    str = memory_dump(computer);
    std::cout << str << std::endl;

    auto lang_helloworld2 = program({
                                       mov(mem(mem(num(10))), num('h')),
                                       inc(mem(num(10))),
                                       mov(mem(mem(num(10))), num('e')),
                                       inc(mem(num(10)))});
    computer.boot(lang_helloworld2);
    str = memory_dump(computer);
    std::cout << str << std::endl;

    lang_helloworld = program({
                                  mov(mem(mem(num(10))), num('h')),
                                  inc(mem(num(10))),
                                  mov(mem(mem(num(10))), num('e')),
                                  inc(mem(num(10))),
                                  mov(mem(mem(num(10))), num('l')),
                                  inc(mem(num(10)))});
    computer.boot(lang_helloworld);
    str = memory_dump(computer);
    std::cout << str << std::endl;

    lang_helloworld = program({
                                  mov(mem(mem(num(10))), num('h')),
                                  inc(mem(num(10))),
                                  mov(mem(mem(num(10))), num('e')),
                                  inc(mem(num(10))),
                                  mov(mem(mem(num(10))), num('l')),
                                  inc(mem(num(10))),
                                  mov(mem(mem(num(10))), num('l')),
                                  inc(mem(num(10)))});
    computer.boot(lang_helloworld);
    str = memory_dump(computer);
    std::cout << str << std::endl;
}
