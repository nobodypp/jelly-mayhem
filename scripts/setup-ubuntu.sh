#!/usr/bin/env bash

set -euo pipefail

echo "Installing Project1 Ubuntu dependencies..."

sudo apt-get update

sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    gdb \
    libx11-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libudev-dev \
    libfreetype6-dev \
    libflac-dev \
    libogg-dev \
    libvorbis-dev

echo
echo "Project1 Ubuntu environment is ready."