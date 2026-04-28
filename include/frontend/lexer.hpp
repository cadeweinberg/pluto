// SPDX-Identifier: GPL-3.0-or-later

#ifndef PLUTO_FRONTEND_LEXER_HPP
#define PLUTO_FRONTEND_LEXER_HPP

#include <string_view>

#include "frontend/token.hpp"
#include "frontend/source_location.hpp"

namespace pluto {
class Lexer {
public:
    Lexer();
    Lexer(std::string_view source);

    void set(std::string_view source);
    void reset();

    Token next();
    std::string_view current() const;
    SourceLocation location() const;

private:
    void step();
    void up();

    SourceLocation m_location;
    // #NOTE: unique names due to dependence with re2c
    unsigned char const *YYTOKEN;
    unsigned char const *YYCURSOR;
    unsigned char const *YYMARKER;
    unsigned char const *YYLIMIT;
};
} // namespace pluto

#endif // !PLUTO_FRONTEND_LEXER_HPP
