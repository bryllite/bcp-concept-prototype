@echo off

start cmd /k "node-server-app -i 0"
start cmd /k "node-server-app -i 1"
start cmd /k "node-server-app -i 2"

start cmd /k "bridge-server-app -i 0"
start cmd /k "game-server-app -i 0"
start cmd /k "game-client-app -i 0 -n 4"

