// unique_file.cpp —— 用 unique_ptr + 自定义删除器复刻昨天的 FileGuard
// 编译：clang++ -std=c++17 -Wall -Wextra -Wpedantic unique_file.cpp -o unique_file
// 运行：./unique_file
#include <cstdio>
#include <memory>
#include <stdexcept>

namespace {
struct FileCloser {                        // 函数对象删除器：比函数指针更利于内联
    void operator()(FILE* f) const {
        if (f != nullptr) {
            std::fclose(f);
            std::printf("  [删除器] fclose 已执行\n");
        }
    }
};
using FilePtr = std::unique_ptr<FILE, FileCloser>;

FilePtr open_file(const char* path, const char* mode) {
    FILE* raw = std::fopen(path, mode);
    if (raw == nullptr) throw std::runtime_error("open failed");
    return FilePtr(raw);                   // 工厂返回：所有权交给调用方
}
} // namespace

void write_report(const char* path, bool ok) {
    FilePtr f = open_file(path, "w");      // 昨天手写的类，今天一行搞定
    std::fputs("line1: 唯一所有权\n", f.get());
    std::fputs("line2: 删除器兜底\n", f.get());
    if (!ok) throw std::runtime_error("数据异常");
    std::fputs("line3: 总结\n", f.get());
}

int main() {
    try {
        write_report("report_unique.txt", /*ok=*/false);
    } catch (const std::exception& e) {
        std::printf("caught: %s\n", e.what());
    }

    FILE* in = std::fopen("report_unique.txt", "r");
    int c = (in != nullptr) ? std::fgetc(in) : EOF;
    std::printf("重开文件读取: %s\n",
                c != EOF ? "有内容（异常路径照样落盘）" : "（空）");
    if (in != nullptr) std::fclose(in);
    return 0;
}
