# Build djvm.exe using Open Watcom
$ErrorActionPreference = "Stop"

$WATCOM = "C:\WATCOM"
$env:WATCOM = $WATCOM
$env:PATH = "$WATCOM\binw;$env:PATH"
$env:INCLUDE = "$WATCOM\h"

Write-Host "Building djvm.exe..."

# Change to dosjava directory
Set-Location $PSScriptRoot

# Compile interpreter.c
Write-Host "Compiling interpreter.c..."
& "$WATCOM\binw\wcc.exe" -ms -0 -w4 -zq -od -d2 -fo=build\obj\interpreter.obj src\vm\interpreter.c
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to compile interpreter.c"
    exit 1
}

# Compile memory.c
Write-Host "Compiling memory.c..."
& "$WATCOM\binw\wcc.exe" -ms -0 -w4 -zq -od -d2 -fo=build\obj\memory.obj src\vm\memory.c
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to compile memory.c"
    exit 1
}

# Link
Write-Host "Linking djvm.exe..."
& "$WATCOM\binw\wlink.exe" system dos file build\obj\interpreter.obj,build\obj\memory.obj name build\bin\djvm.exe
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to link djvm.exe"
    exit 1
}

Write-Host "Build successful: build\bin\djvm.exe"

# Made with Bob
