# 06 复杂度分析(Ⅰ)

> 课程设计 OJ 题号 **1722**，问题 A：复杂度分析(Ⅰ)。

## 题目

分析如下循环 `printf` 的执行次数，以及循环结束后 `i+j+k` 的值：

```c
for(i=1;i<n;i++)
  for(j=1;j<i;j++)
    for(k=1;k<j;k++)
      printf("\n");
```

## 结论

- **printf 次数** = C(n-1, 3)（组合计数：从 1..n-1 中选三个互异数 i>j>k）。
- **循环结束后 i+j+k = 3n-3**（n ≥ 3；此时 i=j=k=n）。n < 3 时 j/k 未定义，按题目要求输出 `RANDOM`。

## 输入样例

```
6
3
1
2
3000
```

多个 n，逐行输出对应结果。

## 文件

| 文件 | 说明 |
|---|---|
| `main.cpp` | 组合计数 + 边界判断 |
| `sample_in.txt` | 样例输入 |

## 本地验证

```bash
g++ -O2 -std=c++11 -Wall -o main.exe main.cpp
./main.exe < sample_in.txt
```
