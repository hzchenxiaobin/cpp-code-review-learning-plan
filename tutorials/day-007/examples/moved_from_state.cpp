// moved_from_state.cpp —— moved-from 状态：合法但未知，且能“复活”
// 编译：clang++ -std=c++17 -Wall -Wextra -Wpedantic moved_from_state.cpp -o moved_from_state
// 运行：./moved_from_state
#include <cstdio>
#include <mutex>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

struct Buf {
    std::string tag;
    int* data_ = nullptr;
    size_t n_ = 0;

    explicit Buf(std::string t, size_t count = 8)
        : tag(std::move(t)), data_(new int[count]{}), n_(count) {}
    ~Buf() { delete[] data_; }

    Buf(const Buf& o) : tag(o.tag + "-copy"), data_(new int[o.n_]{}), n_(o.n_) {}
    Buf(Buf&& o) noexcept : tag(std::move(o.tag)), data_(o.data_), n_(o.n_) {
        o.data_ = nullptr;                        // 我们实现的选择：移空
        o.n_ = 0;
    }
    Buf& operator=(const Buf&) = delete;
    Buf& operator=(Buf&&) = delete;

    // 无前置条件的操作：moved-from 状态下依然可用
    size_t size() const { return n_; }
    bool empty() const { return n_ == 0; }

    void revive(std::string t, size_t count) {     // 赋新值：标准认可的复活方式
        delete[] data_;
        data_ = new int[count]{};
        n_ = count;
        tag = std::move(t);
    }
};

int main() {
    std::printf("[1] 观察 moved-from（我们的实现选择“置空”）：\n");
    Buf a("a", 8);
    Buf b(std::move(a));
    std::printf("    移动后：a.empty() = %d, a.size() = %zu\n", a.empty(), a.size());
    std::printf("            b.size() = %zu（资源已归 b）\n", b.size());

    std::printf("[2] moved-from 对象的合法操作（标准保证）：\n");
    std::printf("    a.empty() = %d（无前置条件 → 合法）\n", a.empty());
    a.revive("a-reborn", 4);
    std::printf("    复活后：a.size() = %zu（赋新值 = 认可的复活）\n", a.size());

    std::printf("[3] 复活对象照样进容器：\n");
    {
        std::vector<Buf> v;
        v.push_back(Buf("tmp", 2));
        v.push_back(std::move(a));                 // 二次移动同一个对象
        std::printf("    容器内 %zu 个 Buf；a.empty() = %d\n", v.size(), a.empty());
    }

    std::printf("[4] std::move 只是类型转换（不交给任何人就什么都没发生）：\n");
    Buf w("w", 8);
    std::move(w);                                  // 转换结果没人接收 → 无任何效果
    std::printf("    裸调用 std::move(w) 后：w.size() = %zu（完好）\n", w.size());
    auto&& r = std::move(w);                       // 绑定引用也不移动
    (void)r;
    std::printf("    绑定 auto&& 后：w.size() = %zu（仍完好）\n", w.size());

    std::printf("[5] 编译期检查：有些类型天生不许移动：\n");
    static_assert(!std::is_move_constructible_v<std::lock_guard<std::mutex>>,
                  "lock_guard 不可移动：移动 = 把锁带出临界区");
    static_assert(std::is_move_constructible_v<std::unique_lock<std::mutex>>,
                  "unique_lock 可移动：它就是为此设计的（可移交/可提前 unlock）");
    std::printf("    lock_guard 可移动？%d ｜ unique_lock 可移动？%d\n",
                (int)std::is_move_constructible_v<std::lock_guard<std::mutex>>,
                (int)std::is_move_constructible_v<std::unique_lock<std::mutex>>);
    return 0;
}
