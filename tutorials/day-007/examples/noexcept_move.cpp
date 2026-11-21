// noexcept_move.cpp —— noexcept 的分量：vector 扩容时到底移动还是拷贝
// 编译：clang++ -std=c++17 -Wall -Wextra -Wpedantic noexcept_move.cpp -o noexcept_move
// 运行：./noexcept_move
#include <cstdio>
#include <vector>

struct SafeBuf {                       // 移动构造标了 noexcept
    inline static int moves = 0, copies = 0;
    SafeBuf() = default;
    SafeBuf(const SafeBuf&) noexcept { ++copies; }
    SafeBuf(SafeBuf&&) noexcept { ++moves; }        // ← 承诺不抛
    SafeBuf& operator=(const SafeBuf&) noexcept = default;
    SafeBuf& operator=(SafeBuf&&) noexcept = default;
};

struct RiskyBuf {                      // 移动构造没标 noexcept（其余完全相同）
    inline static int moves = 0, copies = 0;
    RiskyBuf() = default;
    RiskyBuf(const RiskyBuf&) noexcept { ++copies; }
    RiskyBuf(RiskyBuf&&) { ++moves; }               // ← 没承诺
    RiskyBuf& operator=(const RiskyBuf&) noexcept = default;
    RiskyBuf& operator=(RiskyBuf&&) noexcept = default;
};

int main() {
    std::printf("[1] SafeBuf（移动构造 noexcept）：\n");
    {
        std::vector<SafeBuf> v;
        v.reserve(2);
        v.emplace_back();
        v.emplace_back();
        std::printf("    扩容前：moves = %d, copies = %d\n", SafeBuf::moves, SafeBuf::copies);
        v.emplace_back();            // 第 3 个 → 触发扩容搬迁
        std::printf("    扩容后：moves = %d, copies = %d  ← 全部偷指针\n",
                    SafeBuf::moves, SafeBuf::copies);
    }

    std::printf("[2] RiskyBuf（移动构造未标 noexcept）：\n");
    {
        std::vector<RiskyBuf> v;
        v.reserve(2);
        v.emplace_back();
        v.emplace_back();
        std::printf("    扩容前：moves = %d, copies = %d\n", RiskyBuf::moves, RiskyBuf::copies);
        v.emplace_back();            // 同样扩容，但……
        std::printf("    扩容后：moves = %d, copies = %d  ← 退回深拷贝！\n",
                    RiskyBuf::moves, RiskyBuf::copies);
    }

    std::printf("[3] 为什么：vector 要给扩容留“强异常保证”：\n");
    std::printf("    搬到第 3 个元素时移动构造抛了怎么办？前 2 个已被掏空，回不去了\n");
    std::printf("    → 标准规定：移动不 noexcept 且可拷贝时，扩容退回拷贝（move_if_noexcept）\n");
    return 0;
}
