
#ifndef S_UTILS_VARIANTDATAINFO_H
#define S_UTILS_VARIANTDATAINFO_H

#include <iostream>

#include "variant.h"

namespace sutils {

namespace variantdatainfo {

namespace __internal__ {

template<typename... Args>
struct isType;

template<>
struct isType <> {

    static bool result(const VariantDataInfo::const_iterator&) { return false; }
};

template<typename T, typename... Args>
struct isType <T, Args...> {

    static bool result(const VariantDataInfo::const_iterator& it) {

        if(it->second.isType<T>()) return true;

        return isType<Args...>::result(it);
    }
};

} /* namespace __internal__ */

std::pair<VariantDataInfo::iterator, bool> defaultsTo(VariantDataInfo& info, const std::string& str, const Variant& v);

bool numberToReal(VariantDataInfo& info, const std::string& str);

bool numberToReal(const VariantDataInfo::iterator& it);

template<typename... Args>
bool isType(const VariantDataInfo& info, const std::string& str) {

    auto it = info.find(str);

    if(it == info.end()) return false;

    return isType<Args...>(it);
}

template<typename... Args>
bool isType(const VariantDataInfo::const_iterator& it) {

    return __internal__::isType<Args...>::result(it);
}

template<typename... Args>
bool isType(const VariantDataInfo::iterator& it) {

    return isType<Args...>(static_cast<VariantDataInfo::const_iterator>(it));
}

} /* namespace variantdatainfo */

std::ostream& operator<<(std::ostream& os, const std::map<std::string, Variant>& vdi);

} /* namespace sutils */

#endif // S_UTILS_VARIANTDATAINFO_H
