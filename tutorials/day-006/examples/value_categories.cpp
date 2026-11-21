// value_categories.cpp —— 用 decltype 探针打印表达式的值类别
// 编译：clang++ -std=c++17 -Wall -Wextra -Wpedantic value_categories.cpp -o value_categories
// 运行：./value_categories
#include <cstdio>
#include <string>
#include <utility>

// 值类别探针：decltype((expr)) 的结果
//   int → prvalue（纯右值）  int& → lvalue（左值）  int&& → xvalue（将亡值）
// 注意：decltype 不求值——探针只“看”表达式，不执行它
template <class T> struct Cat;
template <> struct Cat<int>   { static constexpr const char* name = "prvalue 纯右值"; };
template <> struct Cat<int&>  { static constexpr const char* name = "lvalue  左值"; };
template <> struct Cat<int&&> { static constexpr const char* name = "xvalue  将亡值"; };

#define CAT(expr) std::printf("  %-22s : %s\n", #expr, Cat<decltype((expr))>::name)

int get();                       // 按值返回 int

struct Buf {
    std::string tag;
    explicit Buf(std::string t) : tag(std::move(t)) {}
};

const char* pick(const Buf&) { return "const Buf&（拷贝阵营）"; }
const char* pick(Buf&&)      { return "Buf&&（移动阵营）"; }

int main() {
    std::printf("[1] 值类别探针（注意：decltype 不执行表达式，x 始终是 10）：\n");
    int x = 10;
    int arr[3] = {1, 2, 3};
    int* p = arr;
    CAT(x);             // 具名变量：左值
    CAT(42);            // 字面量：纯右值
    CAT(x + 1);         // 算术结果：纯右值
    CAT(std::move(x));  // move 的产物：将亡值
    CAT(arr[0]);        // 下标：左值
    CAT(*p);            // 解引用：左值
    CAT(x = 99);        // 赋值表达式：返回左值（但不会真的执行！）
    CAT(++x);           // 前置自增：返回左值
    CAT(x++);           // 后置自增：返回临时的旧值——纯右值（意外！）
    CAT(get());         // 按值返回的函数调用：纯右值

    std::printf("[2] 值类别决定重载决议（拷贝 vs 移动）：\n");
    Buf b("具名对象");
    std::printf("  pick(b)                 → %s\n", pick(b));
    std::printf("  pick(std::move(b))      → %s\n", pick(std::move(b)));
    std::printf("  pick(Buf(\"临时对象\"))    → %s\n", pick(Buf(std::string("临时对象"))));

    std::printf("[3] 绑定规则（谁可以绑谁）：\n");
    int& lr = x;                // 左值引用 ← 左值：天经地义
    int&& rr = 42;              // 右值引用 ← 纯右值：延长临时生命周期
    const int& clr = 42;        // const& 特权：可绑右值，且延长其生命周期
    std::printf("  int&  ← x   ：OK（值 %d）\n", lr);
    std::printf("  int&& ← 42  ：OK（值 %d；注意 rr 自己是左值！）\n", rr);
    std::printf("  const int&←42：OK（值 %d，临时活到了作用域结束）\n", clr);
    return 0;
}

int get() { return 7; }
