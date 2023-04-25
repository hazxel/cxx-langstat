#!/bin/bash

langstat="/Users/boyan/Documents/ETH-Courses/Thesis/cxx-langstat/build/cxx-langstat"

ast_out_dir="/Users/boyan/Documents/ETH-Courses/Thesis/ast-out"
json_out_dir="/Users/boyan/Documents/ETH-Courses/Thesis/json-out"
statistics="stats.langstat.json"

rm -rf "$ast_out_dir"/*
rm -rf "$json_out_dir"/*

# find "$@" -type f -name "*.ast" -exec cp {} "$ast_out_dir" \;
# find . -name '*.csv' -exec cp --parents \{\} /target \; # parents not supported on macos cp
find "$@" -type f -name "*.ast" | cpio -pdm "$ast_out_dir"
# copy files with .ast suffix and preserve directory structure
# find "$@" -type f -name "*.ast" -exec sh -c '
#     for file do
#         mkdir -p "$1/$(dirname "${file#$2}")"
#         cp -p "$file" "$1/${file#$2}"
#     done' sh "$ast_out_dir" {} +

"$langstat" --analyses=cslma -emit-features -indir "$ast_out_dir" -outdir "$json_out_dir" --  
"$langstat" --analyses=cslma -emit-statistics -indir "$json_out_dir" -out "$json_out_dir"/"$statistics"

echo "\n statistics:\n"
tail -n 20 "$json_out_dir"/"$statistics" # | jq -r '.[] | [.name, .value] | @csv' 
