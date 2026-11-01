param(
    [switch]$Install
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$build = Join-Path $root 'build'

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    throw 'CMake was not found in PATH. Install CMake 3.22+ and reopen PowerShell.'
}
if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    throw 'Git was not found in PATH. Install Git for Windows and reopen PowerShell.'
}

Write-Host 'Configuring Hoonaar Audio Radif Atlas (Windows x64 / VST3)...' -ForegroundColor Cyan
cmake -S $root -B $build -G 'Visual Studio 17 2022' -A x64
if ($LASTEXITCODE -ne 0) { throw "CMake configure failed with exit code $LASTEXITCODE" }

Write-Host 'Building Release VST3...' -ForegroundColor Cyan
cmake --build $build --config Release --target HoonaarRadifAtlas_VST3 --parallel
if ($LASTEXITCODE -ne 0) { throw "Build failed with exit code $LASTEXITCODE" }

$artifactRoot = Join-Path $build 'HoonaarRadifAtlas_artefacts'
$artifact = Get-ChildItem -Path $artifactRoot -Filter 'Radif Atlas.vst3' -Directory -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
if ($null -eq $artifact) {
    throw "Build finished, but 'Radif Atlas.vst3' was not found under $artifactRoot. Inspect the build output."
}

Write-Host "VST3 created at: $($artifact.FullName)" -ForegroundColor Green

if ($Install) {
    $destination = Join-Path $env:CommonProgramFiles 'VST3\Radif Atlas.vst3'
    $parent = Split-Path -Parent $destination
    New-Item -ItemType Directory -Path $parent -Force | Out-Null
    if (Test-Path $destination) { Remove-Item $destination -Recurse -Force }
    Copy-Item -Path $artifact.FullName -Destination $destination -Recurse -Force
    Write-Host "Installed to: $destination" -ForegroundColor Green
    Write-Host 'Open REAPER and re-scan VST3 plug-ins.' -ForegroundColor Yellow
} else {
    Write-Host 'To install, copy the .vst3 bundle to the VST3 folder scanned by REAPER.' -ForegroundColor Yellow
}
