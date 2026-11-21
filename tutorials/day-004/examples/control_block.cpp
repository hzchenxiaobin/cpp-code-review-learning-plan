// control_block.cpp —— 引用计数：shared_ptr 的心脏
// 编译：clang++ -std=c++17 -Wall -Wextra -Wpedantic control_block.cpp -o control_block
// 运行：./control_block
#include <cstdio>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct Doc {
    std::string name;
    explicit Doc(std::string n) : name(std::move(n)) {
        std::printf("  [构造] Doc %s\n", name.c_str());
    }
    ~Doc() { std::printf("  [析构] Doc %s\n", name.c_str()); }
};

void observe(const char* tag, const std::shared_ptr<Doc>& p) {
    std::printf("  %-12s use_count = %ld\n", tag, p.use_count());
}

int main() {
    std::printf("[1] 计数随 拷贝/移动/作用域 变化：\n");
    {
        auto a = std::make_shared<Doc>("报告");
        observe("创建后", a);              // 1
        {
            auto b = a;                    // 拷贝：计数 +1
            observe("拷贝后", a);          // 2
            auto c = std::move(a);         // 移动：计数不变，只换主人
            observe("移动后", c);          // 2
        }   // b、c 离开作用域：计数 -1 -1 → 0 → Doc 析构
        std::printf("  内层作用域结束，Doc 已析构（见上方日志）\n");
    }

    std::printf("[2] 容器与局部变量共享同一份资源：\n");
    {
        std::vector<std::shared_ptr<Doc>> docs;
        auto d = std::make_shared<Doc>("合同");
        docs.push_back(d);
        docs.push_back(d);                 // 容器里两份 + 局部一份
        observe("入容器×2", d);            // 3
        docs.clear();
        observe("容器清空", d);            // 1
    }                                      // d 离开 → 0 → 析构

    std::printf("[3] 最后一个引用者负责释放：\n");
    {
        std::shared_ptr<Doc> last;
        {
            auto first = std::make_shared<Doc>("尾声");
            last = first;
        }   // first 离开作用域，但 last 仍持有 → Doc 还活着
        std::printf("  first 已离开，Doc 仍存活（last 持有）\n");
    }       // last 离开 → 0 → 此刻才析构
    std::printf("main 结束\n");
    return 0;
}
