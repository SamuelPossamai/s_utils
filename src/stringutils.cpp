
#include "stringutils.h"

namespace sutils::stringutils {

void splitSingle(std::string& f, std::string& s, char c, std::string::const_iterator begin, std::string::const_iterator end) {

    auto it = std::find(begin, end, c);

    if(it == end) return;

    s.assign(it + 1, end);

    if(f.cbegin() == begin) {
        f.resize(std::string::size_type(std::distance(begin, it)));
    }
    else f.assign(begin, it);
}

void splitSingle(std::string& f, std::string& s, const std::string& sub) {

    auto it = std::search(f.begin(), f.end(), sub.begin(), sub.end());

    if(it == f.end()) return;

    s.assign(it + std::string::difference_type(sub.size()), f.end());
    f.resize(std::string::size_type(std::distance(f.begin(), it)));
}

void rSplitSingle(std::string& f, std::string& s, char c, std::string::const_iterator begin, std::string::const_iterator end) {

    auto it = std::find(std::string::const_reverse_iterator(begin), std::string::const_reverse_iterator(end), c);

    if(it.base() == begin) return;

    s.assign(it.base() - 1, end);

    if(f.cbegin() == begin) {
        f.resize(std::string::size_type(std::distance(begin, it.base())) - 2);
    }
    else f.assign(begin, it.base() - 2);
}

void replace(std::string& str, const std::string& o, const std::string& n) {

    auto it = str.rbegin();
    while(true) {

        it = std::search(it, str.rend(), o.rbegin(), o.rend());

        if(it >= str.rend()) break;

        it += std::string::difference_type(o.size());

        str.replace(it.base(), it.base() + std::string::difference_type(o.size()), n.begin(), n.end());
    }
}

} /* namespace sutils::stringutils */
