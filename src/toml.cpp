
#include <iostream>

#include "stringutils.h"
#include "variantreader.h"
#include "toml.h"

using namespace sutils::stringutils;

namespace sutils::toml {

struct VariantReaderComponents {

    VariantReaderComponents() : bool_reader({"true"}, {"false"}, true),
        string_reader({{"\\\\", "\\"}, {"\\n", "\n"}}) {

        reader.setReader(VariantReader::ReadMode::BOOL, &bool_reader);
        reader.setReader(VariantReader::ReadMode::NUMBER, &number_reader);
        reader.setReader(VariantReader::ReadMode::LIST, &list_reader);
        reader.setReader(VariantReader::ReadMode::STRING, &string_reader);

        reader.setBracketsReadSequence({ VariantReader::ReadMode::LIST });
        reader.setDefaultReadSequence({ VariantReader::ReadMode::BOOL,
                                        VariantReader::ReadMode::NUMBER });
        reader.setQuotesReadSequence({ VariantReader::ReadMode::STRING });
    }

    VariantReader reader;
    VariantReader::BoolReader bool_reader;
    VariantReader::NumberReader number_reader;
    VariantReader::ListReader list_reader;
    VariantReader::StringReader string_reader;
};

const char *ReadingError::what() const noexcept {

    return _error_message.c_str();
}

static void advanceTables(const std::vector<std::string>& to, Variant *&current,
                          std::size_t line) {

    for(auto&& s : to) {

        if(current->valid() && !current->isMap() && !current->isList()) {

            throw ReadingError("Cannot create a table named '" + s + "'", line);
        }

        if(current->isList()) {

            auto& cur_list = current->get<Variant::List>();

            if(cur_list.empty()) throw ReadingError("Internal error", line);

            current = &cur_list.back();
        }

        if(!current->valid()) *current = VariantMap();

        auto& map = current->get<VariantMap>();

        current = &map[s];
    }
}

static Variant *findTable(std::map<std::string, Variant>& out,
                          std::string table, std::size_t current_line) {

    std::vector<std::string> to_sp;
    std::vector<std::pair<std::string, std::string> > not_inside = {
        {"\"", "\""}, {"'", "'"}
    };
    if(!split(to_sp, table, ".", not_inside)) {

        throw ReadingError("Unterminated quotes", current_line);
    }
    if(to_sp.empty()) throw ReadingError("Error", current_line);

    Variant *current = &out[to_sp[0]];
    to_sp.erase(to_sp.begin());

    advanceTables(to_sp, current, current_line);

    if(!current->valid()) *current = VariantMap();
    else if(!current->isMap()) {

        throw ReadingError("Cannot create a table named '" + to_sp.back() + "'",
                           current_line);
    }

    return current;
}

static void assignValue(std::map<std::string, Variant>& out,
                        std::string to, Variant *table_place,
                        const Variant& value, std::size_t current_line) {

    std::vector<std::string> to_sp;
    std::vector<std::pair<std::string, std::string> > not_inside = {
        {"\"", "\""}, {"'", "'"}
    };
    if(!split(to_sp, to, ".", not_inside)) {

        throw ReadingError("Error", current_line);
    }

    Variant *current = table_place;
    if(current == nullptr) {

        current = &out[to_sp[0]];
        to_sp.erase(to_sp.begin());
    }

    advanceTables(to_sp, current, current_line);

    if(current->valid()) throw ReadingError("Error", current_line);

    *current = value;
}

static void parseAssignLine(std::map<std::string, Variant>& out,
                            const std::string& s, const VariantReader& reader,
                            Variant *current_table,
                            std::size_t current_line) {

    std::vector<std::string> to_sp;
    std::vector<std::pair<std::string, std::string> > not_inside = {
        {"\"", "\""}, {"'", "'"}
    };
    if(!split(to_sp, s, "=", not_inside, 1)) {

        throw ReadingError("Error", current_line);
    }
    if(to_sp.size() != 2) throw ReadingError("Error", current_line);

    Variant val = reader.read(to_sp[1]);

    if(!val.valid()) throw ReadingError("Error", current_line);

    stripInPlace(to_sp[0]);

    assignValue(out, to_sp[0], current_table, val, current_line);
}

static void appendTableList(Variant *&current_table, Variant& dest_table,
                            std::size_t current_line) {

    if(dest_table.valid()) {

        if(!dest_table.isList()) throw ReadingError("Error", current_line);
    }
    else {

        dest_table = VariantList();
    }

    auto& v_list = dest_table.get<VariantList>();
    v_list.push_back(VariantMap());

    current_table = &v_list.back();
}

static void parseLine(std::map<std::string, Variant>& out,
                      const std::string& s, const VariantReader& reader,
                      Variant *&current_table,
                      std::size_t current_line) {

    std::string stripped_str = strip(s);

    stripped_str.erase(std::find(stripped_str.begin(), stripped_str.end(), '#'),
                       stripped_str.end());

    if(stripped_str.empty()) return;

    if(stripped_str.size() >= 2) {

        if(stripped_str[0] == '[' && stripped_str[1] == '[') {

            if(stripped_str[stripped_str.size() - 1] != ']' ||
                stripped_str[stripped_str.size() - 2] != ']') {

                throw ReadingError("Expected ']]' at the end of the line",
                                   current_line);
            }

            stripped_str.pop_back();
            stripped_str.pop_back();
            stripped_str.erase(stripped_str.begin(), stripped_str.begin() + 2);

            std::vector<std::string> to_sp;
            std::vector<std::pair<std::string, std::string> > not_inside = {
                {"\"", "\""}, {"'", "'"}
            };
            if(!rSplit(to_sp, stripped_str, ".", not_inside, 1)) {
                throw ReadingError("Error", current_line);
            }
            if(to_sp.empty()) throw ReadingError("Error", current_line);

            if(to_sp.size() == 1) {

                Variant& v = out[stripped_str];

                appendTableList(current_table, v, current_line);
            }
            else {

                Variant *table = findTable(out, to_sp[1], current_line);

                if(!table->isMap()) throw ReadingError("Error", current_line);

                auto& v = table->get<Variant::Map>()[to_sp[0]];

                appendTableList(current_table, v, current_line);
            }

            return;
        }
    }

    if(stripped_str.front() == '[') {

        if(stripped_str.back() != ']') {

            throw ReadingError("Expected ']' at the end of the line",
                               current_line);
        }

        stripped_str.pop_back();
        stripped_str.erase(stripped_str.begin());
        current_table = findTable(out, stripped_str, current_line);
    }
    else parseAssignLine(out, s, reader, current_table, current_line);
}

static void countBrackets(const std::string& s, std::size_t *b_start,
                          std::size_t *b_end) {

    for(char c: s) {

        if(c == '[') (*b_start)++;
        else if(c == ']') (*b_end)++;
    }
}

std::map<std::string, Variant> read(std::istream& is) {

    VariantReaderComponents c;

    std::map<std::string, Variant> out;

    std::string str;
    std::string aux_str;
    std::string aux2_str;

    std::size_t current_line = 1;

    Variant *table = nullptr;

    std::size_t b_start = 0;
    std::size_t b_end = 0;

    while(is) {

        b_start = b_end = 0;

        std::getline(is, str);
        splitSingle(str, aux_str, '#');

        std::size_t sum_value = 0;

        countBrackets(str, &b_start, &b_end);
        while(b_start != b_end && is) {

            std::getline(is, aux_str);
            splitSingle(aux_str, aux2_str, '#');

            str.push_back(' ');

            countBrackets(aux_str, &b_start, &b_end);

            str += aux_str;
            sum_value++;
        }

        parseLine(out, str, c.reader, table, current_line++);

        current_line += sum_value;
    }

    return out;
}

} /* namespace sutils::toml */
