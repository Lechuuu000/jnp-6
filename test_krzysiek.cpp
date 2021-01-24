#include "ooasm.h"
#include "computer.h"
#include <string>
#include <sstream>
#include <cassert>
#include <iostream>

namespace
{
std::string memory_dump(Computer const& computer)
{
    std::stringstream ss;
    computer.memory_dump(ss);
    return ss.str();
}
}

int main()
{
    std::string mem_dump;

    //sprawdza początkowe wartości flag i funkcję one
    Computer computer1(3);
    auto ooasm_one = program
        ({
             data("one", num(2)),
             data("sf0", num(2)),
             data("zf0", num(2)),
             one(mem(lea("one"))),
             ones(mem(lea("sf0"))),
             onez(mem(lea("zf0")))
         });

    computer1.boot(ooasm_one);
    mem_dump = memory_dump(computer1);
    std::cout << mem_dump << std::endl;
    assert(mem_dump == "1 2 2 ");

    //sprawdza dodawanie i wartości flag po nim
    Computer computer2(7);
    auto ooasm_add = program
        ({
             data("var", num(0)),

             data("sf1", num(2)),
             data("zf1", num(2)),
             add(mem(lea("var")), num(42)), /* var == 42 */
             ones(mem(lea("sf1"))), /* sf1 == 2 */
             onez(mem(lea("zf1"))), /* zf2 == 2 */

             data("sf2", num(2)),
             data("zf2", num(2)),
             add(mem(lea("var")), num(-42)), /* var == 0 */
             ones(mem(lea("sf2"))), /* sf2 == 2 */
             onez(mem(lea("zf2"))), /* zf2 == 1 */

             data("sf3", num(2)),
             data("zf3", num(2)),
             add(mem(lea("var")), num(-404)), /* var == -404 */
             ones(mem(lea("sf3"))), /* sf2 == 1 */
             onez(mem(lea("zf3"))) /* zf3 == 2 */
         });

    computer2.boot(ooasm_add);
    mem_dump = memory_dump(computer2);
    std::cout << mem_dump << std::endl;
    assert(mem_dump == "-404 2 2 2 1 1 2 ");

    //sprawdza odejmowanie i wartości flag po nim
    Computer computer3(7);
    auto ooasm_sub = program
        ({
             data("var", num(0)),

             data("sf1", num(2)),
             data("zf1", num(2)),
             sub(mem(lea("var")), num(42)), /* var == -42 */
             ones(mem(lea("sf1"))), /* sf1 == 1 */
             onez(mem(lea("zf1"))), /* zf2 == 2 */

             data("sf2", num(2)),
             data("zf2", num(2)),
             sub(mem(lea("var")), num(-42)), /* var == 0 */
             ones(mem(lea("sf2"))), /* sf2 == 2 */
             onez(mem(lea("zf2"))), /* zf2 == 1 */

             data("sf3", num(2)),
             data("zf3", num(2)),
             sub(mem(lea("var")), num(-404)), /* var == 404 */
             ones(mem(lea("sf3"))), /* sf2 == 2 */
             onez(mem(lea("zf3"))) /* zf3 == 2 */
         });

    computer3.boot(ooasm_sub);
    mem_dump = memory_dump(computer3);
    std::cout << mem_dump << std::endl;
    assert(mem_dump == "404 1 2 2 1 2 2 ");

    //sprawdza inkrementację i wartości flag po niej
    Computer computer4(7);
    auto ooasm_inc = program
        ({
             data("var", num(-2)),

             data("sf1", num(2)),
             data("zf1", num(2)),
             inc(mem(lea("var"))), /* var == -1 */
             ones(mem(lea("sf1"))), /* sf1 == 1 */
             onez(mem(lea("zf1"))), /* zf2 == 2 */

             data("sf2", num(2)),
             data("zf2", num(2)),
             inc(mem(lea("var"))), /* var == 0 */
             ones(mem(lea("sf2"))), /* sf2 == 2 */
             onez(mem(lea("zf2"))), /* zf2 == 1 */

             data("sf3", num(2)),
             data("zf3", num(2)),
             inc(mem(lea("var"))), /* var == 1 */
             ones(mem(lea("sf3"))), /* sf2 == 2 */
             onez(mem(lea("zf3"))) /* zf3 == 2 */
         });

    computer4.boot(ooasm_inc);
    mem_dump = memory_dump(computer4);
    std::cout << mem_dump << std::endl;
    assert(mem_dump == "1 1 2 2 1 2 2 ");

    //sprawdza dekrementację i wartości flag po niej
    Computer computer5(7);
    auto ooasm_dec = program
        ({
             data("var", num(2)),

             data("sf1", num(2)),
             data("zf1", num(2)),
             dec(mem(lea("var"))), /* var == 1 */
             ones(mem(lea("sf1"))), /* sf1 == 2 */
             onez(mem(lea("zf1"))), /* zf2 == 2 */

             data("sf2", num(2)),
             data("zf2", num(2)),
             dec(mem(lea("var"))), /* var == 0 */
             ones(mem(lea("sf2"))), /* sf2 == 2 */
             onez(mem(lea("zf2"))), /* zf2 == 1 */

             data("sf3", num(2)),
             data("zf3", num(2)),
             dec(mem(lea("var"))), /* var == -1 */
             ones(mem(lea("sf3"))), /* sf2 == 1 */
             onez(mem(lea("zf3"))) /* zf3 == 2 */
         });

    computer5.boot(ooasm_dec);
    mem_dump = memory_dump(computer5);
    std::cout << mem_dump << std::endl;
    assert(mem_dump == "-1 2 2 2 1 1 2 ");

    std::cout << std::endl;
    std::cout << "test ok" << std::endl;
}
