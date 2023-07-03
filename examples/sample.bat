@echo off
rem ==================================================================================================================================
rem Local Dev Cluster setup
rem
rem ==================================================================================================================================

echo.
echo Local Dev Cluster Setup
echo =======================
echo.

set SELF=%0
set COMMAND=%1
rem %2 holds the "-c" parameter name
set TILT_CONFIG_DIR=%3

if "%TILT_CONFIG_DIR%" == "" (
	echo Configuration error! TILT_CONFIG_DIR is not set
	exit /B
)
if not exist %TILT_CONFIG_DIR% (
	echo Configuration error! %TILT_CONFIG_DIR% does not exist!
	exit /B
)
echo Using Tilt installation: %TILT_CONFIG_DIR%
echo.

call %TILT_CONFIG_DIR%\cluster-config.bat

call :parse_yaml_config_value "cluster_name"
set CLUSTER_NAME=%YAML_VALUE%
call :parse_yaml_config_value "cluster_namespace"
set NAMESPACE=%YAML_VALUE%

if "%COMMAND%" == "create" goto create_cluster
if "%COMMAND%" == "destroy" goto destroy_cluster
if "%COMMAND%" == "stop" goto stop_cluster
if "%COMMAND%" == "start" goto start_cluster
if "%COMMAND%" == "reset" goto reset_cluster
if "%COMMAND%" == "status" goto status
if "%COMMAND%" == "attach" goto attach

echo.
echo Usage: cluster-setup command
echo.
echo Available commands:
echo   create                      - Create the cluster
echo   destroy                     - Destroy the cluster
echo   stop                        - Stop the cluster
echo   start                       - Start the cluster
echo   reset                       - Reset the cluster
echo   status                      - Show the cluster status
echo   attach                      - Check cluster status, attach to the cluster if it is running
exit /B

:parse_yaml_config_value
rem local config has precedence over deploy config
@SET YAML_VALUE=
FOR /F %%I IN ('yq .%~1 "%TILT_CONFIG_DIR%\local_config.yml"') DO @SET "YAML_VALUE=%%I"

if "%YAML_VALUE%" == "null" (
  FOR /F %%I IN ('yq .%~1 "%TILT_CONFIG_DIR%\deploy_config.yml"') DO @SET "YAML_VALUE=%%I"
)
exit /B

:create_cluster
if "%CLUSTER_MODE%" == "remote" (
  echo Cluster is set to remote. Skipping cluster creation.

  exit /B
)

echo Creating cluster %CLUSTER_NAME%

call :get_cluster_status cluster_status
if not "%cluster_status%" == "Not Present" (
  echo.
  echo The cluster already exists. Please start or destroy it.
  exit /B
)

if "%CLUSTER_HTTP_PORT%" == "" (
	echo.
    echo CLUSTER_HTTP_PORT not set. Defaulting to 80...
	set CLUSTER_HTTP_PORT=80
	echo.
)

if "%CLUSTER_HTTPS_PORT%" == "" (
	echo.
    echo CLUSTER_HTTPS_PORT not set. Defaulting to 443...
	set CLUSTER_HTTPS_PORT=443
	echo.
)

netstat -an | grep 0.0.0.0:%CLUSTER_HTTP_PORT% >nul
if "%errorlevel%" == "0" (
  echo.
  echo The configured CLUSTER_HTTP_PORT port %CLUSTER_HTTP_PORT% is already in use.
  echo Is the cluster already running?
  echo Please terminate the service using that port or configure a different port.
  exit /B
)

netstat -an | grep 0.0.0.0:%CLUSTER_HTTPS_PORT% >nul
if "%errorlevel%" == "0" (
  echo.
  echo The configured CLUSTER_HTTPS_PORT port %CLUSTER_HTTPS_PORT% is already in use.
  echo Is the cluster already running?
  echo Please terminate the service using that port or configure a different port.
  exit /B
)

echo Using HTTP Port %CLUSTER_HTTP_PORT%
echo Using HTTPS Port %CLUSTER_HTTPS_PORT%

setlocal
set PORTS=--port %CLUSTER_HTTP_PORT%:80@loadbalancer --port %CLUSTER_HTTPS_PORT%:443@loadbalancer
set IMAGE=rancher/k3s:v1.22.5-k3s2
set KAFDROP_PROTOBUF_DESCRIPTORS_DIRECTORY=%TILT_CONFIG_DIR%\.kafdrop_protobuf_descriptors
if not "%IMAGE%" == "" (
  set IMAGE_DIRECTIVE=--image %IMAGE%
)
if not exist "%KAFDROP_PROTOBUF_DESCRIPTORS_DIRECTORY%" (
	mkdir %KAFDROP_PROTOBUF_DESCRIPTORS_DIRECTORY%
)
k3d cluster create %CLUSTER_NAME% %PORTS% %IMAGE_DIRECTIVE% --servers-memory %CLUSTER_SERVERS_MEMORY% --k3s-arg "--disable=traefik@server:0" --registry-create %CLUSTER_NAME%-registry --volume %KAFDROP_PROTOBUF_DESCRIPTORS_DIRECTORY%:/kafdrop_protobuf_descriptors
endlocal
if errorlevel 1 (
  exit 1 /B
)

call :set_namespace

echo.
echo Cluster %CLUSTER_NAME% created.
echo.

echo You can now deploy the configured profile using "gmvn tilt:tilt-up".
echo.
exit /B

:reset_cluster
call :get_cluster_status cluster_status
if not "%cluster_status%" == "Running" (
  echo.
  echo The cluster is not running.
  exit /B
)
echo.
echo Resetting cluster
kubectl delete deploy -A -l app.kubernetes.io/managed-by=tilt
kubectl delete sts -A -l app.kubernetes.io/managed-by=tilt
kubectl delete svc -A -l app.kubernetes.io/managed-by=tilt
kubectl delete ing -A -l app.kubernetes.io/managed-by=tilt
kubectl delete cm -A -l app.kubernetes.io/managed-by=tilt
kubectl delete secrets -A -l app.kubernetes.io/managed-by=tilt
kubectl delete role -A -l app.kubernetes.io/managed-by=tilt
kubectl delete rolebinding -A -l app.kubernetes.io/managed-by=tilt
kubectl get pvc --no-headers=true -o custom-columns=:metadata.name | xargs kubectl delete pvc
exit /B

:destroy_cluster
if "%CLUSTER_MODE%" == "remote" (
  echo Cluster is set to remote.
  echo Remote clusters cannot be managed using this script.
  exit /B
)

call :get_cluster_status cluster_status
if "%cluster_status%" == "Running" (
	echo.
	echo Deleting namespace %NAMESPACE%
	kubectl delete ns %NAMESPACE%
)
echo.
echo Destroying cluster %CLUSTER_NAME%

k3d cluster delete %CLUSTER_NAME%

echo.
echo Cleaning up networks
docker network rm %CLUSTER_NAME% 2>nul

exit /B

:stop_cluster
if "%CLUSTER_MODE%" == "remote" (
  echo Cluster is set to remote.
  echo Remote clusters cannot be managed using this script.
  exit /B
)
call :get_cluster_status cluster_status
echo.
if "%cluster_status%" == "Running" (
	echo Stopping cluster %CLUSTER_NAME%
	k3d cluster stop %CLUSTER_NAME%
) else if "%cluster_status%" == "Not Present" (
	echo The cluster not present. Please create one.
) else if "%cluster_status%" == "Stopped" (
	echo Cluster %CLUSTER_NAME% already stopped
) else (
	echo Unknown cluster status: "%cluster_status%"
)

exit /B

:start_cluster
if "%CLUSTER_MODE%" == "remote" (
  echo Cluster is set to remote.
  echo Remote clusters cannot be managed using this script.
  exit /B
)
call :get_cluster_status cluster_status
echo.
if "%cluster_status%" == "Running" (
	echo The cluster is already running.
) else if "%cluster_status%" == "Not Present" (
	echo The cluster not present. Please create one.
) else if "%cluster_status%" == "Stopped" (
	echo Starting cluster %CLUSTER_NAME%
	k3d cluster start %CLUSTER_NAME%
  call :set_namespace
) else (
	echo Unknown cluster status: "%cluster_status%"
)
exit /B

:attach
call :set_namespace

exit /B

:status
call :get_cluster_status cluster_status
echo.
call :list_cluster
exit /B

:set_namespace
echo.
echo Using context k3d-%CLUSTER_NAME%
kubectl config use-context k3d-%CLUSTER_NAME%
echo.
echo Setting namespace %NAMESPACE%
kubectl config set-context --current --namespace=%NAMESPACE%
exit /B

:get_cluster_status

echo | set /p="Determining %CLUSTER_MODE% cluster status... "

setlocal EnableDelayedExpansion
if "%CLUSTER_MODE%" == "remote" (
	kubectl get pods >nul 2>nul
	if "%errorlevel%" == "0" (
	  set cluster_status=Running
	) else (
	  set cluster_status=Not Present
	)
) else (
	FOR /F "delims=" %%i IN ('k3d cluster list ^| grep %CLUSTER_NAME% ^| cut "-d " -f 2- ^| tr -s [:blank:] ^| cut "-d " -f 2') DO set status=%%i
	call :trim cluster_status !status!
	if "!cluster_status!" == "" (
		set cluster_status=Not Present
	)
	if "!cluster_status!" == "0/1" (
	  set cluster_status=Stopped
	) else if "!cluster_status!" == "1/1" (
	  set cluster_status=Running
	)
)
echo !cluster_status!
for /f "delims=" %%a in ("!cluster_status!") do EndLocal & set %1=%%a
exit /B

:list_cluster
if not "%CLUSTER_MODE%" == "remote" (
  k3d cluster list
)
exit /B

:trim
setlocal EnableDelayedExpansion
set input=%*
for /f "tokens=1*" %%a in ("!input!") do EndLocal & set %1=%%b
exit /B

:check_prerequisites
set %1=OK
if not exist %TILT_CONFIG_DIR%\cluster-config.bat (
  echo Cluster configuration file %TILT_CONFIG_DIR%\cluster-config.bat not found.
  echo Please create it by copying %TILT_CONFIG_DIR%\cluster-config-example.bat.
  set %1=NOK
  echo.
)

if not exist %TILT_CONFIG_DIR%\local_config.yml (
  echo Cluster configuration file %TILT_CONFIG_DIR%\local_config.yml not found.
  echo Please create it by copying %TILT_CONFIG_DIR%\local_config_example.yml.
  set %1=NOK
  echo.
)

exit /B
