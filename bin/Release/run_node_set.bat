@echo off

if "%1" == "" (
	@echo Invalid parameter.
	@echo usage: %0 [node-id] [client-count]
	@echo  e.g. %0 1 5
	goto :END
)

:START
set node_id=%1

if not "%2" == "" (
set client_count=-n %2
)

start cmd /k node-server-app -i %node_id%
start cmd /k bridge-server-app -i %node_id%
start cmd /k game-server-app -i %node_id%
start cmd /k game-client-app -i %node_id% -n %client_count%

:END


