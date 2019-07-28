
#include <iostream>
#include <fstream>

#include "variantdatainfo.h"
#include "toml.h"

using namespace sutils::variantdatainfo;
using namespace sutils;

int main(int argc, char **argv) {

    if(argc != 2) {

        std::cout << "Wrong number of arguments" << std::endl;
        return 0;
    }

    std::fstream file("tests/test" + std::string(argv[1]) + ".toml");

    auto toml_content = toml::read(file);

    std::cout << toml_content << std::endl;

}
