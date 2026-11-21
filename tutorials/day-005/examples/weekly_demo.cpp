// weekly_demo.cpp —— 第 1 周收官 mini 项目：资产报表系统
// 融合 Day1 警告全开 / Day2 RAII / Day3 unique_ptr / Day4 shared+weak
// 编译：clang++ -std=c++17 -Wall -Wextra -Wpedantic weekly_demo.cpp -o weekly_demo
// 运行：./weekly_demo
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// ---- Day 3：unique_ptr + 自定义删除器管理 FILE* ----
struct FileCloser {
    void operator()(FILE* f) const {
        if (f != nullptr) {
            std::fclose(f);
            std::printf("    [file] fclose 已执行（异常路径也没漏）\n");
        }
    }
};
using FilePtr = std::unique_ptr<FILE, FileCloser>;

// ---- Day 3：连接池，独占所有权 ----
struct Conn {
    std::string id;
    explicit Conn(std::string i) : id(std::move(i)) {
        std::printf("    [pool] 建立连接 %s\n", id.c_str());
    }
    ~Conn() { std::printf("    [pool] 关闭连接 %s\n", id.c_str()); }
};

// ---- Day 4：会话（共享所有权）+ 观察者（弱引用） ----
struct Session : std::enable_shared_from_this<Session> {
    std::string user;
    explicit Session(std::string u) : user(std::move(u)) {
        std::printf("    [sess] 登录 %s\n", user.c_str());
    }
    ~Session() { std::printf("    [sess] 登出 %s\n", user.c_str()); }
};

struct AuthMonitor {                              // 观察者：不延长会话生命周期
    std::vector<std::weak_ptr<Session>> watching;
    void report() const {
        for (const auto& w : watching) {
            if (auto s = w.lock()) {
                std::printf("    [mon ] %s 在线\n", s->user.c_str());
            } else {
                std::printf("    [mon ] （会话已失效，weak_ptr 安全察觉）\n");
            }
        }
    }
};

int main() {
    std::printf("[1] 写报表（Day2+3：异常路径也落盘）：\n");
    try {
        FilePtr out(std::fopen("weekly_report.txt", "w"));
        if (!out) throw std::runtime_error("open failed");
        std::fputs("周报：第 1 周知识资产\n", out.get());
        throw std::runtime_error("模拟中途异常");
    } catch (const std::exception& e) {
        std::printf("    caught: %s\n", e.what());
    }
    if (FILE* in = std::fopen("weekly_report.txt", "r")) {
        std::printf("    重读报表: %s\n",
                    std::fgetc(in) != EOF ? "有内容（落盘成功）" : "（空）");
        std::fclose(in);
    }

    std::printf("[2] 连接池（Day3：独占所有权）：\n");
    {
        std::vector<std::unique_ptr<Conn>> pool;
        pool.push_back(std::make_unique<Conn>("行情源"));
        pool.push_back(std::make_unique<Conn>("风险引擎"));
        std::printf("    池内有 %zu 个连接\n", pool.size());
    }

    std::printf("[3] 会话监视（Day4：共享 + 弱引用）：\n");
    AuthMonitor mon;
    {
        auto s = std::make_shared<Session>("alice");
        mon.watching.push_back(s);
        mon.report();
    }
    mon.report();

    std::printf("[4] 全部作用域结束——每类资源的 [-] 日志都应配对出现：\n");
    std::printf("main 结束\n");
    return 0;
}
