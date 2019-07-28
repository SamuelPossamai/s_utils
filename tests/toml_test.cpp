
#include <iostream>
#include <fstream>

#include "variantdatainfo.h"
#include "toml.h"

using namespace sutils::variantdatainfo;
using namespace sutils;

int main() {

    std::fstream file("tests/test2.toml");

    auto toml_content = toml::read(file);

    std::cout << toml_content << std::endl;

}
