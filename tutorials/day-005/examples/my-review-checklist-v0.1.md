<!-- my-review-checklist.md · v0.1 —— Day 5 复盘定稿的资源类种子（≥10 条） -->
# 我的 C++ 检视清单 v0.1

## 通用
- [ ] 变更是否附带测试？测试是否真的覆盖新行为？
- [ ] 命名与注释是否清楚？（风格交给工具，人看意图）

## 资源与所有权（第 1 周沉淀）
1. 每个 `fopen/malloc/new/lock/socket` 的“获取-释放对”是否都有对应 guard 或容器接管？裸管理要写明理由
2. 释放代码是否只覆盖了“完美路径”？提前 return / throw / break 的出口逐个核对过吗
3. 裸资源是否被 `make_unique` / `unique_ptr + 删除器` 包裹？看到裸 `new` 就问一句
4. 多态基类的析构函数是否 `virtual`？存在“经基类指针 delete”就必查
5. `shared_ptr` 出现时：所有权方向图画过了吗？“回头指”（子持父、观察者持对象、回调持 this）是否已改 `weak_ptr`
6. `make_shared` vs `shared_ptr(new T)`：是否知晓合并分配与 weak 延长内存的权衡（大对象慎用 make_shared + 长寿 weak）
7. `p.get()` 的裸指针是否逃逸到：全局/缓存/成员/异步回调？get 只许“立刻用完”
8. `const` 对象/成员上出现 `std::move`？→ 静默退化为拷贝，性能优化落空
9. 对象被 `std::move` 之后是否又被读取（use-after-move）？string/容器移动后常见为空
10. `use_count()` 是否被用于业务逻辑（==1 判断是竞态）？只允许调试打印
11. 构造函数里是否调用了 `shared_from_this()`？→ 两段式初始化或工厂函数
12. 静态/全局对象里是否囤积只增不减的容器（缓存无淘汰 = 慢泄漏）？

## 编译与工具（Day 1）
13. 本地/CI 是否 `-Wall -Wextra -Wpedantic` 全开？新代码是否 `-Werror`
14. 提交前是否跑过 sanitized 构建（ASan/UBSan，Day 13 起）

## 记录
- v0.1 · Day 5 定稿（12 条资源类 + 2 条工具类）
