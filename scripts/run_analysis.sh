#!/bin/bash

langstat="/Users/boyan/Documents/ETH-Courses/Thesis/cxx-langstat/build/cxx-langstat"

ast_out_dir="/Users/boyan/Documents/ETH-Courses/Thesis/ast-out"
json_out_dir="/Users/boyan/Documents/ETH-Courses/Thesis/json-out"
statistics="stats.langstat.json"

rm -rf "$ast_out_dir"/*
rm -rf "$json_out_dir"/*

find "$@" -type f -name "*.ast" -exec cp {} "$ast_out_dir" \;

"$langstat" --analyses=csla -emit-features -indir "$ast_out_dir" -outdir "$json_out_dir" --  
"$langstat" --analyses=csla -emit-statistics -indir "$json_out_dir" -out "$json_out_dir"/"$statistics"

echo "\n statistics:\n"
cat "$json_out_dir"/"$statistics" | jq -r '.[] | [.name, .value] | @csv' 
