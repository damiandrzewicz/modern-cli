FROM mcr.microsoft.com/devcontainers/cpp:ubuntu-24.04

# Avoid interactive tz prompts
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    clang \
    clang-tidy \
    git \
    gdb \
    python3 \
    python3-pip \
    curl \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Optional: default to clang++ (you can change to g++ if you like)
ENV CC=clang
ENV CXX=clang++

WORKDIR /workspace

ENV CCACHE_DIR=/home/vscode/.ccache
ENV CCACHE_COMPRESS=1
ENV CCACHE_SLOPPINESS=time_macros
