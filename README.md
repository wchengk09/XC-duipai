# XC-duipai: 新一代功能强大的对拍器

`XC-duipai`是一款用于替代 [duipai_ji](https://github.com/Wangzehao2009/duipai_ji) 的强大对拍器。

开发者：[wchengk09](https://github.com/wchengk09)

## XC-duipai 的功能

- 普通对拍
- `SPJ`对拍
- **`TLE/MLE/RE`检测**
- **生成数据**
- **带 hack 造数据**

## XC-duipai 3.0.0 新变化

- 重构了整个项目，使其更加符合项目规范。

## XC-duipai 2.0.0 新功能

- **正式支持SPJ比较**。
- 将 `main.cpp`的部分代码整理到了`src/`目录中。
- 将`readline`和`ncurses`库整合到了项目中，无需下载。
- 修改了对拍计时器的`BUG`。

## XC-duipai 的优势

- 功能丰富多样（见上），不只局限于普通的对拍。
- **支持多线程。**
- 支持`TLE/MLE/RE`检测。
- 支持`readline`库，可做出类似`bash`终端的效果。
- 兼容`Shell`命令，可在`XC-duipai`的内部直接运行`Shell`命令，无需退出`XC-duipai`。

## 安装教程

### 依赖

- `ncurses`库（提供了下载脚本）
- `readline`库（提供了下载脚本）
- `zip`命令（用于将生成的数据打包，如果你只用`XC-duipai`进行对拍操作，不生成数据，可不安装`zip`）
- `vimdiff`（非必须）

### 安装教程

1. 安装`readline`和`ncurses`库。（如果你本机已经安装了这两个库，可忽略）

```bash
./install.sh
```

该脚本会自动安装`readline`和`ncurses`库。

2. 编译主程序

```bash
make
```

3. 运行主程序

```bash
./main
```

尽情享用吧！

## 使用教程

运行主程序`./main`后，你会得到一个类似于`bash`的终端界面，可用上下键获取历史记录，也可键入命令。

命令格式为：`command [options]`

下面是`XC-duipai`支持的命令：

```data
run [-t]
spjcmp [-t]
spjcheck [-t]
tle [-t]
gen <testcases> [-t]
gen hack <testcases> [-t] [-a] [-o] [-m <max_attempts>]
time <time_limit>
mem <mem_limit>
threads <threads>
getconf
exit
quit
clear
cd <dir>
<任意Shell命令>
```

### 各模式下文件角色一览

| 模式 | `rand.cpp` | `std.cpp` | `wa.cpp` | `spj.cpp` | `hacks/*.cpp` |
|---|---|---|---|---|---|
| `run` | 数据生成器 | 正解 | 错解 | 不使用 | 不使用 |
| `spjcheck` | 数据生成器 | 不使用 | 错解 | SPJ（判断 wa 输出是否合法） | 不使用 |
| `spjcmp` | 数据生成器 | 正解 | 错解 | SPJ（比较 wa 与 std 输出） | 不使用 |
| `tle` | 数据生成器 | 不使用 | 被测程序（等待 TLE/MLE/RE） | 不使用 | 不使用 |
| `gen` | 数据生成器 | 正解 | 不使用 | 不使用 | 不使用 |
| `gen hack` | 数据生成器 | 正解 | 不使用 | 不使用 | 被 hack 的解法 |

### SPJ 的命令行参数

| 模式 | SPJ 接收的参数 | 含义 |
|---|---|---|
| `spjcheck` | `in_file out_file out_file` | 输入数据、wa 输出（传两次，兼容 Testlib） |
| `spjcmp` | `in_file out_file ans_file` | 输入数据、wa 输出、std 输出 |

SPJ 返回 `0` 表示 AC，返回非零表示 WA。

### `-t` 测试模式行为

| 模式 | `-t` 做什么 | 输入来源 |
|---|---|---|
| `run -t` | 显示 in.txt → 运行 wa → 运行 std → 显示两者输出 | `in.txt`（需预先准备） |
| `spjcheck -t` | 显示 in.txt → 运行 wa → 运行 spj → 显示 SPJ 返回值 | `in.txt` |
| `spjcmp -t` | 显示 in.txt → 运行 wa → 运行 std → 运行 spj | `in.txt` |
| `tle -t` | 显示 in.txt 内容（不运行 wa） | `in.txt` |
| `gen -t` | 运行 rand(n) → 保存到 in.txt → 运行 std → 显示输出 | `in.txt`（由 rand 生成） |
| `gen hack -t` | 运行 rand(1) → in.txt → 运行 std → 运行每个 hack → 显示输出与比较结果 | `in.txt`（由 rand 生成） |

### 自动编译范围

| 模式 | 自动编译的文件 |
|---|---|
| `run` | `std` + `wa` + `rand` |
| `spjcheck` | `wa` + `rand` + `spj` |
| `spjcmp` | `std` + `wa` + `rand` + `spj` |
| `tle` | `rand` + `wa` |
| `gen` | `rand` + `std` |
| `gen hack` | `rand` + `std` + `hacks/*.cpp` |

### run
- 功能：启动普通对拍。该命令首先会编译`rand.cpp`、`std.cpp`、`wa.cpp`三个文件。此时`rand.cpp`会充当数据生成器，`std.cpp`会充当正解，`wa.cpp`会充当错解。对拍会一直进行下去，直到发现了错误数据（`WA`），或者有一个程序`TLE/MLE/RE`。此时，在`XC-duipai`目录下会生成`in.txt`，`std.txt`，`wa.txt`三个文件，表示一组错误数据。对拍过程中，可按`Ctrl+C`中断（`Interrupted`）。
- 选项：
  - `-t`选项：不启动对拍，而是进行测试。该测试会分别运行`std`和`wa`，并从`in.txt`中读入数据，然后在屏幕上显示`std`和`wa`的输出结果。
### spjcheck
- 功能：启动`SPJ`检测功能（关于`SPJ`检测与`SPJ`比较的区别，可参阅下文）。该命令首先会编译`rand.cpp`、`spj.cpp`、`wa.cpp`三个文件。**此时`spj.cpp`会充当
`SPJ`**，`rand.cpp`会充当数据生成器,`wa.cpp`会充当错解。
- `SPJ`的格式：`SPJ`会接受三个命令行参数`in_file`、`out_file`、`out_file`（**请注意，为了兼容某些用`Testlib`库编写的`SPJ`，`out_file`这个参数会被传两次**），分别表示输入数据，和`wa.cpp`的输出数据。如果这组数据`AC`，`SPJ`应该返回 $0$，否则 `SPJ` 应该返回一个非零数。
- 选项：
  - `-t`选项：不启动对拍，而是进行测试。该测试会运行`wa`，并从`in.txt`中读入数据，显示`wa`的输出结果，然后显示`SPJ`的返回值和输出。

### spjcmp
- 功能：启动`SPJ`比较功能。该命令首先会编译`rand.cpp`、`std.cpp`、`wa.cpp`、`spj.cpp`四个文件。
- `SPJ`的格式：`SPJ`会接受三个命令行参数`in_file`、`out_file`、`ans_file`，分别表示输入数据、`wa.cpp`的输出数据和`std.cpp`的输出数据。如果这组数据`AC`，`SPJ`应该返回 $0$，否则 `SPJ` 应该返回一个非零数。
- 选项：
  - `-t`选项：不启动对拍，而是进行测试。该测试会运行`wa.cpp`、`std.cpp`、`spj.cpp`并显示`spj`的输出结果。

### tle
- 功能：启动`TLE/MLE/RE`检测功能。有些时候，我们不需要对拍，只需要找到一个让错解`TLE/MLE/RE`的数据，此时`XC-duipai`就是一个绝佳选择。
- **该模式下会忽略`std.cpp`文件。** 此时`rand.cpp`充当数据生成器，`wa.cpp`充当会`TLE/MLE/RE`的程序。程序会一直执行，直到`wa.cpp`出现了`TLE/MLE/RE`等错误，或者用户按下`Ctrl+C`。
- 选项：
  - `-t`选项：不启动检测，而是显示`in.txt`的内容（不运行`wa`），用于快速查看当前输入数据。

### gen
- 功能：生成数据。**该模式下会忽略`wa.cpp`文件**，此时`rand.cpp`充当数据生成器，`std.cpp`充当正解程序。
- `rand.cpp`格式：`rand.cpp`会接受一个命令行参数`id`，表示要生成的测试点编号。这有利于生成有部分分的数据。
- 命令格式：**`gen <testcases> [-t]`**，其中`testcases`为需要生成的测试点个数。
  - 运行该操作后，你会在`csd/`目录下得到`testcases.zip`文件（需要额外安装`zip`命令），里面包含`1.in`, `1.out`, `2.in`, `2.out`, ..., `<testcases>.in`, `<testcases>.out`，总共`<testcases>`组数据。
  - `-t`选项：不启动数据生成，而是进行测试。该测试会运行`rand`，并传入命令行参数`testcases`，表示生成第`testcases`个测试点，保存到`in.txt`中，然后显示`in.txt`的内容（数据生成器生成的数据）。然后运行`std`，并从`in.txt`中读入数据，并显示其输出结果。

### gen hack
- 功能：**带 hack 造数据**。`gen` 的子命令，在 `gen` 的基础上，要求生成的每组数据都能 hack 掉 `hacks/` 目录下的解法（即让它们输出错误答案、`TLE`/`MLE`/`RE`）。常用于构造 hack 数据来攻击他人代码。
- **目录约定**：在项目根目录下创建 `hacks/` 文件夹，里面放置若干份 `*.cpp` 文件，每份是一个被 hack 的解法。可用 `make init-hack` 快速生成一份模板。
  ```
  hacks/
  ├── brute.cpp       # 暴力解法（容易被卡 TLE）
  └── greedy.cpp      # 错误贪心（容易被卡 WA）
  ```
- `rand.cpp` 格式：与 `gen` 相同，接受一个命令行参数 `id`。`gen hack` 模式会以 `id=1,2,3,...` 不断尝试，直到找到足够数量的 hack 数据。默认无尝试上限，若长时间找不到可按 `Ctrl+C` 中断。
- 命令格式：**`gen hack <testcases> [-t] [-a] [-o] [-m <max_attempts>]`**
  - `testcases`：需要找到的 hack 测试点个数。
  - 默认（任意模式）：只要 hack 掉 `hacks/` 中**至少一份**解法，即视为成功，保存该测试点。这符合 Codeforces 等平台的 hack 语义。
  - `-a` 选项（全部模式）：必须 hack 掉 `hacks/` 中的**所有**解法，才视为成功。适用于想构造一组「通杀」数据的场景。
  - `-o` 选项（覆盖模式）：每保存一个测试点，都必须 hack 掉**至少一份之前从未被 hack 过的代码**。适用于想保证测试集覆盖所有错解的场景。可与 `-a` 组合使用。注意：若 `testcases` 大于解法数量，或某解法不可 hack，将无法满足条件而持续运行，此时可按 `Ctrl+C` 中断。
  - `-m <max_attempts>` 选项（兜底模式）：设为 `N`（≥0）时，若连续 `N` 次尝试仍未找到满足 hack 规则的测试点，则**无视 hack 规则直接保存**该测试点（仅运行 `rand` + `std`，跳过 hack 解法）。默认为 `-1`（即 $+\infty$，永不放弃）。`-m 0` 表示完全不尝试 hack，等同于 `gen`。可与 `-a`、`-o` 组合使用。
  - `-t` 选项：不启动造数据，而是进行测试。该测试会运行 `rand(1)` 保存到 `in.txt`，运行 `std` 保存到 `std.txt`，然后依次运行每个 hack 解法，显示其输出并标注是否被 hack（`HACKED` / `not hacked`）。
- hack 判定标准（针对每个 hack 解法）：
  - `TLE` / `MLE` / `RE` → 视为 hack 成功。
  - 输出与 `std` 不同（`diff -Z`，忽略行尾空格）→ 视为 hack 成功。
  - 否则 → 未被 hack。
- 输出：与 `gen` 一致，成功后在 `csd/` 目录下得到 `testcases.zip`，包含 `1.in`, `1.out`, ..., `<n>.in`, `<n>.out`。
- 注意：`gen hack` 模式会使用 `time` / `mem` 配置作为 hack 解法的时限/空限，因此如果想构造 `TLE` 数据，记得先用 `time` 命令设置一个合理的时限。`rand` 与 `std` 不受时限/空限约束。

### time <time_limit>

功能：设置时间限制为`time_limit`毫秒。若`time_limit`为`-1`，则无限制。该限制会保存在`config`这个文件中。

### mem <mem_limit>

功能：设置空间限制为`mem_limit` `MB`。若`mem_limit`为`-1`，则无限制。该限制会保存在`config`这个文件中。

### threads <thread_num>

功能：设置线程数为`thread_num`。该配置会保存在文件`config`中。

### getconf

功能：获取当前配置（时间、空间、线程数）。

### quit / exit

功能：退出`XC-duipai`。

### clear

功能：清空屏幕

## 其它命令

**如果你输入的命令不是上述命令中的任何一个，`XC-duipai` 会把它当作 `Shell` 命令来处理**。它会调用系统终端`/bin/sh`，并运行你输入的命令。

例如，如果你在`XC-duipai`的命令行界面中输入`ls`，它会直接调用系统的`ls`命令。

或者，你可以在`XC-duipai`的界面中直接启动`gdb`：

```bash
gdb ./rand
```

## SPJ比较与SPJ检测的区别

- 有些时候，我们需要利用`SPJ`来比较错解和正解是否正确（比如允许浮点误差），这叫做 **SPJ比较**。
- 有些时候，我们仅仅希望用 `SPJ` 来判断错解输出的数据是否符合格式（比如构造题），这叫做 **SPJ检测**。

## 如何编译？

**`XC-duipai` 的`run`、`spjcheck`、`spjcmp`、`tle`、`gen`、`gen hack`六个命令都会自动编译所需的源代码（详见上文"自动编译范围"表格），因此你无需手动编译。**

当然，如果你想手动编译你的代码，可以直接调用系统的`make`命令编译：

```bash
make <你要编译的代码>
```

例如：

```bash
make wa  # 编译wa.cpp，得到可执行文件wa
make std # 编译std.cpp
make rand
make -j wa std # 同时编译wa和std两个文件，使用多线程编译
make -j run    # 编译wa.cpp,std.cpp,rand.cpp三个文件
make -j hack   # 编译rand.cpp,std.cpp以及hacks/下所有.cpp
```

你也可以通过`make init`初始化你的代码。**请注意，`make init`会覆盖当前代码，因此你需要谨慎操作**。

你可以分别通过`make init-std/init-wa/init-rand/init-spj`初始化`std.cpp`,`wa.cpp`,`rand.cpp`,`spj.cpp`。

`make init-hack` 会创建 `hacks/` 目录并放入一份模板 `hack1.cpp`（不会覆盖已有文件，但若 `hacks/hack1.cpp` 已存在会被覆盖）。

> This `XC-duipai` has super `XC` powers.