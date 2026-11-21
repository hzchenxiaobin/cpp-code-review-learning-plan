// leaky_report.cpp —— C 风格资源管理：一条异常路径就能让数据悄悄消失
// 编译：clang++ -std=c++17 -Wall -Wextra -Wpedantic leaky_report.cpp -o leaky_report
// 运行：./leaky_report
#include <cstdio>
#include <stdexcept>

// C 风格：谁打开谁关闭——听起来简单，路径一多必失手
void write_report_c_style(const char* path, bool data_ok) {
    FILE* f = std::fopen(path, "w");            // 资源获取
    if (f == nullptr) throw std::runtime_error("open failed");

    std::fputs("line1: 收盘数据\n", f);         // 数据先进入 stdio 缓冲区
    std::fputs("line2: 风险指标\n", f);

    if (!data_ok)                               // 数据异常：
        throw std::runtime_error("数据异常，中止写报告");   // 泄漏点：异常直接飞走

    std::fputs("line3: 总结\n", f);
    std::fclose(f);                             // 只有“完美路径”才关文件
}

int main(int argc, char** /*argv*/) {
    if (argc > 1) {
        // 模式二（./leaky_report --uncaught）：异常不被任何人捕获，
        // std::terminate → abort：进程非正常退出，exit() 兜底 flush 不会发生
        write_report_c_style("report_leaky.txt", /*data_ok=*/false);
        return 0;
    }

    // 模式一（./leaky_report）：异常被 main 捕获
    try {
        write_report_c_style("report_leaky.txt", /*data_ok=*/false);
    } catch (const std::exception& e) {
        std::printf("caught: %s\n", e.what());
    }

    // 异常被接住后立刻检查：文件里还有我们的数据吗？
    FILE* in = std::fopen("report_leaky.txt", "r");
    if (in == nullptr) {
        std::printf("重开文件读取: 文件不存在！\n");
        return 1;
    }
    int c = std::fgetc(in);
    std::printf("重开文件读取: %s\n",
                c == EOF ? "（空！缓冲区里的数据全丢了）" : "有内容");
    std::fclose(in);
    return 0;
}
