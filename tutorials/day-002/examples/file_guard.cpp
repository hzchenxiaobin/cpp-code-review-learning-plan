// file_guard.cpp —— RAII 版：把 fclose 绑定到对象生命周期，异常也逃不掉
// 编译：clang++ -std=c++17 -Wall -Wextra -Wpedantic file_guard.cpp -o file_guard
// 运行：./file_guard
#include <cstdio>
#include <stdexcept>

class FileGuard {
public:
    explicit FileGuard(const char* path, const char* mode)
        : f_(std::fopen(path, mode)) {
        if (f_ == nullptr) throw std::runtime_error("open failed");
    }

    ~FileGuard() {                 // 核心动作：无论正常返回还是异常栈展开，
        if (f_ != nullptr) {       // 析构函数都会执行 → fclose 必然发生
            std::fclose(f_);
            f_ = nullptr;
        }
    }

    // 拷贝会让两个对象关同一个文件（双重关闭），直接禁掉；
    // 移动语义的完整写法 Day 3 用 unique_ptr 讲
    FileGuard(const FileGuard&) = delete;
    FileGuard& operator=(const FileGuard&) = delete;

    FILE* get() const { return f_; }

private:
    FILE* f_;
};

void write_report(const char* path, bool data_ok) {
    FileGuard f(path, "w");                       // 获取即初始化

    std::fputs("line1: 收盘数据\n", f.get());
    std::fputs("line2: 风险指标\n", f.get());

    if (!data_ok)
        throw std::runtime_error("数据异常，中止写报告");
    // 即便从这里飞出去，~FileGuard() 也会先 fclose，数据落盘

    std::fputs("line3: 总结\n", f.get());
}

int main() {
    try {
        write_report("report_raii.txt", /*data_ok=*/false);
    } catch (const std::exception& e) {
        std::printf("caught: %s\n", e.what());
    }

    FileGuard in("report_raii.txt", "r");         // 读文件也用同一个类
    int c = std::fgetc(in.get());
    std::printf("重开文件读取: %s\n",
                c != EOF ? "有内容！异常也没丢数据" : "（空）");
    return 0;
}
