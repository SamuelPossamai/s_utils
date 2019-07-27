
#include "variantdatainfo.h"

namespace sutils {

namespace variantdatainfo {

std::pair<VariantDataInfo::iterator, bool> defaultsTo(VariantDataInfo& info, const std::string& str, const Variant& v) {

    auto it = info.find(str);

    if(it == info.end()) {

        auto p = info.insert({ str, v });

        if(!p.second) return { it, false };

        return { p.first, true };
    }
    return { it, false };
}

bool numberToReal(VariantDataInfo& info, const std::string& str) {

    auto it = info.find(str);

    if(it == info.end()) return false;

    return numberToReal(it);
}

bool numberToReal(const VariantDataInfo::iterator& it) {

    if(it->second.isNumber()) {

        it->second.numberToReal();
        return true;
    }
    return false;
}

} /* namespace variantdatainfo */

std::ostream& operator<<(std::ostream& os, const std::map<std::string, Variant>& vdi) {

    os << '{';

    if(!vdi.empty()) {

        os << '\n';

        for(auto&& [key, value]: vdi) {

            std::cout << "\t\"" << key << "\": " << value << '\n';
        }
    }

    os << '}';

    return os;
}

} /* namespace sutils */
