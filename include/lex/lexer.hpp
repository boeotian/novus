#pragma once
#include "lex/token.hpp"
#include "lex/token_itr.hpp"
#include <deque>
#include <vector>

namespace lex {

namespace internal {

class LexerImpl {
protected:
  LexerImpl() : m_inputPos{-1}, m_readBuffer{} {}

  auto next() -> Token;

private:
  int m_inputPos;
  std::deque<char> m_readBuffer;

  auto nextLitNumber(char mostSignficantChar) -> Token;
  auto nextLitNumberHex() -> Token;
  auto nextLitNumberBinary() -> Token;

  auto nextLitStr() -> Token;
  auto nextLitChar() -> Token;
  auto nextWordToken(char startingChar) -> Token;
  auto nextLineComment() -> Token;

  auto consumeChar() -> char;
  auto peekChar(size_t ahead) -> char&;

  virtual auto getFromInput() -> char = 0;
};

} // namespace internal

// Lexer object.
// Takes a character iterator and provides a token iterator.
template <typename InputItrBegin, typename InputItrEnd>
class Lexer final : private internal::LexerImpl {

  static_assert(
      std::is_same<typename std::iterator_traits<InputItrBegin>::value_type, char>::value,
      "Valuetype of input iterator has to be 'char'");

public:
  Lexer() = delete;
  Lexer(InputItrBegin inputBegin, const InputItrEnd inputEnd) :
      m_input{inputBegin}, m_inputEnd{inputEnd} {}

  [[nodiscard]] auto next() -> Token { return LexerImpl::next(); }

  [[nodiscard]] auto begin() -> TokenItr<Lexer> { return TokenItr{*this}; }

  [[nodiscard]] auto end() -> TokenItr<Lexer> { return TokenItr<Lexer>{}; }

private:
  InputItrBegin m_input;
  const InputItrEnd m_inputEnd;

  auto getFromInput() -> char override {
    if (m_input == m_inputEnd) {
      return '\0';
    }
    auto val = *m_input;
    ++m_input;
    return std::move(val);
  }
};

// Lex a list of tokens from a pair of character iterators.
template <typename InputItrBegin, typename InputItrEnd>
auto lexAll(InputItrBegin inputBegin, const InputItrEnd inputEnd) {
  auto lexer = Lexer{inputBegin, inputEnd};
  return std::vector<Token>{lexer.begin(), lexer.end()};
}

} // namespace lex
