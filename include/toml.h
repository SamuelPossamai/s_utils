
#ifndef S_UTILS_TOML_H
#define S_UTILS_TOML_H

#include <exception>
#include <istream>

#include "variant.h"

namespace sutils::toml {

    class ReadingError : public std::exception {

    public:

        ReadingError(const std::string& error_message) : _error_message("toml::ReadingError" + error_message) {}

        virtual ~ReadingError() = default;

        virtual const char *what() const noexcept;

    private:

        std::string _error_message;
    };

    std::map<std::string, Variant> read(std::istream& is);

} /* namespace sutils::toml */

#endif // S_UTILS_TOML_H
