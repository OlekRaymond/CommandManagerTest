#include <string_view>
#include <type_traits>
#include <string>
#include <vector>
#include <array>
#include <stdexcept>

// As an additional requirement I have chossen to:
//  accept unicode and ascii string_views
// Because:
//  - Requirements state full unicode but 
//      no known valid opcodes are non-ascii.
//  - All examples are unicode only
// So:
//  - assume they are might be downcast at some point earlier in the program 
//  - allow for both until we find out


namespace wstring_utils {
    // wstring annoances: 
    //  We have chosen to support both w and short strings
    //  A two strings have to be the same types to be added
    //  So doing anything with string literals is annoying
    template<typename StringView>
    constexpr bool are_same(StringView first, std::string_view literal) {
        // both are ascii
        if constexpr (std::is_same_v<decltype(first), decltype(literal)>) {
            return first == literal;
        }

        // first is a wide, make litteral a wide too
        size_t i = 0;
        for (auto character : first) {
            if (character >= 127) { return false; }
            if (character != literal[i]) { return false; }
            i++;
        }
        return true;
    }

    template<typename char_t, typename StringView>
    auto str(StringView literal) {
        using String = std::basic_string<char_t>;
        if constexpr (std::is_same_v<char_t, typename StringView::value_type>) {
            return String{literal};
        }
        String result{};
        for (const auto character : literal) {
            result += character;
        }
        return result;
    }
}

// As we don't have concepts, use functions to do type constraints
template<typename StringView>
constexpr void ValidateStringViewType() {
    // You could remove this if you don't care about performace
    static_assert(
        !(
            std::is_same_v<StringView, std::string>
        ||  std::is_same_v<StringView, std::wstring>
        )
        , "Expected string_view but got string, avoid coping string for performance"
    );

    // constrain to a string_view like object
    static_assert(
          std::is_same_v<StringView, std::string_view>
      ||  std::is_same_v<StringView, std::wstring_view>
        // would be nive to accept a std::span<char_t, (>10)> in C++20
        // never const char* because it's a pain to work with 
        //  and a raw pointer should enter or leave a function
    , "Expected string_view like but was not a valid input type");
}

// Assume no validation is done earlier in the program
template<typename StringView>
constexpr std::true_type ValidateString(StringView message) {
    ValidateStringViewType<StringView>();
    if (message.size() < 11) {
        throw std::runtime_error("size cannot be less than 11, 10 for opcode and 1 for #");
    }

    if (message.back() != '#') {
        throw std::runtime_error("message does not end in a #");
    }

    return std::true_type();
}

template<typename StringView>
std::vector<StringView> SplitIntoPairs(StringView from) {
    if (from.back() == ',') {
        from.remove_suffix(1);
    }
    std::vector<StringView> res{};
    constexpr size_t pair_start = 0;
    while(pair_start != StringView::npos && !from.empty()) {
        size_t pair_middle = from.find(',', pair_start);
        if (pair_middle == StringView::npos) { pair_middle = from.size(); }
        size_t pair_end = from.find(',', std::min(pair_middle + 1, from.size()-1));
        if (pair_end == StringView::npos) { pair_end = from.size(); }
        res.emplace_back(from.substr(pair_start, pair_end));
        from.remove_prefix(std::min(pair_end + 1, from.size()));
    }
    return res;
}

template<typename StringView>
std::basic_string<typename StringView::value_type> ParseKeyValueFields(StringView message_data) {
    // param_value WILL have no more than 6 decimal places
    //  the wording "will" was taken to mean 
    //  this is validated ahead of time
    using String = std::basic_string<typename StringView::value_type>;
    ValidateStringViewType<StringView>();
    String result{};
    if (message_data.size() < 3) {
        throw std::runtime_error(
            "message could not be parsed into key value pairs as it was not long enough"
        );
    }
    const auto pairs = SplitIntoPairs(message_data);
    // const auto pair = pairs.at(0); 
    for (const auto pair : pairs )
    {
        const auto comma_pos = pair.find(',');
        if (comma_pos == std::string_view::npos) {
            throw std::logic_error("Bug in SplitIntoPairs");
        }
        const auto param_name = pair.substr(0, comma_pos);
        const auto param_value = pair.substr(comma_pos + 1);
        if (param_name.size() > 15) {
            throw std::runtime_error(
                "Paramerter name was longer than the allowed 15 characters");
        }
        result += param_name;
        // do elemtent wise to avoid wstring issues
        result += ' '; result += '='; result += ' ';
        result += param_value;
        result += '\n';
    }
    result.resize(result.size() -1);
    return result;
}

[[nodiscard]] inline std::array<std::string, 5>& GetHistory() {
    static std::array<std::string, 5> history{};
    return history;
}

template<typename StringView>
void AddHistory(StringView opcode) {
    auto& history = GetHistory();
    history[4] = history[3];
    history[3] = history[2];
    history[2] = history[1];
    history[1] = history[0];
    history[0] = wstring_utils::str<char>(opcode);
}

// Note:
//  This is the only place the char/wchar stuff will ever produce a
//  wrong answer and that is only if an opcode entered is a wchar
//  current expected usage is always a short string
template<typename char_t>
std::basic_string<char_t> HistoryToString() {
    const auto& history = GetHistory();
    std::basic_string<char_t> result{};
    for (const std::string& item : history) {
        if (item.size() < 10) { break; }
        using wstring_utils::str;
        result += str<char_t>(item);
        result += '\n';
    }
    result.resize(result.size() -1);
    return result;
} 

template<typename char_t>
struct Message {
    static constexpr size_t op_code_length = 10;
    using StringView = std::basic_string_view<char_t>;
    using String = std::basic_string<char_t>;

    Message() = delete;
    constexpr Message(StringView message) {
        ValidateString(message);
        message.remove_suffix(1); // length of #
        opcode = message.substr(0, op_code_length);
        message_contents = message.substr(op_code_length);
    }

    void AddToHistory() const { AddHistory(opcode); }

    String ToString() const {
        using namespace std::string_view_literals;
        using namespace wstring_utils;
        #define CASE(other) if (are_same(opcode, other)) return 
            CASE("RUN_NO____") str<char_t>("Run number: "sv) + String{message_contents};
            CASE("POLAR_NO__") str<char_t>("Polar number: "sv) + String{message_contents};
            CASE("USR_MSG___") String{message_contents};
            CASE("D_USR_FLD_") ParseKeyValueFields(message_contents);
            CASE("HISTORY___") HistoryToString<char_t>();
            else return String{};
        #undef CASE 
    }

    StringView opcode;
    StringView message_contents;
};
// Deduction guides:
Message(std::basic_string_view<char>) -> Message<char>;
Message(std::basic_string_view<wchar_t>) -> Message<wchar_t>;
