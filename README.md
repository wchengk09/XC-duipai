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

- `run`
  - 功能：启动普通对拍。该命令首先会编译`rand.cpp`、`std.cpp`、`wa.cpp`三个文件。此时`rand.cpp`会充当数据生成器，`std.cpp`会充当正解，`wa.cpp`会充当错解。对拍会一直进行下去，直到发现了错误数据（`WA`），或者有一个程序`TLE/MLE/RE`。此时，在`XC-duipai`目录下会生成`in.txt`，`std.txt`，`wa.txt`三个文件，表示一组错误数据。对拍过程中，可按`Ctrl+C`中断（`Interrupted`）。
  - `-t`选项：不启动对拍，而是进行测试。该测试会分别运行`std`和`wa`，并从`in.txt`中读入数据，然后在屏幕上显示`std`和`wa`的输出结果。
