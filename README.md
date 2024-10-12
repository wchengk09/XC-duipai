# XC-duipai: 新一代功能强大的对拍器

`XC-duipai`是一款用于替代 [duipai_ji](https://github.com/Wangzehao2009/duipai_ji) 的强大对拍器。

开发者：[wchengk09](https://github.com/wchengk09)

## XC-duipai 的功能

- 普通对拍
- `SPJ`对拍
- `TLE/MLE/RE`检测
- 生成数据

## XC-duipai 的优势

- 功能丰富多样（见上），不只局限于普通的对拍。
- 支持多线程。
- 支持`TLE/MLE/RE`检测。
- 支持`readline`库，可做出类似`bash`终端的效果。

## 安装教程

### 依赖

- `ncurses`库（提供了下载脚本）
- `readline`库（提供了下载脚本）
- `zip`命令（用于将生成的数据打包，如果你只用`XC-duipai`进行对拍操作，不生成数据，可不安装`zip`）
- `vimdiff`（非必须）

### 安装教程

1. 下载`readline`和`ncurses`库。（如果你本机已经安装了这两个库，可忽略）

```bash
./download.sh
```

该脚本会自动下载`readline`和`ncurses`库，并编译。

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
spj [-t]
tle [-t]
gen <testcases> [-t]
time <time_limit>
mem <mem_limit>
threads <threads>
getconf
exe <program> [args...]
exit
quit
clear
```

### run
- 功能：启动普通对拍。该命令首先会编译`rand.cpp`、`std.cpp`、`wa.cpp`三个文件。此时`rand.cpp`会充当数据生成器，`std.cpp`会充当正解，`wa.cpp`会充当错解。对拍会一直进行下去，直到发现了错误数据（`WA`），或者有一个程序`TLE/MLE/RE`。此时，在`XC-duipai`目录下会生成`in.txt`，`std.txt`，`wa.txt`三个文件，表示一组错误数据。对拍过程中，可按`Ctrl+C`中断（`Interrupted`）。
- 选项：
  - `-t`选项：不启动对拍，而是进行测试。该测试会分别运行`std`和`wa`，并从`in.txt`中读入数据，然后在屏幕上显示`std`和`wa`的输出结果。
### spj
- 功能：启动`SPJ`对拍。该命令首先会编译`rand.cpp`、`std.cpp`、`wa.cpp`三个文件。**此时`std.cpp`会充当
`SPJ`**，`rand.cpp`会充当数据生成器,`wa.cpp`会充当错解。
- `SPJ`的格式：`SPJ`会接受两个命令行参数`in_file`和`out_file`，分别表示输入数据，和`wa.cpp`的输出数据。如果这组数据`AC`，`SPJ`应该返回 $0$，否则 `SPJ` 应该返回一个非零数。
- **注意，`XC-duipai`的`SPJ`不支持`SPJ`比较（即运行两个程序，用`SPJ`来比较二者的输出是否一致）**。实际上，如果你需要用`XC-duipai`进行`SPJ`比较，你可以将正解压到`SPJ`当中。
- 选项：
  - `-t`选项：不启动对拍，而是进行测试。该测试会运行`wa`，并从`in.txt`中读入数据，显示`wa`的输出结果，然后显示`SPJ`的返回值和输出。

### tle
- 功能：启动`TLE/MLE/RE`检测功能。有些时候，我们不需要对拍，只需要找到一个让错解`TLE/MLE/RE`的数据，此时`XC-duipai`就是一个绝佳选择。
- **该模式下会忽略`std.cpp`文件。** 此时`rand.cpp`充当数据生成器，`wa.cpp`充当会`TLE/MLE/RE`的程序。程序会一直执行，直到`wa.cpp`出现了`TLE/MLE/RE`等错误，或者用户按下`Ctrl+C`。
- 选项：
  - `-t`选项：不启动对拍，而是进行测试。该测试会运行`wa`，并从`in.txt`中读入数据，然后显示输出结果。

### gen
- 功能：生成数据。**该模式下会忽略`wa.cpp`文件**，此时`rand.cpp`充当数据生成器，`std.cpp`充当正解程序。
- `rand.cpp`格式：`rand.cpp`会接受一个命令行参数`id`，表示要生成的测试点编号。这有利于生成有部分分的数据。
- 命令格式：**`gen <testcases> [-t]`**，其中`testcases`为需要生成的测试点个数。
  - 运行该操作后，你会在`csd/`目录下得到`testcases.zip`文件（需要额外安装`zip`命令），里面包含`1.in`, `1.out`, `2.in`, `2.out`, ..., `<testcases>.in`, `<testcases>.out`，总共`<testcases>`组数据。
  - `-t`选项：不启动数据生成，而是进行测试。该测试会运行`rand`，并传入命令行参数`testcases`，表示生成第`testcases`个测试点，保存到`in.txt`中，然后显示`in.txt`的内容（数据生成器生成的数据）。然后运行`std`，并从`in.txt`中读入数据，并显示其输出结果。

### time <time_limit>

功能：设置时间限制为`time_limit`毫秒。若`time_limit`为`-1`，则无限制。该限制会保存在`config`这个文件中。

### mem <mem_limit>

功能：设置空间限制为`mem_limit` `MB`。若`mem_limit`为`-1`，则无限制。该限制会保存在`config`这个文件中。

### threads <thread_num>

功能：设置线程数为`thread_num`。该配置会保存在文件`config`中。

### getconf

功能：获取当前配置（时间、空间、线程数）。

### exe

- 功能：运行`rand/std/wa`中的一个程序。
- 命令格式：`exe <command> [args]`，其中`<command>`为你要运行的程序，`args`为程序的命令行参数。`<command>`的所有合法取值有以下几种：
  - `r`,`rand`：运行数据生成器。
  - `w`,`wa`：运行错解。
  - `s`,`std`：运行正解。

### quit / exit

功能：退出`XC-duipai`。

### clear

功能：清空屏幕

> This `XC-duipai` has super `XC` powers.