extern "C" {
    #include "tree_sitter/api.h"
}
#include <string>

namespace feastful {
class Utils {
 public:
  std::string Node2Txt(TSNode node, const char* source_code);

  std::string readFileIntoString(const std::string& path);
};
}
