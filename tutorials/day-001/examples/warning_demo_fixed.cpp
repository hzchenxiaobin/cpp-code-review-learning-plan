// warning_demo_fixed.cpp —— warning_demo.cpp 的修复版：0 告警
// 编译（-Werror 证明它完全干净）：
//   clang++ -std=c++17 -Wall -Wextra -Wpedantic -Werror warning_demo_fixed.cpp -o warning_demo_fixed
// 运行：./warning_demo_fixed
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v{1, 2, 3};

    // 修复 1：删掉用不上的变量 unused
    // 修复 2：不做有符号/无符号比较，直接和 0 比，或用 size_t 类型
    if (v.size() > 0) {
        std::cout << "v 里有 " << v.size() << " 个元素\n";
    }

    // 更地道：容器判空用 empty()，遍历用范围 for
    if (!v.empty()) {
        for (int x : v) {
            std::cout << x << ' ';
        }
        std::cout << '\n';
    }
    return 0;
}
