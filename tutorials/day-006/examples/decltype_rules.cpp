// decltype_rules.cpp —— decltype 与 decltype((x))：一对括号之差
// 编译：clang++ -std=c++17 -Wall -Wextra -Wpedantic decltype_rules.cpp -o decltype_rules
// 运行：./decltype_rules
#include <cstdio>
#include <type_traits>
#include <utility>

static int g = 1;   // 全局变量（by_parens 返回其引用是安全的）

// decltype(auto)：按 decltype 规则推导返回类型
decltype(auto) by_value()  { return g; }    // return 名字   → int（拷贝）
decltype(auto) by_parens() { return (g); }  // return (名字) → int&（引用！）

int main() {
    // [1] 编译期就成立的断言（运行前已验证）
    static_assert(std::is_same_v<decltype(g), int>,          "名字 → 声明类型");
    static_assert(std::is_same_v<decltype((g)), int&>,       "双括号名字 → int&（lvalue）");
    static_assert(std::is_same_v<decltype(std::move(g)), int&&>, "move 结果 → int&&（xvalue）");
    static_assert(std::is_same_v<decltype(42), int>,         "字面量 → int（prvalue）");

    std::printf("[1] 一对括号之差：\n");
    std::printf("  decltype(g)       是 int   ? %d\n", (int)std::is_same_v<decltype(g), int>);
    std::printf("  decltype((g))     是 int&  ? %d\n", (int)std::is_same_v<decltype((g)), int&>);
    std::printf("  decltype(move(g)) 是 int&& ? %d\n", (int)std::is_same_v<decltype(std::move(g)), int&&>);

    std::printf("[2] decltype(auto) 的返回类型陷阱：\n");
    int a = by_value();
    std::printf("  by_value()  → int（安全拷贝，值 %d）\n", a);
    int& b = by_parens();
    std::printf("  by_parens() → int&（引用全局 g，值 %d）\n", b);

    std::printf("[3] 同样写法换成局部变量就是悬垂（仅说明，不运行 UB）：\n");
    std::printf("  int local = 42; decltype(auto) r = (local); // r 是 int&\n");
    std::printf("  函数返回后 local 销毁 → 悬垂。review 看到 return (x) 要亮红灯！\n");
    return 0;
}
