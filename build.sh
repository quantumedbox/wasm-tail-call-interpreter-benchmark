#!/bin/env bash

# Wasm
zig cc tail-call.c -shared -mtail-call -O3 -Wl,--export=main -target wasm32-freestanding-musl -o tail-call.wasm
zig cc jump-table.c -shared -O3 -Wl,--export=main -target wasm32-freestanding-musl -o jump-table.wasm

# Native
zig cc tail-call.c -O3 -o tail-call
zig cc jump-table.c -O3 -o jump-table
