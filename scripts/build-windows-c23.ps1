param(
  [ValidateSet("Release","Debug")]
  [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$Build = Join-Path $Root "build\windows-c23"

$clang = Get-Command clang.exe -ErrorAction Stop
$cmake = Get-Command cmake.exe -ErrorAction Stop
$ninja = Get-Command ninja.exe -ErrorAction Stop

Write-Host "RMAL / RMALC native Windows C23 build"
& $clang.Source --version

& $cmake.Source -S $Root -B $Build -G Ninja -DCMAKE_C_COMPILER=$($clang.Source) -DCMAKE_BUILD_TYPE=$Configuration
& $cmake.Source --build $Build --parallel
& (Join-Path $Build "rmalc.exe") version
& (Join-Path $Build "rmalc.exe") selfcheck
& (Join-Path $Build "rmalc.exe") check (Join-Path $Root "tests\recovered_surface.rmal")
& (Join-Path $Build "rmalc.exe") check (Join-Path $Root "provenance\library\RMALC.rmal")
& (Join-Path $Build "rmalc.exe") run (Join-Path $Root "examples\hello.rmal")
& ctest --test-dir $Build --output-on-failure

Write-Host "RMAL C23 Windows build: PASS"
