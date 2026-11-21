// guard_variants.cpp —— RAII 不止管文件：任何“获取-释放”对都能绑定到对象
// 编译：clang++ -std=c++17 -Wall -Wextra -Wpedantic guard_variants.cpp -o guard_variants
// 运行：./guard_variants
#include <cstdio>
#include <stdexcept>

struct Tracer {                        // 模拟任意资源：构造=获取，析构=释放
    static int alive;                  // 当前仍持有的资源数
    explicit Tracer(const char* tag) : tag_(tag) {
        ++alive;
        std::printf("    获取 %s（存活 %d）\n", tag_, alive);
    }
    ~Tracer() {
        --alive;
        std::printf("    释放 %s（存活 %d）\n", tag_, alive);
    }
    const char* tag_;
};
int Tracer::alive = 0;

void nested(bool bomb) {
    Tracer a("内存块A");               // 函数作用域
    {
        Tracer b("锁L");               // 内层作用域
        if (bomb) throw std::runtime_error("boom");
    }                                  // 正常时 b 在这里释放
    Tracer c("socketC");               // 异常时根本走不到这行
}

int main() {
    std::printf("[1] 正常路径：\n");
    try { nested(false); } catch (...) {}
    std::printf("  结束时 alive = %d（应为 0）\n", Tracer::alive);

    std::printf("[2] 异常路径（在 b 作用域内抛出）：\n");
    try { nested(true); } catch (const std::exception& e) {
        std::printf("  caught: %s\n", e.what());
    }
    std::printf("  结束时 alive = %d（应为 0）\n", Tracer::alive);
    return 0;
}
