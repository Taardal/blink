#!/bin/bash

#cd lua

# Directory containing Lua files
lua_dir="lua"

# Check if the directory exists
if [ ! -d "$lua_dir" ]; then
    echo "Directory not found: $lua_dir"
    exit 1
fi

for lua_file in "$lua_dir"/*.lua; do
  luac -o "${lua_file%.lua}.out" "$lua_file"
  if [ $? -eq 0 ]; then
    echo "Compiled: $lua_file"
  else
    echo "Compilation failed for: $lua_file"
  fi
done

