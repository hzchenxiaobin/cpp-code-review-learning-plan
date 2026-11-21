// unique_basics.cpp —— unique_ptr：独占所有权的标准表达
// 编译：clang++ -std=c++17 -Wall -Wextra -Wpedantic unique_basics.cpp -o unique_basics
// 运行：./unique_basics
#include <cstdio>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct Conn {
    std::string name;
    explicit Conn(std::string n) : name(std::move(n)) {
        std::printf("  [获取] 连接 %s 建立\n", name.c_str());
    }
    ~Conn() { std::printf("  [释放] 连接 %s 关闭\n", name.c_str()); }
};

// 工厂函数返回 unique_ptr：所有权随返回值交给调用方（《EMC++》条款 18 的推荐姿势）
std::unique_ptr<Conn> make_conn(std::string name) {
    return std::make_unique<Conn>(std::move(name));   // 隐式移动构造返回值
}

// 按值接收 = 明确要求调用方交出所有权
void handoff(std::unique_ptr<Conn> c) {
    std::printf("  [交接] 现在由 handoff 持有 %s\n", c->name.c_str());
}   // 离开作用域，c 自动释放

class ConnPool {
public:
    void add(std::unique_ptr<Conn> c) {
        conns_.push_back(std::move(c));     // 容器要求元素可移动——unique_ptr 正合适
    }
    ~ConnPool() {
        std::printf("  [池析构] 池内还有 %zu 个连接，逐一释放\n", conns_.size());
    }
private:
    std::vector<std::unique_ptr<Conn>> conns_;   // 独占：池拥有全部连接
};

int main() {
    std::printf("[1] make_unique 创建 + std::move 转移：\n");
    auto a = make_conn("db");
    auto b = std::move(a);                   // 转移所有权
    std::printf("    移动后 a=%s, b->name=%s\n",
                a ? "非空" : "空（unique_ptr 移动后保证为空）", b->name.c_str());

    std::printf("[2] 按值传参交给 handoff（= 交出所有权）：\n");
    handoff(std::move(b));
    std::printf("    b=%s（所有权已不在我们手里）\n", b ? "非空" : "空");

    std::printf("[3] 装进资源池：\n");
    ConnPool pool;
    pool.add(make_conn("redis"));
    pool.add(make_conn("kafka"));

    std::printf("[4] main 结束，观察释放顺序：\n");
    return 0;
}
