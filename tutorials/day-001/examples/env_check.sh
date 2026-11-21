#!/usr/bin/env bash
# env_check.sh —— Day 1 环境自检脚本：逐项检查学习环境并给出结论
# 运行：bash env_check.sh
set -u

pass=0; fail=0
ok()  { printf "  [PASS] %s\n" "$1"; pass=$((pass+1)); }
bad() { printf "  [FAIL] %s\n" "$1"; fail=$((fail+1)); }

echo "== 编译器 =="
if command -v clang++ >/dev/null 2>&1; then
  ok "clang++ : $(clang++ --version | head -1)"
else
  bad "未找到 clang++（macOS: xcode-select --install；Linux: 安装 clang 包）"
fi
# 注意：macOS 上 g++ 只是 clang++ 的别名，这正是 Day 1 要认识的一个"坑"
if command -v g++ >/dev/null 2>&1; then
  ok "g++     : $(g++ --version | head -1)"
else
  bad "未找到 g++"
fi

echo "== C++ 标准支持探测 =="
for std in c++17 c++20 c++23; do
  if echo 'int main(){return 0;}' | clang++ -std=$std -x c++ - -o /dev/null 2>/dev/null; then
    ok "-std=$std 可用"
  else
    bad "-std=$std 不可用"
  fi
done

echo "== 常用工具 =="
command -v git     >/dev/null 2>&1 && ok "git     : $(git --version)"                 || bad "未找到 git"
command -v python3 >/dev/null 2>&1 && ok "python3 : $(python3 --version 2>&1)"         || bad "未找到 python3"

echo
echo "结果：$pass 项通过，$fail 项失败"
if [ "$fail" -eq 0 ]; then
  echo "环境就绪，可以开始 Day 1 的练习了 ✅"
else
  echo "请先修复上面的失败项，再继续 ❌"
fi
