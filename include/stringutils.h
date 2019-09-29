
#ifndef S_UTILS_STRINGUTILS_H
#define S_UTILS_STRINGUTILS_H

#include <algorithm>
#include <string>
#include <limits>

namespace sutils::stringutils {

template<typename ITER1, typename ITER2, template<typename, typename...> class NotInsideContainer, typename String>
class FindIterator {

public:

    FindIterator(ITER1 begin, ITER1 end, ITER2 pat_begin, ITER2 pat_end,
                 const NotInsideContainer<std::pair<String, String> > *not_inside,
                 bool using_reverse_iterator = false);

    FindIterator(const FindIterator&) = default;

    auto operator *() const { return *_cur; }
    auto operator->() const { return _cur.operator->(); }

    FindIterator& operator++();

    FindIterator operator++(int) {

        auto before = *this;

        ++(*this);

        return before;
    }

    const ITER1& base() const { return _cur; }
    const ITER1 adjustedBase() const { return _using_reverse_iterator ? _cur + std::distance(_pat_begin, _pat_end) : _cur; }

    bool ok() const { return _s_inside.empty() || _cur != _end; }

private:

    ITER1 _cur;
    ITER1 _end;
    ITER2 _pat_begin;
    ITER2 _pat_end;
    String _s_inside;
    const NotInsideContainer<std::pair<String, String> >& _not_inside;
    bool _using_reverse_iterator;
};

inline bool notBlank(char c) {

    return !std::isspace(c);
}

template<typename ITER1, typename ITER2>
bool startsWith(ITER1 b1, ITER1 e1, ITER2 b2, ITER2 e2) {

    while(b1 != e1 && b2 != e2) {

        if(*b1 != *b2) return false;

        ++b1;
        ++b2;
    }

    return b2 == e2;
}

inline bool startsWith(const std::string& s1, const std::string& s2) {

    return startsWith(s1.begin(), s1.end(), s2.begin(), s2.end());
}

inline bool endsWith(const std::string& s1, const std::string& s2) {

    return startsWith(s1.rbegin(), s1.rend(), s2.rbegin(), s2.rend());
}

template<typename String, typename Callable1, typename Callable2>
std::pair<typename String::const_iterator, typename String::const_iterator> findInterval(const String& str,
                                                                                         Callable1 c1, Callable2 c2) {

    auto stripped_begin = std::find_if(str.begin(), str.end(), c1);
    auto stripped_end = std::find_if(str.rbegin(), str.rend(), c2).base();

    if(stripped_begin > stripped_end) stripped_end = stripped_begin;

    return { stripped_begin, stripped_end };
}

template<typename Callable>
void stripInPlace(std::string& str, Callable callable) {

    auto p = findInterval(str, callable, callable);
    str.assign(p.first, p.second);
}

inline void stripInPlace(std::string& str) {

    stripInPlace(str, notBlank);
}

template<typename Callable>
std::string strip(const std::string& str, Callable callable) {

    auto p = findInterval(str, callable, callable);
    return std::string(p.first, p.second);
}

inline std::string strip(const std::string& str) {

    return strip(str, notBlank);
}

void splitSingle(std::string& f, std::string& s, const std::string& sub);

void splitSingle(std::string& f, std::string& s, char c, std::string::const_iterator begin, std::string::const_iterator end);

inline void splitSingle(std::string& f, std::string& s, char c) {

    splitSingle(f, s, c, f.begin(), f.end());
}

void rSplitSingle(std::string& f, std::string& s, char c, std::string::const_iterator begin, std::string::const_iterator end);

inline void rSplitSingle(std::string& f, std::string& s, char c) {

    rSplitSingle(f, s, c, f.begin(), f.end());
}

inline std::pair<std::string, std::string> splitSingle(const std::string& str, const std::string& sub) {

    std::string f = str, s;
    splitSingle(f, s, sub);

    return {f, s};
}

inline std::pair<std::string, std::string> splitSingle(const std::string& str, char c) {

    std::string f = str, s;
    splitSingle(f, s, c);

    return {f, s};
}

template<template<typename, typename...> class Container, typename String>
void split(Container<String>& container, const String& str, const String& pat) {

    auto it = str.begin();
    while(it <= str.end()) {

        auto end_it = std::search(it, str.end(), pat.begin(), pat.end());

        container.push_back(String(it, end_it));

        it = end_it + pat.size();
    }
}

template<template<typename, typename...> class Container, typename String>
void split(Container<String>& container, const String& str, const char *pat) {

    split(container, str, String(pat));
}

template<template<typename, typename...> class OutputContainer,
         template<typename, typename...> class NotInsideContainer, typename String, typename Pair>
bool split(OutputContainer<String>& output, const String& str, const String& pat,
           const NotInsideContainer<Pair>& not_inside, std::size_t max = std::numeric_limits<std::size_t>::max()) {

    auto beg_it = str.begin();

    FindIterator f_it(str.begin(), str.end(), pat.begin(), pat.end(), &not_inside);
    for(std::size_t i = 0; f_it.base() != str.end() && i < max; ++f_it, ++i) {

        output.push_back(String(beg_it, f_it.base()));

        beg_it = f_it.base() + pat.size();
    }

    output.push_back(String(beg_it, str.end()));

    return f_it.ok();
}

template<template<typename, typename...> class OutputContainer,
         template<typename, typename...> class NotInsideContainer, typename String, typename Pair>
bool split(OutputContainer<String>& output, const String& str, const char *pat,
           const NotInsideContainer<Pair>& not_inside, std::size_t max = std::numeric_limits<std::size_t>::max()) {

    return split(output, str, String(pat), not_inside, max);
}

template<template<typename, typename...> class OutputContainer,
         template<typename, typename...> class NotInsideContainer, typename String, typename Pair>
bool rSplit(OutputContainer<String>& output, const String& str, const String& pat,
           const NotInsideContainer<Pair>& not_inside, std::size_t max = std::numeric_limits<std::size_t>::max()) {

    auto beg_it = str.rbegin();

    FindIterator f_it(str.rbegin(), str.rend(), pat.rbegin(), pat.rend(), &not_inside, true);
    for(std::size_t i = 0; f_it.base() != str.rend() && i < max; ++f_it, ++i) {

        output.push_back(String(f_it.base().base(), beg_it.base()));

        beg_it = f_it.base() + pat.size();
    }

    output.push_back(String(str.begin(), beg_it.base()));

    return f_it.ok();
}

template<template<typename, typename...> class OutputContainer,
         template<typename, typename...> class NotInsideContainer, typename String, typename Pair>
bool rSplit(OutputContainer<String>& output, const String& str, const char *pat,
           const NotInsideContainer<Pair>& not_inside, std::size_t max = std::numeric_limits<std::size_t>::max()) {

    return rSplit(output, str, String(pat), not_inside, max);
}

inline void toLower(std::string& s) {

    std::transform(s.begin(), s.end(), s.begin(), static_cast<int (*)(int)>(std::tolower));
}

inline void toUpper(std::string& s) {

    std::transform(s.begin(), s.end(), s.begin(), static_cast<int (*)(int)>(std::toupper));
}

void replace(std::string& str, const std::string& o, const std::string& n);

template<typename ITER1, typename ITER2, template<typename, typename...> class NotInsideContainer, typename String>
FindIterator<ITER1, ITER2, NotInsideContainer, String>::FindIterator(ITER1 begin, ITER1 end, ITER2 pat_begin, ITER2 pat_end,
                                                                     const NotInsideContainer<std::pair<String, String> > *not_inside,
                                                                     bool using_reverse_iterator) :
                                                                        _cur(begin), _end(end), _pat_begin(pat_begin), _pat_end(pat_end),
                                                                        _not_inside(*not_inside), _using_reverse_iterator(using_reverse_iterator) {

    if(!startsWith(_cur, _end, _pat_begin, _pat_end)) ++(*this);
}

template<typename ITER1, typename ITER2, template<typename, typename...> class NotInsideContainer, typename String>
FindIterator<ITER1, ITER2, NotInsideContainer, String>& FindIterator<ITER1, ITER2, NotInsideContainer, String>::operator++() {

    _cur += std::distance(_pat_begin, _pat_end);

    for(; _cur < _end; ++_cur) {

        if(_s_inside.empty()) {

            for(auto&& p : _not_inside) {

                if(_using_reverse_iterator) {

                    if(startsWith(_cur, _end, p.second.rbegin(), p.second.rend())) {

                        _s_inside.assign(p.first.rbegin(), p.first.rend());
                        break;
                    }
                }
                else {

                    if(startsWith(_cur, _end, p.first.begin(), p.first.end())) {

                        _s_inside = p.second;
                        break;
                    }
                }
            }

            if(!_s_inside.empty()) continue;

            if(startsWith(_cur, _end, _pat_begin, _pat_end)) {

                return *this;
            }
        }
        else if(startsWith(_cur, _end, _s_inside.begin(), _s_inside.end())) {

            _cur += _s_inside.size() - 1;
            _s_inside.clear();
        }
    }

    if(_cur > _end) _cur = _end;

    return *this;
}

} /* namespace sutils::stringutils */

#endif // S_UTILS_STRINGUTILS_H
