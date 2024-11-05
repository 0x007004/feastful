#include "src/utils.h"
extern "C" {
    #include "tree_sitter/api.h"
}

namespace feastful {
std::string Utils::Node2Txt(::TSNode node, const char* source_code) {
  auto length = ts_node_end_byte(node) - ts_node_start_byte(node);
  return std::string(source_code + ts_node_start_byte(node), length);
}

std::string Utils::readFileIntoString(const std::string& path) {
  std::ifstream input(path);
  std::stringstream buffer;
  buffer << input.rdbuf();
  return buffer.str();
}

}  // namespace feastful
