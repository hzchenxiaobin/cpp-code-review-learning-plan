// self_assignment_bad.cpp —— 反面教材：移动赋值不查自赋值会怎样
// 本文件故意不写防护，用于观察“自移动赋值”的真实后果。
// 编译：clang++ -std=c++17 -Wall -Wextra -Wpedantic self_assignment_bad.cpp -o self_assignment_bad
// 运行：./self_assignment_bad   （本演示不触发崩溃，但状态被静默破坏）
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

    // 反面教材：没有自赋值检查的移动赋值
    Buf& operator=(Buf&& o) noexcept {
        std::printf("    [赋值] 进入（this=%p, &o=%p，同一个？%d）\n",
                    (void*)this, (void*)&o, (void*)this == (void*)&o);
        delete[] data_;          // 若 o 就是 *this：家已经炸了
        data_ = o.data_;         // o.data_ 就是刚 delete 的那块（悬垂指针值）
        n_ = o.n_;
        o.data_ = nullptr;       // o 就是 *this → 把自己也置空了！
        o.n_ = 0;
        return *this;
    }

    size_t size() const { return n_; }
    const void* raw() const { return data_; }
};

int main() {
    Buf b("b", 4);
    std::printf("[前] b.size() = %zu, data_ = %p\n", b.size(), b.raw());
    b = std::move(b);            // 自移动赋值
    std::printf("[后] b.size() = %zu, data_ = %p\n", b.size(), b.raw());
    std::printf("     自移动赋值后：4 个元素不翼而飞（size=0，data_=空）\n");
    std::printf("     无崩溃、无运行期报错——静默数据丢失\n");
    std::printf("     （若成员顺序不同，还可能出现“size 撒谎”或双重释放等别的坏法）\n");
    return 0;
}
