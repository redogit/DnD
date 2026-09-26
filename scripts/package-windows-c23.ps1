param(
  [ValidateSet("Release","Debug")]
  [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$Build = Join-Path $Root "build\windows-c23"
$Stage = Join-Path $Root "stage\windows-c23"
$Dist = Join-Path $Root "dist"

& (Join-Path $PSScriptRoot "build-windows-c23.ps1") -Configuration $Configuration

if (Test-Path $Stage) { Remove-Item -Recurse -Force $Stage }
if (!(Test-Path $Dist)) { New-Item -ItemType Directory -Path $Dist | Out-Null }

cmake --install $Build --prefix $Stage --config $Configuration

$Rmalc = Join-Path $Stage "bin\rmalc.exe"
& $Rmalc version
& $Rmalc selfcheck

cpack --config (Join-Path $Build "CPackConfig.cmake") -G ZIP -B $Dist -D "CPACK_PACKAGE_FILE_NAME=RMAL-3.1.0-windows-x64-c23"

$Zip = Join-Path $Dist "RMAL-3.1.0-windows-x64-c23.zip"
if (!(Test-Path $Zip)) { throw "Package was not produced: $Zip" }

Write-Host "RMAL Windows C23 package: $Zip"
