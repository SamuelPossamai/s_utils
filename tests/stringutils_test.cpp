
#include <iostream>
#include <vector>
#include <cassert>
#include <csignal>

#include "stringutils.h"

using namespace sutils::stringutils;

void abortHandler(int signum) {

    std::cerr << "\033[0m" << std::endl;

    exit(signum);
}

int main() {

    std::cerr << "\033[1;31m";

    signal(SIGABRT, abortHandler);

    std::string aux = "  asdasd dsassadads   asd  ";
    stripInPlace(aux);
    assert(aux == "asdasd dsassadads   asd");

    assert(startsWith(aux, "asdasd ") == true);

    assert(startsWith(aux, "asdfffsd") == false);

    assert(endsWith(aux, "  asd") == true);

    assert(endsWith(aux, "ssasd") == false);

    aux = "127.0.0.1";

    std::string aux2;

    splitSingle(aux, aux2, '.');

    assert(aux == "127");
    assert(aux2 == "0.0.1");

    aux = "127.0.0.1";
    aux2.clear();

    rSplitSingle(aux, aux2, '.');

    assert(aux == "127.0.0");
    assert(aux2 == "1");

    aux = "#$asda#$sdasd#$asdsadd(as##sadasd#$da)sas$$sadda#$sasd#$";
    const char pat[] = "#$";

    std::vector<std::pair<std::string, std::string> > not_inside = {{"(", ")"}};

    const int dist_vec[] = {0, 6, 13, 48, 54};

    {
        FindIterator f_it(aux.cbegin(), aux.cend(), pat, pat + sizeof(pat)/sizeof(*pat) - 1, &not_inside);

        int i = 0;
        for(; f_it.base() != aux.cend(); ++f_it) {

            assert(std::distance(aux.cbegin(), f_it.base()) == dist_vec[i]);

            i++;
        }
    }

    std::string str_pat(pat, pat + sizeof(pat)/sizeof(*pat) - 1);

    {
        FindIterator f_it(aux.crbegin(), aux.crend(), str_pat.crbegin(), str_pat.crend(), &not_inside, true);

        int i = sizeof(dist_vec)/sizeof(*dist_vec) - 1;
        for(; f_it.base() != aux.crend(); ++f_it) {

            assert(std::distance(f_it.adjustedBase(), aux.crend()) == dist_vec[i]);

            i--;
        }
    }

    std::vector<std::string> split_out;

    split(split_out, aux, str_pat, not_inside);

    assert(split_out.size() == 6);

    assert(split_out[0] == "");
    assert(split_out[1] == "asda");
    assert(split_out[2] == "sdasd");
    assert(split_out[3] == "asdsadd(as##sadasd#$da)sas$$sadda");
    assert(split_out[4] == "sasd");
    assert(split_out[5] == "");

    split_out.clear();
    rSplit(split_out, aux, str_pat, not_inside);

    assert(split_out.size() == 6);

    assert(split_out[0] == "");
    assert(split_out[1] == "sasd");
    assert(split_out[2] == "asdsadd(as##sadasd#$da)sas$$sadda");
    assert(split_out[3] == "sdasd");
    assert(split_out[4] == "asda");
    assert(split_out[5] == "");

    std::cerr << "\033[1;34m" << std::endl;

    std::cout << "Passed all tests :)" << std::endl;

    std::cerr << "\033[0m";
}
