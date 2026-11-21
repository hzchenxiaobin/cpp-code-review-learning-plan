// warning_demo.cpp —— 亲眼看看“编译器是第一位 reviewer”
// 这段代码能编译、能运行，但藏着两个会被 -Wall -Wextra 揪出来的问题。
// 编译（不加 -Werror，让告警显形）：
//   clang++ -std=c++17 -Wall -Wextra -Wpedantic warning_demo.cpp -o warning_demo
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v{1, 2, 3};

    int unused = 42;          // 问题 1：定义了却从未使用

    int n = 0;                // 问题 2：拿有符号 int 去和无符号 size() 比较
    if (n < v.size()) {       // n 是 int，v.size() 是 size_t（无符号）
        std::cout << "v 里有 " << v.size() << " 个元素\n";
    }
    return 0;
}
