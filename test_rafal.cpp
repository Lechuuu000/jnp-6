#include "ooasm.h"
#include "computer.h"
#include <string>
#include <sstream>
#include <cassert>

namespace {
std::string memory_dump(Computer const& computer) {
    std::stringstream ss;
    computer.memory_dump(ss);
    return ss.str();
}
}

int main() {
    auto ooasm_move = program({
                                  mov(mem(num(0)), num(42))
                              });
    Computer computer1(1);
    computer1.boot(ooasm_move);
    assert(memory_dump(computer1) == "42 ");

    auto ooasm_ones = program({
                                  dec(mem(num(1))),
                                  ones(mem(num(0)))
                              });
    Computer computer2(2);
    computer2.boot(ooasm_ones);
    assert(memory_dump(computer2) == "1 -1 ");

    auto ooasm_data = program({
                                  inc(mem(lea("a"))),
                                  data("a", num(0)),
                                  data("b", num(2)),
                                  data("c", num(3))
                              });
    Computer computer3(4);
    computer3.boot(ooasm_data);
    assert(memory_dump(computer3) == "1 2 3 0 ");

    auto ooasm_operations = program({
                                        data("a", num(4)),
                                        data("b", num(3)),
                                        data("c", num(2)),
                                        data("d", num(1)),
                                        add(mem(lea("a")), mem(lea("c"))),
                                        sub(mem(lea("b")), mem(lea("d"))),
                                        mov(mem(lea("c")), num(0)),
                                        mov(mem(lea("d")), num(0))
                                    });
    Computer computer4(5);
    computer4.boot(ooasm_operations);
    assert(memory_dump(computer4) == "6 2 0 0 0 ");

    auto ooasm_memmemmem = program({
                                       inc(mem(lea("a"))),
                                       inc(mem(mem(mem(mem(mem(lea("b"))))))),
                                       inc(mem(mem(mem(mem(mem(num(0))))))),
                                       data("a", num(0)),
                                       data("b", num(2)),
                                       data("c", num(3))
                                   });
    Computer computer5(4);
    computer5.boot(ooasm_memmemmem);
    assert(memory_dump(computer5) == "1 4 3 0 ");

    auto ooasm_bigger = program({
                                    inc(mem(lea("aaaaaaaaaa"))),
                                    dec(mem(mem(mem(mem(mem(lea("aaaaaaaaaa"))))))),
                                    inc(mem(lea("aaaaaaaaaa"))),
                                    one(mem(mem(mem(mem(mem(num(4))))))),
                                    data("aaaaaaaaaa", num(0)),
                                    data("bbbbbbbbbb", num(1)),
                                    data("cccccccccc", num(2)),
                                    data("DDDDDDDDDD", num(3)),
                                    data("eeeeeeeeee", num(4)),
                                    mov(mem(lea("cccccccccc")), num(7)),
                                    mov(mem(lea("DDDDDDDDDD")), num(9)),
                                    add(mem(lea("aaaaaaaaaa")), mem(lea("bbbbbbbbbb"))),
                                    sub(mem(lea("aaaaaaaaaa")), mem(lea("eeeeeeeeee"))),
                                    add(mem(lea("aaaaaaaaaa")), mem(lea("DDDDDDDDDD"))),
                                    sub(mem(lea("aaaaaaaaaa")), mem(lea("cccccccccc"))),
                                    add(mem(lea("aaaaaaaaaa")), mem(lea("DDDDDDDDDD"))),
                                    sub(mem(lea("aaaaaaaaaa")), mem(lea("cccccccccc"))),
                                    add(mem(lea("aaaaaaaaaa")), mem(lea("DDDDDDDDDD"))),
                                    sub(mem(lea("aaaaaaaaaa")), mem(lea("cccccccccc"))),
                                    add(mem(lea("bbbbbbbbbb")), mem(lea("cccccccccc"))),
                                    sub(mem(lea("bbbbbbbbbb")), mem(lea("eeeeeeeeee")))
                                });
    Computer computer6(5);
    computer6.boot(ooasm_bigger);
    assert(memory_dump(computer6) == "7 6 7 9 1 ");

    auto ooasm_flags = program({
                                   data("1", num(0)),
                                   data("2", num(-1)),
                                   data("3", num(-2)),
                                   data("4", num(-3)),
                                   data("5", num(-4)),
                                   data("6", num(-5)),
                                   data("7", num(-6)),
                                   onez(mem(lea("1"))),
                                   ones(mem(lea("2")))
                               });
    Computer computer7(7);
    computer7.boot(ooasm_flags);
    assert(memory_dump(computer7) == "0 -1 -2 -3 -4 -5 -6 ");

    auto ooasm_mov = program({
                                 mov(mem(num(0)), num(1980)),
                                 mov(mem(num(1)), num(1981)),
                                 mov(mem(num(2)), num(1991)),
                                 mov(mem(num(3)), num(1992)),
                                 mov(mem(num(4)), num(1993)),
                                 mov(mem(num(5)), num(1994)),
                                 mov(mem(num(6)), num(1995)),
                                 mov(mem(num(7)), num(1996)),
                                 mov(mem(num(8)), num(1997)),
                                 mov(mem(num(9)), num(1998)),
                                 mov(mem(num(10)), num(2014)),
                                 mov(mem(num(11)), num(2017)),
                                 mov(mem(num(12)), num(2018)),
                                 mov(mem(num(13)), num(2019)),
                                 mov(mem(num(14)), num(2020)),
                                 mov(mem(num(15)), num(2021))
                             });
    Computer computer8(16);
    computer8.boot(ooasm_mov);
    assert(memory_dump(computer8) == "1980 1981 1991 1992 1993 1994 1995 1996 1997 1998 2014 2017 2018 2019 2020 2021 ");
}