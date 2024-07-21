#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
extern "C" {
    #include <tree_sitter/api.h>
}

extern "C" TSLanguage* tree_sitter_cpp();

std::string readFileIntoString(const std::string& path) {
    std::ifstream input(path);
    std::stringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

std::string node_text(TSNode node, const char* source_code) {
  auto length = ts_node_end_byte(node) - ts_node_start_byte(node);
  return std::string(source_code + ts_node_start_byte(node), length);
}

std::string Node2Txt(TSNode node, const std::string& sourceCode) {
  auto start = ts_node_start_byte(node);
  auto end = ts_node_end_byte(node);
  return sourceCode.substr(start, end - start);
}

void SplitExpress(TSNode node, const char* source_code) {
  // 获取当前节点的类型
  auto type = ts_node_type(node);
  // 检查当前节点是否是逻辑AND或逻辑OR
  if (std::string(type) == "binary_expression") {
    TSNode left_node = ts_node_child(node, 0);
    TSNode operator_node = ts_node_child(node, 1);
    TSNode right_node = ts_node_child(node, 2);

    std::string operator_text = node_text(operator_node, source_code);

    // 检查操作符是否是逻辑AND或逻辑OR
    if (operator_text == "&&" || operator_text == "||") {
      // 打印左侧的表达式
      std::string s = node_text(left_node, source_code);
      if (s.find("&&") == std::string::npos && s.find("||") == std::string::npos) {
        std::cout << s << std::endl;
      }
      // 打印右侧的表达式
      std::cout << node_text(right_node, source_code) << std::endl;
    }
  }

  // 递归遍历子节点
  uint32_t child_count = ts_node_child_count(node);
  for (uint32_t i = 0; i < child_count; ++i) {
    TSNode child = ts_node_child(node, i);
    SplitExpress(child, source_code);
  }
}

void ProcessExpression(TSNode node, const char* source_code) {
  std::string l = node_text(node, source_code);
  if (l.find("&&") == std::string::npos &&
      l.find("||") == std::string::npos) {
    std::cout << l << std::endl;
  }

  SplitExpress(node, source_code);
}

void processIfStatement(TSNode node, const std::string& sourceCode) {
  for (uint32_t i = 0; i < ts_node_child_count(node); ++i) {
    TSNode child = ts_node_child(node, i);

    auto start = ts_node_start_byte(child);
    auto end = ts_node_end_byte(child);
    std::string ifText = sourceCode.substr(start, end - start);

    std::string childType = ts_node_type(child);
    //std::cout <<childType <<std::endl;
    if (childType == "if_statement") {
      processIfStatement(child, sourceCode);
    }
    if (childType == "condition_clause") {
      ProcessExpression(child, sourceCode.c_str());
    }

    if (childType == "compound_statement") {
      processIfStatement(child, sourceCode);
    }

    if (childType == "else_clause") {
      processIfStatement(child, sourceCode);
    }
  }
}

void findIfStatements(TSNode node, const std::string& sourceCode) {
    uint32_t childCount = ts_node_child_count(node);

    for (uint32_t i = 0; i < childCount; i++) {
        TSNode child = ts_node_child(node, i);
        std::string nodeType = ts_node_type(child);

        if (nodeType == "if_statement") {
          // auto start = ts_node_start_byte(child);
          // auto end = ts_node_end_byte(child);
          // std::string ifText = sourceCode.substr(start, end - start);
          // std::cout << "------" << ifText << std::endl;
          processIfStatement(child, sourceCode);
        } else {
          findIfStatements(child, sourceCode);
        }
    }
}

int main() {
    TSParser* parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_cpp());

    std::string sourceCode = readFileIntoString("/home/liuyang/request_parser.cc");
    TSTree* tree = ts_parser_parse_string(parser, nullptr, sourceCode.c_str(), sourceCode.size());

    TSNode root_node = ts_tree_root_node(tree);

    findIfStatements(root_node, sourceCode);

    ts_tree_delete(tree);
    ts_parser_delete(parser);

    return 0;
}
