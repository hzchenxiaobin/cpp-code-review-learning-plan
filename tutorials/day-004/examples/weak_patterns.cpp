// weak_patterns.cpp —— weak_ptr 的正确打开方式：观察者、lock、shared_from_this
// 编译：clang++ -std=c++17 -Wall -Wextra -Wpedantic weak_patterns.cpp -o weak_patterns
// 运行：./weak_patterns
#include <cstdio>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct Session : std::enable_shared_from_this<Session> {
    std::string user;
    explicit Session(std::string u) : user(std::move(u)) {
        std::printf("  [会话建立] %s\n", user.c_str());
    }
    ~Session() { std::printf("  [会话结束] %s\n", user.c_str()); }

    std::shared_ptr<Session> self() { return shared_from_this(); }
};

struct Monitor {                            // 观察者：只看，不延长被观察者的生命周期
    std::vector<std::weak_ptr<Session>> watched;
    void status() const {
        for (const auto& w : watched) {
            if (auto s = w.lock()) {
                std::printf("    监视中: %s（活着，use_count = %ld）\n",
                            s->user.c_str(), s.use_count());
            } else {
                std::printf("    监视中: （已失效——对象已销毁，weak_ptr 自动察觉）\n");
            }
        }
    }
};

int main() {
    Monitor mon;

    std::printf("[1] weak_ptr 不延长生命周期：\n");
    {
        auto s = std::make_shared<Session>("alice");
        mon.watched.push_back(s);           // 弱引用登记
        mon.status();                       // 活着
    }                                       // s 离开 → 会话立刻结束
    mon.status();                           // 已失效

    std::printf("[2] lock() = 临时升级强引用，用完即还：\n");
    {
        auto s = std::make_shared<Session>("bob");
        mon.watched.push_back(s);
        {
            auto strong = mon.watched[1].lock();
            std::printf("    lock 期间 use_count = %ld\n", strong.use_count());   // 2
        }   // strong 离开，强引用归还
        std::printf("    lock 结束后 use_count = %ld\n", s.use_count());          // 1
    }

    std::printf("[3] 经典错误：构造函数里调 shared_from_this → 抛异常\n");
    try {
        struct Boom : std::enable_shared_from_this<Boom> {
            Boom() { (void)shared_from_this(); }   // 此时还没接管任何 shared_ptr
        };
        auto b = std::make_shared<Boom>();
        (void)b;
    } catch (const std::exception& e) {
        std::printf("    caught: %s\n", e.what());
    }

    std::printf("[4] 正确姿势：需要时再 self()：\n");
    auto s = std::make_shared<Session>("carol");
    auto self = s->self();
    std::printf("    self() 与 s 指向同一对象？%d ｜ use_count = %ld\n",
                self.get() == s.get(), s.use_count());
    return 0;
}
