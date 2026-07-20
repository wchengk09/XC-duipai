#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

# 1. 若 .env 不存在则创建示例文件
if [ ! -f .env ]; then
	cat > .env <<-EOF
		# 自定义宏定义
		EXTRA_DEFS = -Dkevin
	EOF
	echo "create  .env"
else
	echo "exists  .env"
fi

# 2. 初始化对拍源文件（从模板复制）
make -j init-wa init-std init-rand init-spj
echo "done    make init-*"

# 3. 编译所有程序
make
echo "done    make"
