@echo off

if "%1" == "" (
	@echo invalid parameter
	goto USAGE
)
if "%2" == "" (
	@echo invalid parameter
	goto USAGE
)
goto RUN

:USAGE
@echo usage: %0 [node_id] [game_client_instance]
goto END


:RUN
start cmd /k "bridge-server-app -i %1"
start cmd /k "game-server-app -i %1"
start cmd /k "game-client-app -i %1 -n %2"

:END
