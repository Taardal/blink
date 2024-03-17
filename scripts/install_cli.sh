#!/bin/bash

# Exit when any command fails
set -e

# Work from project root
if [[ "$(pwd)" == */scripts ]]; then
  cd ..
fi

# Run CMake script to install CLI
command="cmake"
if which ninja >/dev/null; then
  command+=" -D GENERATOR=Ninja"
fi
command+=" -D CMAKE_BUILD_TYPE=Release"
command+=" -P cmake/install_cli.cmake"
eval "${command}"

# Make CLI executable
chmod +x "bl"

echo ""
