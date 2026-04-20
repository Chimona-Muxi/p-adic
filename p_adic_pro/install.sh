#!/bin/zsh

echo "============================================"
echo " p-adic Converter - Installation"
echo "============================================"
echo ""
echo "Please select your language / 请选择语言:"
echo ""
echo "  1. English"
echo "  2. 简体中文"
echo "  3. 繁體中文"
echo "  4. 日本語"
echo "  5. 한국어"
echo "  6. Français"
echo "  7. Русский"
echo "  8. Español"
echo "  9. Deutsch"
echo ""

while true; do
    printf "Enter number (1-9): "
    read lang_choice

    case $lang_choice in
        1) LANG_CODE="en"; LANG_NAME="English"; break ;;
        2) LANG_CODE="zh_CN"; LANG_NAME="简体中文"; break ;;
        3) LANG_CODE="zh_TW"; LANG_NAME="繁體中文"; break ;;
        4) LANG_CODE="ja"; LANG_NAME="日本語"; break ;;
        5) LANG_CODE="ko"; LANG_NAME="한국어"; break ;;
        6) LANG_CODE="fr"; LANG_NAME="Français"; break ;;
        7) LANG_CODE="ru"; LANG_NAME="Русский"; break ;;
        8) LANG_CODE="es"; LANG_NAME="Español"; break ;;
        9) LANG_CODE="de"; LANG_NAME="Deutsch"; break ;;
        *) echo "Invalid choice, please enter 1-9." ;;
    esac
done

echo ""
echo "Selected: $LANG_NAME"
echo ""

# 检测系统
OS="$(uname)"
if [[ "$OS" == "Darwin" ]]; then
    echo "✅ Detected: macOS"
elif [[ "$OS" == "Linux" ]]; then
    echo "✅ Detected: Linux"
else
    echo "❌ Unsupported system: $OS"
    echo "   Windows users: please install WSL first."
    echo "   https://learn.microsoft.com/windows/wsl/install"
    exit 1
fi

# 检测 gcc
if ! command -v gcc &> /dev/null; then
    echo "❌ gcc not found. Please install gcc first:"
    echo "   macOS: brew install gcc"
    echo "   Linux: sudo apt install gcc"
    exit 1
fi

# 检测 make
if ! command -v make &> /dev/null; then
    echo "❌ make not found. Please install make first:"
    echo "   macOS: xcode-select --install"
    echo "   Linux: sudo apt install make"
    exit 1
fi

# 编译
echo ""
echo "🔨 Compiling..."
make clean && make

if [[ $? -ne 0 ]]; then
    echo "❌ Compilation failed."
    exit 1
fi

echo "✅ Compilation successful."

# 写入配置文件
CONFIG_FILE="$HOME/.padic_config"
echo "LANG=$LANG_CODE" > "$CONFIG_FILE"
echo "✅ Language config saved to $CONFIG_FILE"

# 安装主程序
echo ""
echo "📦 Installing..."
sudo cp padic_converter /usr/local/bin/padic_converter
sudo chmod +x /usr/local/bin/padic_converter

# 安装 smc-padic 启动脚本
sudo tee /usr/local/bin/smc-padic > /dev/null << 'EOF'
#!/bin/zsh
padic_converter
EOF
sudo chmod +x /usr/local/bin/smc-padic

echo ""
echo "============================================"
echo "✅ Installation complete!"
echo "   Run: smc-padic"
echo "============================================"