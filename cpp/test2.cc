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

// 新函数：提取并打印if条件
void printIfCondition(TSNode node, const std::string& sourceCode) {
    // 获取条件部分的节点
    TSNode conditionNode = ts_node_child(node, 1); // 一般情况下，条件节点是第二个子节点
    TSNode conditionNode2 = ts_node_child(node, 3); // 一般情况下，条件节点是第二个子节点

    // 获取条件文本
    {
      auto start = ts_node_start_byte(conditionNode);
      auto end = ts_node_end_byte(conditionNode);
      std::string conditionText = sourceCode.substr(start, end - start);
      std::cout << "node type: " << ts_node_type(conditionNode) <<std::endl;
      std::cout << "If condition: " << conditionText << std::endl;
    }
    {
      auto start = ts_node_start_byte(conditionNode2);
      auto end = ts_node_end_byte(conditionNode2);
      std::string conditionText = sourceCode.substr(start, end - start);
      std::cout << "node type: " << ts_node_type(conditionNode2) <<std::endl;
      std::cout << "If condition: " << conditionText << std::endl;
    }
}

void findIfStatements(TSNode node, const std::string& sourceCode) {
    uint32_t childCount = ts_node_child_count(node);

    for (uint32_t i = 0; i < childCount; i++) {
        TSNode child = ts_node_child(node, i);
        std::string nodeType = ts_node_type(child);

        if (nodeType == "if_statement") {
            // 打印整个if语句
            auto start = ts_node_start_byte(child);
            auto end = ts_node_end_byte(child);
            std::string ifText = sourceCode.substr(start, end - start);
            std::cout << "Found an if statement: " << ifText << std::endl;

            // 打印if条件
            printIfCondition(child, sourceCode);
        }
        
        std::cout <<"-----" <<std::endl;
        // 递归遍历子节点
        findIfStatements(child, sourceCode);
    }
}

int main() {
    TSParser* parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_cpp());

    std::string sourceCode = readFileIntoString("ex.cc");
    TSTree* tree = ts_parser_parse_string(parser, nullptr, sourceCode.c_str(), sourceCode.size());

    TSNode root_node = ts_tree_root_node(tree);

    findIfStatements(root_node, sourceCode);

    ts_tree_delete(tree);
    ts_parser_delete(parser);

    return 0;
}
