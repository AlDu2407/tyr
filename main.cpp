#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <execution>
#include <fstream>
#include <ios>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

enum TokenType {
  func,
  ident,
  open_paren,
  close_paren,
  open_curl,
  close_curl,
  ret,
  lit_int,
  semi
};

typedef struct Token {
  TokenType type;
  std::optional<std::string> value;
} Token;

typedef struct NodeStmt {

} NodeStmt;

typedef struct NodeMain {
  std::vector<NodeStmt> statements;

} NodeMain;

typedef struct NodeProg {
  NodeMain main;
} NodeProg;

class Tokenizer {
public:
  inline Tokenizer(const std::string source, const int length)
      : m_source(source), m_length(length) {}

  inline std::vector<Token> tokenize() {
    std::vector<Token> tokens;
    Token prev;
    std::string buffer;
    while (auto next_val = next()) {
      if (next_val.value() == "fun") {
        tokens.push_back({.type = TokenType::func});
      } else if (next_val.value() == "ret") {
        tokens.push_back({.type = TokenType::ret});
      } else if (next_val.value() == "(") {
        tokens.push_back({.type = TokenType::open_paren});
      } else if (next_val.value() == ")") {
        tokens.push_back({.type = TokenType::close_paren});
      } else if (next_val.value() == "{") {
        tokens.push_back({.type = TokenType::open_curl});
      } else if (next_val.value() == "}") {
        tokens.push_back({.type = TokenType::close_curl});
      } else if (next_val.value() == ";") {
        tokens.push_back({.type = TokenType::semi});
      } else if (std::all_of(std::execution::seq, next_val.value().begin(),
                             next_val.value().end(), ::isdigit)) {
        tokens.push_back(
            {.type = TokenType::lit_int, .value = next_val.value()});
      } else if (std::all_of(std::execution::seq, next_val.value().begin(),
                             next_val.value().end(), ::isalnum)) {
        tokens.push_back({.type = TokenType::ident, .value = next_val.value()});
        if (next_val.value() == "main" && prev.type == TokenType::func) {
          m_has_main_func = true;
        }
      }
    }

    reset();
    return tokens;
  }

  inline bool has_main_func() const { return m_has_main_func; }

private:
  inline std::optional<std::string> next() {
    if (m_pos >= m_length)
      return {};
    std::string result;
    while (m_pos < m_length) {
      auto curr = m_source.at(m_pos);
      if (std::isalnum(curr)) {
        result.push_back(curr);
        m_pos++;
      } else if (std::isspace(curr) && !result.empty()) {
        m_pos++;
        return result;
      } else if (constraints.contains(curr)) {
        if (result.empty()) {
          m_pos++;
          return std::string{curr};
        } else {
          return result;
        }
      } else {
        m_pos++;
      }
    }

    return {};
  }

  inline void reset() { m_pos = 0; }

  const std::string m_source;
  const int m_length;
  const std::unordered_set<char> constraints = {'(', ')', '{', '}', ';'};
  bool m_has_main_func = false;
  int m_pos = 0;
};

class Parser {
public:
  Parser(const std::vector<Token> &tokens) : m_tokens(tokens) {}

  inline NodeProg parse() {}

private:
  [[nodiscard]] std::optional<Token> next() const {
    if (m_pos >= m_tokens.size())
      return {};

    return m_tokens.at(m_pos);
  }

  [[nodiscard]] std::optional<Token> consume() {
    if (m_pos >= m_tokens.size())
      return {};

    return m_tokens.at(m_pos++);
  }

  std::vector<Token> m_tokens;
  size_t m_pos = 0;
};

void print(Token &token) {
  switch (token.type) {
  case TokenType::func:
    std::cout << "`func`\n";
    break;
  case TokenType::open_paren:
    std::cout << "`(`\n";
    break;
  case TokenType::close_paren:
    std::cout << "`)`\n";
    break;
  case TokenType::open_curl:
    std::cout << "`{`\n";
    break;
  case TokenType::close_curl:
    std::cout << "`}`\n";
    break;
  case TokenType::semi:
    std::cout << "`;`\n";
    break;
  case TokenType::ret:
    std::cout << "`ret`\n";
    break;
  case TokenType::ident:
    std::cout << "`ident`"
              << ": value = `"
              << (token.value.has_value() ? token.value.value() : "") << "`\n";
    break;
  case TokenType::lit_int:
    std::cout << "`lit_int`"
              << ": value = `"
              << (token.value.has_value() ? token.value.value() : "") << "`\n";
    break;
  default:
    std::cerr << "Exhaustive exception, failed to implement print for `"
              << token.type << "`\n";
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Required argument missing...\n";
    std::cout << "Usage:\n";
    std::cout << "    ./main <path_to_file.ty> \n";

    return EXIT_FAILURE;
  }

  std::string file_path = argv[1];
  std::string source;
  int length;

  {
    std::cout << file_path << '\n';
    std::ifstream file{file_path, std::ios::in};
    file.seekg(0, file.end);
    length = file.tellg();
    file.seekg(0, file.beg);
    char *buffer = new char[length];
    file.read(buffer, length);
    source = buffer;

    delete[] buffer;
  }
  Tokenizer lexer{source, length};
  auto tokens = lexer.tokenize();

  if (!lexer.has_main_func()) {
    std::cerr << "Required `main`-function is missing.\n";
    std::cerr << "Please add `fun main() {...}`\n";
    return EXIT_FAILURE;
  }

  for (auto &token : tokens) {
    print(token);
  }

  return EXIT_SUCCESS;
}
