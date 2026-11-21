// self_assignment.cpp —— 移动赋值的自赋值防护（正确版）
// 编译：clang++ -std=c++17 -Wall -Wextra -Wpedantic self_assignment.cpp -o self_assignment
// 运行：./self_assignment
#include <cstdio>
#include <string>
#include <utility>

struct Buf {
    std::string tag;
    int* data_ = nullptr;
    size_t n_ = 0;

    explicit Buf(std::string t, size_t count = 4)
        : tag(std::move(t)), data_(new int[count]{}), n_(count) {}
    ~Buf() { delete[] data_; }
    Buf(const Buf&) = delete;
    Buf(Buf&&) = delete;

    // 正确的移动赋值：①自赋值检查 ②先清自己 ③偷对方 ④置空对方
    Buf& operator=(Buf&& o) noexcept {
        if (this != &o) {                          // ← 关键防线
            std::printf("    [赋值] %s 接管 %s 的资源\n", tag.c_str(), o.tag.c_str());
            delete[] data_;
            data_ = o.data_;
            n_ = o.n_;
            tag = std::move(o.tag);
            o.data_ = nullptr;
            o.n_ = 0;
        } else {
            std::printf("    [赋值] 检测到自赋值，跳过（%s 安然无恙）\n", tag.c_str());
        }
        return *this;
    }

    size_t size() const { return n_; }
    const void* raw() const { return data_; }
};

int main() {
    std::printf("[1] 正常移动赋值：\n");
    Buf x("x", 4), y("y", 8);
    x = std::move(y);
    std::printf("    x.size() = %zu, y.size() = %zu\n", x.size(), y.size());

    std::printf("[2] 自赋值：b = std::move(b)：\n");
    Buf b("b", 4);
    const void* before = b.raw();
    b = std::move(b);                              // 有防线：无害
    std::printf("    自赋值后：b.size() = %zu, data_ 与之前相同？%d\n",
                b.size(), b.raw() == before);
    return 0;
}
