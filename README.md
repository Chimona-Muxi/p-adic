# p-adic Converter

A terminal-based p-adic number conversion tool supporting integer and rational p-adic expansions, as well as p-adic square root computation.

## Features

- Integer to p-adic conversion
- Rational number to p-adic conversion
- p-adic square root computation (via Hensel's Lemma)
- Expression evaluation and p-adic conversion
- Interactive command-line interface
- Supports 9 languages (EN / ZH-CN / ZH-TW / JA / KO / FR / RU / ES / DE)

## Requirements

| System | Requirements |
|--------|-------------|
| macOS | gcc, make (run `xcode-select --install`) |
| Linux | gcc, make (`sudo apt install gcc make`) |
| Windows | MinGW (gcc + make), see instructions below |

## Installation

### macOS / Linux

```bash
git clone -b main2.0 https://github.com/Chimona-Muxi/p-adic.git
cd p-adic/p_adic_pro
bash install.sh
```

### Windows

**Prerequisite: Install MinGW**
1. Download and install MinGW from https://www.mingw-w64.org/downloads/
2. Add the MinGW bin directory (e.g. `C:\mingw64\bin`) to your system PATH

**Install p-adic:**
1. Download this project (click Code → Download ZIP on the top right)
2. Extract and navigate to the `p_adic_pro` folder
3. Run in Command Prompt:
```bat
install.bat
```
4. Restart your terminal and type `smc-padic` to use

### Windows (WSL method, recommended)

If you prefer not to install MinGW, use WSL instead:

```powershell
# Run in PowerShell as Administrator
wsl --install
```

After restarting, open a WSL terminal and follow the macOS/Linux steps above.

## Usage

After installation, run:

```bash
smc-padic
```

This launches an interactive mode with the following input formats:

```
# Integer conversion
5 7          → 5 in 7-adic representation

# Rational conversion
1/3 5        → 1/3 in 5-adic representation

# Square root
sqrt(2) 5    → √2 in 5-adic representation
pow(2,1/2) 5 → same as above

# Commands
help         → show help
language     → show current language
menu         → open main menu
quit / exit  → exit
```

## Uninstallation

### macOS / Linux
```bash
sudo rm /usr/local/bin/smc-padic
sudo rm /usr/local/bin/padic_converter
rm ~/.padic_config
```

### Windows
Delete the installation directory (default: `C:\p-adic`), remove it from your system PATH, and delete `%USERPROFILE%\.padic_config`.

## About the Name

p-adic numbers are a number system introduced by Kurt Hensel in 1897. This tool implements basic p-adic arithmetic and expansion, including a square root lifting algorithm based on Hensel's Lemma.
