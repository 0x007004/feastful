#include <cstring> // 或<string.h>，用于strlen
#include <cstdio> // 或<stdio.h>，用于printf

extern "C" {
    #include <tree_sitter/api.h>
}

extern "C" TSLanguage* tree_sitter_cpp();

int main() {
    // 创建一个新的解析器
    TSParser* parser = ts_parser_new();

    // 设置解析器使用的语言
    ts_parser_set_language(parser, tree_sitter_cpp());

    // 待解析的C++代码
    const char* source_code = "int main() { return 0; }";

    // 执行解析操作
    TSTree* tree = ts_parser_parse_string(
        parser,
        nullptr, // 没有旧树，从头开始解析
        source_code,
        strlen(source_code)
    );

    // 获取根节点
    TSNode root_node = ts_tree_root_node(tree);

    // 打印根节点的类型（应该是"translation_unit"）
    printf("Root node type: %s\n", ts_node_type(root_node));

    // 清理
    ts_tree_delete(tree);
    ts_parser_delete(parser);

    return 0;
}

