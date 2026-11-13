#!/usr/bin/env bash

set -euo pipefail

echo "Installing Jelly-Mayhem Ubuntu dependencies..."

sudo apt update

sudo apt install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    gdb \
    dpkg-dev \
    libx11-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libudev-dev \
    libgl1-mesa-dev \
    libfreetype6-dev \
    libharfbuzz-dev \
    libvorbis-dev \
    libflac-dev \
    libmbedtls-dev \
    libssl-dev

echo
echo "Jelly-Mayhem Ubuntu environment is ready."