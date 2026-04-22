# p-adic Converter

一个在终端运行的 p-adic 数转换工具。支持整数与有理数的 p-adic 展开、
算术表达式求值并转 p-adic、p-adic 平方根（Hensel 引理提升），以及
**p-adic 指数函数 exp 和对数函数 log**。

## 功能

- 整数 / 有理数 → p-adic 展开（形式表示 + 级数表示 + 循环节识别）
- 算术表达式 (`+ - * /`、括号) 求值，再转为 p-adic —— **默认模式即可直接使用**
- p-adic 平方根（Hensel 引理），自动给出两个解
- `pow(base, exponent)`：整数指数给出精确幂；`1/2` 幂退化为 sqrt
- **p-adic 指数函数 `exp(x)`**：
  - 收敛条件：$v_p(x) \ge 1$（$p>2$），或 $v_p(x) \ge 2$（$p=2$）
  - 级数 $\exp(x) = \sum_{n \ge 0} x^n / n!$
- **p-adic 对数函数 `log(x)` / `ln(x)`**：
  - 收敛条件：$x \equiv 1 \pmod p$（$p>2$），或 $x \equiv 1 \pmod 4$（$p=2$）
  - 级数 $\log(1+y) = \sum_{n \ge 1} (-1)^{n+1} y^n / n$
- 9 种界面语言（英 / 简中 / 繁中 / 日 / 韩 / 法 / 俄 / 西 / 德）

## 环境要求

| 系统 | 要求 |
|------|------|
| macOS | gcc, make（运行 `xcode-select --install`）|
| Linux | gcc, make（`sudo apt install gcc make`）|
| Windows | MinGW (gcc + make)，见下方说明 |

## 安装方法

### macOS / Linux

```bash
git clone -b main2.0 https://github.com/Chimona-Muxi/p-adic.git
cd p-adic/p_adic_pro
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

启动后进入交互模式。**所有输入形式都写在同一行里**，格式为
`<表达式> <p>`，其中 `<p>` 是一个大于 1 的整数：

```
# 最基本的整数 / 有理数
5 7               → 5 的 7-adic 展开
1/3 5             → 1/3 的 5-adic 展开

# 默认模式即可使用的表达式（新）
3+4*2 5           → 11 的 5-adic
(1/3+1)*2 5       → 8/3 的 5-adic
2^10 7            → 1024 的 7-adic   (通过 pow(2,10))
pow(2,10) 7       → 同上

# 平方根
sqrt(2) 5         → √2 的 5-adic（Q_5 中无解，会提示）
sqrt(2) 7         → √2 的 7-adic，给出两个解
pow(2,1/2) 7      → 等价于 sqrt(2) 7

# 指数与对数（新）
exp(5) 5          → exp(5) 的 5-adic 展开
exp(2*3) 3        → exp(6) 的 3-adic 展开
log(1+5) 5        → log(6) 的 5-adic 展开
log(1+3+9) 3      → log(13) 的 3-adic 展开
ln(1+5) 5         → log 的别名

# 特殊命令
help              → 查看帮助
language          → 查看当前语言
menu              → 打开主菜单（含解析器测试、批量测试等）
quit / exit       → 退出
```

### 关于收敛性

p-adic 的 exp 和 log 不是对所有输入都收敛：

| 函数 | 收敛条件 |
|------|----------|
| `exp(x)` | `v_p(x) >= 1` (`p > 2`) 或 `v_p(x) >= 2` (`p = 2`) |
| `log(x)` | `x ≡ 1 (mod p)` (`p > 2`) 或 `x ≡ 1 (mod 4)` (`p = 2`) |

不满足时程序会给出 "不收敛" 的提示，而不是返回错误结果。

### 表达式语法

表达式解析器支持：

- 整数、有理数字面量（`3`、`-5`、`1/7`）
- 运算符 `+ - * /` 与括号 `( )`
- 一元负号
- `sqrt(expr)`、`pow(base, exp)`、`exp(expr)`、`log(expr)` / `ln(expr)`
- 函数参数可以是任意子表达式，例如 `log(1 + 5*2) 5`

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

p-adic 数是数学中的一种数系，由 Kurt Hensel 在 1897 年引入。本工具实现了
p-adic 数的常用运算：展开、循环节识别、Hensel 提升求平方根，以及在
$\mathbb Z_p$ 上收敛的指数与对数级数。
