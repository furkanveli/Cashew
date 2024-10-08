@echo off
setlocal

set VshaderDir=CashewEngine\src\Engine\Shaders\Vertex
set PshaderDir=CashewEngine\src\Engine\Shaders\Pixel
set outputDir=CashewEngine\src\Engine\Shaders\ShadersCompiled


for %%f in ("%VshaderDir%\*.hlsl") do (
    echo Compiling %%f...
    dxc.exe -T vs_5_1 -E main -Fo "%outputDir%\%%~nf.cso" "%%f"
)
for %%f in ("%PshaderDir%\*.hlsl") do (
    echo Compiling %%f...
    dxc.exe -T ps_5_1 -E main -Fo "%outputDir%\%%~nf.cso" "%%f"
)

echo Compilation complete.
endlocal

call premake5 vs2022
PAUSE