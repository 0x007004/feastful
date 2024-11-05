#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

extern "C" {
    #include <tree_sitter/api.h>
}
std::string code;

extern "C" TSLanguage* tree_sitter_cpp();

std::string readFileIntoString(const std::string& path) {
    std::ifstream input(path);
    std::stringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

std::string node_text(TSNode node, const char* source_code) {
//  auto start = ts_node_start_byte(child);
//  auto end = ts_node_end_byte(child);
//  std::string ifText = sourceCode.substr(start, end - start);

  auto length = ts_node_end_byte(node) - ts_node_start_byte(node);
  return std::string(source_code + ts_node_start_byte(node), length);
}



std::string Node2Txt(TSNode node, const std::string& sourceCode) {
  auto start = ts_node_start_byte(node);
  auto end = ts_node_end_byte(node);
  return sourceCode.substr(start, end - start);
}

const std::string NOT_OPERATOR_SYMBOL = "!";

void SplitExpress(TSNode node, const char* source_code) {
  // 获取当前节点的类型
  auto type = ts_node_type(node);

  // 检查当前节点是否是逻辑AND或逻辑OR
  if (std::string(type) == "binary_expression") {
    TSNode left_node = ts_node_child(node, 0);
    TSNode operator_node = ts_node_child(node, 1);
    TSNode right_node = ts_node_child(node, 2);

    std::string operator_text = node_text(operator_node, source_code);
    std::cout << "operator: "<< operator_text << std::endl;

    // 检查操作符是否是逻辑AND或逻辑OR
    if (operator_text == "&&" || operator_text == "||") {
      // 打印左侧的表达式
      std::string s = node_text(left_node, source_code);
      if (s.find("&&") == std::string::npos && s.find("||") == std::string::npos) {
         std::cout << "left: "<< s << std::endl;
      }

      if (s == "FLAGS_test_del_code") {
        auto length = ts_node_end_byte(left_node) - ts_node_start_byte(left_node);
        code.replace(ts_node_start_byte(left_node), length,"true");
      }

      // 打印右侧的表达式
      std::cout << "right: " <<node_text(right_node, source_code) << std::endl;
    }
  }

  // 递归遍历子节点
  // uint32_t child_count = ts_node_child_count(node);
  // std::cout << "express size : " <<child_count << node_text(node, source_code)<<std::endl; 
  // for (uint32_t i = 0; i < child_count; ++i) {
  //   TSNode child = ts_node_child(node, i);

  //   std::cout <<"    " << ts_node_type(child) << ": "<< node_text(child, source_code) <<std::endl;  

  //   SplitExpress(child, source_code);
  // }
}

void ProcessUnary(TSNode node, const char* source_code) {
  auto op = ts_node_child(node, 0);
  auto identifier = ts_node_child(node, 1);
  if (ts_node_type(op) == NOT_OPERATOR_SYMBOL) {
    std::cout << node_text(identifier, source_code) <<std::endl;
  }
}

void ProcessExpression(TSNode node, const char* source_code) {
  std::string l = node_text(node, source_code);
  std::cout << l <<std::endl;
  auto expression = ts_node_child(node, 1);

  std::string nodeType = ts_node_type(expression);
  std::cout << nodeType << " : "<< node_text(expression, source_code) <<std::endl;

  if (nodeType == "binary_expression") {
    // ProcessBinary(node, source_code);
  } else if (nodeType == "unary_expression") {
    ProcessUnary(expression, source_code);
  } else if (nodeType == "identifier") {
    // 只包含某个表达式
  }
}


void processIfStatement(TSNode node, const std::string& sourceCode) {
  int total_count = ts_node_child_count(node);
  (void)total_count;
  auto if_kw = ts_node_child(node, 0);
  (void)if_kw;
  auto if_kw_n = ts_node_start_byte(if_kw);
  (void)if_kw_n;
  std::cout << node_text(if_kw, sourceCode.c_str()) << std::endl;
  auto if_condition = ts_node_child(node, 1);
  (void)if_condition;
  ProcessExpression(if_condition, sourceCode.c_str());

  //auto if_consequence = ts_node_child(node,2);
  //auto if_consequence_n = ts_node_end_byte(if_consequence);

  //if (total_count >= 4) {
  //  auto else_clause = ts_node_child(node, 3);
  //  int s = ts_node_start_byte(else_clause);
  //  int end = s+4;
  //  if_consequence_n = end;
  //}

  //std::cout <<sourceCode <<std::endl;
  //auto sourceCode_copy = sourceCode;
  //sourceCode_copy.replace(if_kw_n, if_consequence_n-if_kw_n," ");
  //std::cout <<sourceCode_copy <<std::endl;

  //std::cout <<"======================" << std::endl;
}

//void processIfStatement2(TSNode node, const std::string& sourceCode) {
//  std::cout <<"in processIfStatement2 " <<std::endl;
//  for (uint32_t i = 0; i < ts_node_child_count(node); ++i) {
//    TSNode child = ts_node_child(node, i);
//    auto start = ts_node_start_byte(child);
//    auto end = ts_node_end_byte(child);
//    std::string ifText = sourceCode.substr(start, end - start);
//    std::cout <<"----" << ts_node_type(child) << ": "<< ifText <<std::endl;  
//
//    //const std::string& childType = ts_node_type(child);
//    //if (childType == "if_statement") {
//    //  std::cout << childType <<" : "<< node_text(child, sourceCode.c_str())<<std::endl;;
//    //}
//    //if (childType == "condition_clause") {
//    //  std::cout << childType <<" : "<< node_text(child, sourceCode.c_str())<<std::endl;;
//    //}
//
//    //if (childType == "compound_statement") {
//    //  std::cout << childType <<" : "<< node_text(child, sourceCode.c_str()) <<std::endl;
//    //}
//
//    //if (childType == "else_clause") {
//    //  std::cout <<"-----" << std::endl;
//    //  std::cout << childType <<" : "<< node_text(child, sourceCode.c_str()) <<std::endl;
//    //  std::cout <<"-----" << std::endl;
//    //  //processIfStatement(child, sourceCode);
//    //}
//  }
//}

void findIfStatements(TSNode node, const std::string& sourceCode) {
  uint32_t childCount = ts_node_child_count(node);

  for (uint32_t i = 0; i < childCount; i++) {
    TSNode child = ts_node_child(node, i);
    std::string nodeType = ts_node_type(child);
    if (nodeType == "if_statement") {
      processIfStatement(child, sourceCode);
    } else {
      findIfStatements(child, sourceCode);
    }
  }
}

int main() {
    TSParser* parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_cpp());
    std::string sourceCode = readFileIntoString("./source.cc");
    code = sourceCode;
    TSTree* tree = ts_parser_parse_string(parser, nullptr, sourceCode.c_str(), sourceCode.size());

    TSNode root_node = ts_tree_root_node(tree);

    findIfStatements(root_node, sourceCode);

    //std::cout <<code <<std::endl;b
    ts_tree_delete(tree);
    ts_parser_delete(parser);

    return 0;
}
