
#include <iostream>

#include "metalist.h"

template <typename T>
struct Act {

    Act() { std::cout << sizeof(T) << ' '; }

};

int main() {

    using meta = sutils::MetaList<int, char, double>;

    std::cout << "size: " << meta::size() << std::endl;
    std::cout << sizeof(meta::get<0>::Result) << std::endl;
    std::cout << sizeof(meta::get<1>::Result) << std::endl;
    std::cout << sizeof(meta::get<2>::Result) << std::endl;

    meta::applyToAll<Act>();

    std::cout << std::endl;
}
