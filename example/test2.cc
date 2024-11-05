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
    std::cout <<  "NODE LIST SIZE: " << ts_node_child_count(node) <<std::endl;
    for (unsigned int i = 0; i < ts_node_child_count(node) ; ++i) {
      TSNode conditionNode = ts_node_child(node, i); // 一般情况下，条件节点是第二个子节点
      // 获取条件文本
      {
        auto start = ts_node_start_byte(conditionNode);
        auto end = ts_node_end_byte(conditionNode);
        std::string conditionText = sourceCode.substr(start, end - start);
        std::cout << "-- node type: " << ts_node_type(conditionNode) <<std::endl;
        std::cout << "-- If condition: " << conditionText << std::endl;
        if (ts_node_type(conditionNode) == std::string("if_statement")) {
          printIfCondition(conditionNode, sourceCode);
        }
      }
    }

    return ;
    // 获取条件部分的节点
    TSNode conditionNode = ts_node_child(node, 1); // 一般情况下，条件节点是第二个子节点

    // 获取条件文本
    {
      auto start = ts_node_start_byte(conditionNode);
      auto end = ts_node_end_byte(conditionNode);
      std::string conditionText = sourceCode.substr(start, end - start);
      std::cout << "node type: " << ts_node_type(conditionNode) <<std::endl;
      std::cout << "If condition: " << conditionText << std::endl;
    }
}

void processIfStatement(TSNode node, const std::string& sourceCode) {
    // 假设我们已经确定node是一个if_statement
    printIfCondition(node, sourceCode);
    // 遍历if_statement的所有子节点
    for (uint32_t i = 0; i < ts_node_child_count(node); ++i) {
        TSNode child = ts_node_child(node, i);
        std::string childType = ts_node_type(child);
        std::cout <<"child type : " << childType << std::endl;
        // 假设我们找到了else关键字
        /*if (childType == "if_statement") {
            auto start = ts_node_start_byte(child);
            auto end = ts_node_end_byte(child);
            std::string ifText = sourceCode.substr(start, end - start);
            std::cout << "Found an if statement: " << ifText << std::endl;
        }*/
        
        if (childType == "else_clause") {
          std::cout << "Process Else" << std::endl;
            // 处理else部分
            // 注意：这里的处理可能需要根据实际情况调整
            //auto start = ts_node_start_byte(child);
            //auto end = ts_node_end_byte(child);
            printIfCondition(child, sourceCode);
 //           std::string elseText = sourceCode.substr(start, end - start);
 //           std::cout << "Else part: " << elseText << std::endl;
        }
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
