#!/usr/bin/env bash
set -euo pipefail

# Simple post-create script to install clangd, clang-format and ccache in the devcontainer.

NEED_APT_UPDATE=0

if command -v clangd >/dev/null 2>&1; then
  echo "clangd already installed: $(command -v clangd)"
else
  NEED_APT_UPDATE=1
fi

if command -v clang-format >/dev/null 2>&1; then
  echo "clang-format already installed: $(command -v clang-format)"
else
  NEED_APT_UPDATE=1
fi

if command -v ccache >/dev/null 2>&1; then
  echo "ccache already installed: $(command -v ccache)"
else
  NEED_APT_UPDATE=1
fi

if [ "$NEED_APT_UPDATE" -eq 1 ]; then
  # Update apt index once if we need to install something.
  sudo apt-get update -y
fi

if ! command -v clangd >/dev/null 2>&1; then
  sudo apt-get install -y clangd
fi

if ! command -v clang-format >/dev/null 2>&1; then
  sudo apt-get install -y clang-format
fi

if ! command -v ccache >/dev/null 2>&1; then
  sudo apt-get install -y ccache
fi

echo "--- Tool versions ---"
# Print versions for debugging.
clangd --version || true
clang-format --version || true
ccache --version || true

echo "clangd, clang-format and ccache installation completed."
