// move_semantics.cpp —— 亲手实现移动构造：看清楚“搬”到底搬了什么
// 编译：clang++ -std=c++17 -Wall -Wextra -Wpedantic move_semantics.cpp -o move_semantics
// 运行：./move_semantics
#include <cstdio>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

class Buffer {
public:
    explicit Buffer(const char* tag) : tag_(tag), data_(new int[8]{}) {
        std::printf("  [%s] 构造：data_ = %p\n", tag_, (void*)data_);
    }
    ~Buffer() {
        std::printf("  [%s] 析构：释放 data_ = %p\n", tag_, (void*)data_);
        delete[] data_;
    }
    // 拷贝构造：分配新内存 + 深复制
    Buffer(const Buffer& o) : tag_(o.tag_), data_(new int[8]{}) {
        std::printf("  [%s] 拷贝构造：新分配 %p（源 %p 不受影响）\n",
                    tag_, (void*)data_, (void*)o.data_);
    }
    // 移动构造：偷走指针 + 把源置空 —— 零分配
    Buffer(Buffer&& o) noexcept : tag_(o.tag_), data_(o.data_) {
        o.data_ = nullptr;
        std::printf("  [%s] 移动构造：接管 %p，源已置空\n", tag_, (void*)data_);
    }
    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) = delete;

    bool empty() const { return data_ == nullptr; }

private:
    const char* tag_;
    int* data_;
};

int main() {
    std::printf("[1] 手写移动构造，观察“偷指针”：\n");
    Buffer a("A");
    Buffer b(std::move(a));              // 真正干活的是移动构造函数
    std::printf("    a.empty() = %d（源被掏空）, b.empty() = %d\n",
                a.empty(), b.empty());

    std::printf("[2] std::move 遇上 const：静默退化成拷贝！\n");
    const std::string s = "我会原地不动";
    std::string t = std::move(s);        // const → 没有移动构造可用 → 退回拷贝
    std::printf("    移动后 s 仍是 \"%s\"（长度 %zu，根本没被搬走）\n",
                s.c_str(), s.size());
    (void)t;

    std::printf("[3] 编译期性质对照：\n");
    std::printf("    Buffer     可拷贝？%d ｜ 可移动？%d\n",
                std::is_copy_constructible_v<Buffer>,
                std::is_move_constructible_v<Buffer>);
    std::printf("    unique_ptr 可拷贝？%d ｜ 可移动？%d\n",
                (int)std::is_copy_constructible_v<std::unique_ptr<int>>,
                (int)std::is_move_constructible_v<std::unique_ptr<int>>);
    return 0;
}
