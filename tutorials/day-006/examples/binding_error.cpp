// binding_error.cpp —— 预期编译失败：引用的绑定规则
// 这个文件故意写错：让编译器亲口告诉你“谁不能绑谁”。
// 编译（预期失败，真实报错见 Day 6 教程“演练 3”）：
//   clang++ -std=c++17 -Wall -Wextra -Wpedantic binding_error.cpp -o binding_error
int main() {
    int x = 10;
    int&& a = x;    // ← 错误 1：右值引用不能绑定左值（x 有名字、可取地址）
    int& b = 42;    // ← 错误 2：非 const 左值引用不能绑定右值（42 是纯右值）
    (void)a;
    (void)b;
    return 0;
}
