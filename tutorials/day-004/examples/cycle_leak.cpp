// cycle_leak.cpp —— 循环引用：让 RAII 集体失灵的经典事故
// 编译：clang++ -std=c++17 -Wall -Wextra -Wpedantic cycle_leak.cpp -o cycle_leak
// 运行：./cycle_leak
#include <cstdio>
#include <memory>
#include <string>
#include <utility>
#include <vector>

static int alive = 0;   // 存活对象计数：泄漏的铁证

struct Node {
    std::string name;
    explicit Node(std::string n) : name(std::move(n)) {
        ++alive;
        std::printf("  [构造] %s（存活 %d）\n", name.c_str(), alive);
    }
    virtual ~Node() {
        --alive;
        std::printf("  [析构] %s（存活 %d）\n", name.c_str(), alive);
    }
};

// ---------- 事故版：双向都是强引用 ----------
struct BadParent;

struct BadChild : Node {
    std::shared_ptr<BadParent> up;                  // ← 强引用“回头”：事故源头
    explicit BadChild(std::string n) : Node(std::move(n)) {}
};

struct BadParent : Node {
    std::vector<std::shared_ptr<BadChild>> kids;    // 强引用“向下”：本身正常
    explicit BadParent(std::string n) : Node(std::move(n)) {}
};

// ---------- 修复版：回头路改成弱引用 ----------
struct GoodParent;

struct GoodChild : Node {
    std::weak_ptr<GoodParent> up;                   // 弱引用：不参与计数
    explicit GoodChild(std::string n) : Node(std::move(n)) {}
};

struct GoodParent : Node {
    std::vector<std::shared_ptr<GoodChild>> kids;
    explicit GoodParent(std::string n) : Node(std::move(n)) {}
};

int main() {
    std::printf("[1] 事故现场：双向 shared_ptr 成环\n");
    {
        auto p = std::make_shared<BadParent>("坏爸爸");
        auto c = std::make_shared<BadChild>("坏孩子");
        p->kids.push_back(c);       // Parent → Child（强）
        c->up = p;                  // Child → Parent（强）← 成环！
        std::printf("  离开作用域前：use_count(parent) = %ld, use_count(child) = %ld\n",
                    p.use_count(), c.use_count());
    }   // p、c 离开，但环内互相抱住：各剩 1 个引用，永远到不了 0
    std::printf("  作用域已结束 —— 析构日志一行都没有！存活对象 = %d\n", alive);

    std::printf("[2] 修复：回头路改用 weak_ptr\n");
    {
        auto p = std::make_shared<GoodParent>("好爸爸");
        auto c = std::make_shared<GoodChild>("好孩子");
        p->kids.push_back(c);       // 向下仍用强引用：父拥有子
        c->up = p;                  // 向上改弱引用：子只是“知道”父
        std::printf("  离开作用域前：use_count(parent) = %ld（弱引用不计数）\n", p.use_count());
        if (auto locked = c->up.lock()) {   // 需要访问时临时升级为强引用
            std::printf("  child 借 lock() 访问 parent：%s\n", locked->name.c_str());
        }
    }
    std::printf("  作用域已结束 → 正常析构（见上方日志），存活对象 = %d\n", alive);
    return 0;
}
