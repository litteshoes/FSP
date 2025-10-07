@echo off
REM ============================================================================
REM  FSP 一键清理脚本（Windows .cmd）
REM  功能：清空以下目录中的“内容”，但保留目录本身：
REM   - tools\formind\Formind_Model\
REM   - data\user_uploads\
REM   - data\simulation_results\
REM   - app\static\models\
REM  可选：如需同时清空视频，可将下方 VIDEOS_DIR 注释去掉
REM  使用方法：双击运行或在仓库根目录执行 scripts\cleanup_all.cmd
REM ============================================================================

setlocal enabledelayedexpansion
pushd %~dp0\..

set ROOT=%CD%
set FMODEL_DIR=%ROOT%\tools\formind\Formind_Model
set UPLOADS_DIR=%ROOT%\data\user_uploads
set RESULTS_DIR=%ROOT%\data\simulation_results
set MODELS_DIR=%ROOT%\app\static\models
set VIDEOS_DIR=%ROOT%\app\static\videos
set PYCACHE_DIR1=%ROOT%\app\__pycache__
set PYCACHE_DIR2=%ROOT%\scripts\__pycache__

echo [FSP Clean] ROOT = %ROOT%

call :clear_dir "%FMODEL_DIR%"
call :clear_dir "%UPLOADS_DIR%"
call :clear_dir "%RESULTS_DIR%"
call :clear_dir "%MODELS_DIR%"
call :clear_dir "%VIDEOS_DIR%"
call :clear_dir "%PYCACHE_DIR1%"
call :clear_dir "%PYCACHE_DIR2%"

echo [FSP Clean] 完成。
popd
exit /b 0

:clear_dir
REM 删除参数指定目录下的所有内容，但保留目录本身
set TARGET=%~1
if not exist %TARGET% (
  echo [Skip] 目录不存在: %TARGET%
  goto :eof
)
echo [Clean] %TARGET%
REM 删除文件
del /f /q "%TARGET%\*" >nul 2>nul
REM 删除子目录
for /d %%D in ("%TARGET%\*") do (
  rd /s /q "%%~fD" >nul 2>nul
)
REM 确保目录仍然存在
if not exist %TARGET% mkdir %TARGET% >nul 2>nul
goto :eof


