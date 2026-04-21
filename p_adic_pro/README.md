# p-adic Converter

一个在终端运行的 p-adic 数转换工具，支持整数、有理数的 p-adic 展开，以及 p-adic 平方根计算。

## 功能

- 整数转换为 p-adic 表示
- 有理数转换为 p-adic 表示
- p-adic 平方根计算（基于 Hensel 引理）
- 表达式求值并转换为 p-adic
- 交互式命令行界面
- 支持 9 种语言（英/简中/繁中/日/韩/法/俄/西/德）

## 环境要求

| 系统 | 要求 |
|------|------|
| macOS | gcc, make（运行 `xcode-select --install`）|
| Linux | gcc, make（`sudo apt install gcc make`）|
| Windows | MinGW (gcc + make)，见下方说明 |

## 安装方法

### macOS / Linux

```bash
# 1. 克隆项目
git clone https://github.com/Chimona-Muxi/p-adic.git

# 2. 进入项目目录
cd p-adic/p_adic_pro

# 3. 运行安装脚本
bash install.sh
```

### Windows

**前置要求：安装 MinGW**
1. 前往 https://www.mingw-w64.org/downloads/ 下载安装
2. 将 MinGW 的 bin 目录（如 `C:\mingw64\bin`）添加到系统 PATH

**安装 p-adic：**
1. 下载本项目（点击页面右上角 Code → Download ZIP）
2. 解压后进入 `p_adic_pro` 文件夹
3. 在命令提示符中执行：
```bat
install.bat
```
4. 重启终端后输入 `smc-padic` 即可使用

### Windows 用户（WSL 方式，推荐）

如果不想安装 MinGW，可以使用 WSL：

```powershell
# 以管理员身份在 PowerShell 中运行
wsl --install
```

重启后打开 WSL 终端，按照 macOS/Linux 步骤操作即可。

## 使用方法

安装完成后，在终端输入：

```bash
smc-padic
```

启动后进入交互模式，支持以下输入格式：

```
# 整数转换
5 7          → 5 的 7-adic 表示

# 有理数转换
1/3 5        → 1/3 的 5-adic 表示

# 平方根
sqrt(2) 5    → √2 的 5-adic 表示
pow(2,1/2) 5 → 同上

# 特殊命令
help         → 查看帮助
language     → 查看当前语言
menu         → 打开主菜单
quit / exit  → 退出
```

## 卸载方法

### macOS / Linux
```bash
sudo rm /usr/local/bin/smc-padic
sudo rm /usr/local/bin/padic_converter
rm ~/.padic_config
```

### Windows
删除安装目录（默认 `C:\p-adic`），并从系统 PATH 中移除该路径，同时删除 `%USERPROFILE%\.padic_config`。

## 关于项目名称

p-adic 数是数学中的一种数系，由 Kurt Hensel 在 1897 年引入。本工具实现了 p-adic 数的基本运算和展开，包括基于 Hensel 引理的平方根提升算法。