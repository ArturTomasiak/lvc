#!/usr/bin/env sh

SCRIPT_DIR="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"

find "$SCRIPT_DIR" -type f \( -name '*.cpp' -o -name '*.hpp' \) \
  -exec clang-format -i {} +