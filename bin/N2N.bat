@echo off

:RUN
start cmd /k "node-server-app -i 0"
start cmd /k "node-server-app -i 1"
start cmd /k "node-server-app -i 2"

:END
