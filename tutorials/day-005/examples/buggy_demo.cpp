// buggy_demo.cpp —— 找 bug 演练：7 个“本周主题”埋点
// 规则：先限时 20 分钟只读代码找 bug 并记录，再看教程答案！
// 编译：clang++ -std=c++17 -Wall -Wextra -Wpedantic buggy_demo.cpp -o buggy_demo
// 运行：./buggy_demo
// 背景：一个“会话中心 + 资产快照”小系统。它大体能跑，但藏着 7 个本周学过的问题。
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

static int alive = 0;                    // 存活对象计数（收尾应为 0）

struct FileCloser {
    void operator()(FILE* f) const { if (f != nullptr) std::fclose(f); }
};

struct Hub;                              // 前置声明

struct Session : std::enable_shared_from_this<Session> {
    std::string user;
    std::shared_ptr<Hub> hub;            // 会话需要“知道”自己属于哪个中心
    explicit Session(std::string u) : user(std::move(u)) {
        ++alive;
        std::printf("  [+] Session %s\n", user.c_str());
    }
    ~Session() { --alive; std::printf("  [-] Session %s\n", user.c_str()); }
};

struct Hub {
    std::vector<std::shared_ptr<Session>> sessions;
    Hub() { ++alive; std::printf("  [+] Hub\n"); }
    ~Hub() { --alive; std::printf("  [-] Hub\n"); }
};

struct Asset {                           // 多态资产基类
    virtual void describe() const { std::printf("    asset（基类默认描述）\n"); }
    ~Asset() { std::printf("  [-] Asset（基类析构）\n"); }   // 非 virtual
};

struct Futures : Asset {                 // 派生资产：持有额外资源
    std::string margin;                  // 保证金账户（模拟重资源）
    Futures() : margin("5.2 亿") { ++alive; std::printf("  [+] Futures\n"); }
    ~Futures() { --alive; std::printf("  [-] Futures（释放 %s）\n", margin.c_str()); }
    void describe() const override { std::printf("    futures 保证金 %s\n", margin.c_str()); }
};

FILE* g_escaped = nullptr;               // 全局仓库（供逃逸的裸指针）

void sink(std::string s) {               // 按值接收：本意是“把数据搬进来”
    std::printf("    sink 收到 %zu 字节\n", s.size());
}

int main() {
    std::printf("[A] 写快照文件：\n");
    {
        FILE* f = std::fopen("snapshot.txt", "w");
        if (f == nullptr) return 1;
        std::fputs("snapshot v1\n", f);
        if (std::getenv("SNAPSHOT_FAIL") != nullptr) {
            std::printf("    快照写入失败，提前退出\n");
            return 1;                    // 异常/失败路径直接返回
        }
        std::fclose(f);
        std::printf("    快照写入完成\n");
    }

    std::printf("[B] 会话中心：\n");
    {
        auto hub = std::make_shared<Hub>();
        {
            auto s = std::make_shared<Session>("alice");
            s->hub = hub;                // 会话登记自己所属的中心
            hub->sessions.push_back(s);  // 中心登记会话
        }
        std::printf("    内层作用域结束，Session 析构了吗？（看上方日志）\n");
    }
    std::printf("    Hub 作用域结束，析构了吗？（看上方日志）\n");

    std::printf("[C] 资产删除：\n");
    {
        Asset* a = new Futures();
        a->describe();
        delete a;                        // 用基类指针删除派生对象
        std::printf("    delete 完成（Futures 的 [-] 日志出现了吗？）\n");
    }

    std::printf("[D] 快照数据处理：\n");
    {
        const std::string big(1 << 20, 'x');      // 1MB 数据
        sink(std::move(big));                     // 想把大块数据搬进 sink
        std::printf("    调用后 big.size() = %zu\n", big.size());

        std::string data = "原始数据";
        sink(std::move(data));
        std::printf("    之后还要用：data = \"%s\"（长度 %zu）\n", data.c_str(), data.size());
    }

    std::printf("[E] 逃逸分支（默认不执行，供 review）：\n");
    if (std::getenv("ESCAPE") != nullptr) {
        std::unique_ptr<FILE, FileCloser> g(std::fopen("esc.txt", "w"));
        g_escaped = g.get();             // 把受保护的句柄存到全局备用
    }

    std::printf("[F] weak_ptr 使用：\n");
    {
        auto s = std::make_shared<Session>("bob");
        std::weak_ptr<Session> w = s;
        auto locked = w.lock();
        std::printf("    locked->user = %s\n", locked->user.c_str());
    }

    std::printf("[G] 收尾：alive = %d（正确应为 0）\n", alive);
    return 0;
}
