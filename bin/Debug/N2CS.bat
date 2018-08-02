@echo off

if "%1" == "" (
	@echo invalid parameter
	goto USAGE
)
goto RUN

:USAGE
@echo usage: %0 [game_client_instance]
goto END

:RUN
start cmd /k "node-server-app -i 0 -l 1"
start cmd /k "bridge-server-app -i 0"
start cmd /k "game-server-app -i 0"
start cmd /k "game-client-app -i 0 -n %1"

:END