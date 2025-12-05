#!/usr/bin/env bash
set -euo pipefail

# Directories to check in the *current* directory
dirs=(src include bin test)

for dir in "${dirs[@]}"; do
  if [[ -d "$dir" ]]; then
    echo "Formatting in: ./$dir"
    find "$dir" -type f \( -name "*.cpp" -o -name "*.hpp" \) -print0 \
      | xargs -0 -r clang-format -i --style=LLVM
  fi
done

