// hello_review.cpp —— Day 1 基线程序：一行告警都没有的“干净”代码
// 编译（-Werror 让任何告警都变成错误，用来验证环境）：
//   clang++ -std=c++17 -Wall -Wextra -Wpedantic -Werror hello_review.cpp -o hello_review
// 运行：./hello_review
#include <iostream>
#include <string>

int main() {
    const std::string greeting = "Hello, C++ Code Review!";
    std::cout << greeting << '\n';
    return 0;
}
