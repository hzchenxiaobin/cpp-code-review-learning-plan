// copy_error.cpp —— 预期编译失败的教学示例：unique_ptr 不可拷贝
// 这个文件故意写错：拷贝独占所有权会在编译期被直接拒绝。
// 编译（预期失败，真实报错见 Day 3 教程“演练 3”）：
//   clang++ -std=c++17 -Wall -Wextra -Wpedantic copy_error.cpp -o copy_error
#include <memory>

int main() {
    std::unique_ptr<int> a = std::make_unique<int>(42);
    std::unique_ptr<int> b = a;      // ← 拷贝 = 出现第二个“唯一”主人，编译期拒绝
    (void)b;
    return 0;
}
