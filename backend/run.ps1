
$ErrorActionPreference = "Stop"

cmake --build "$PSScriptRoot/build"

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

& "$PSScriptRoot/build/one_line_backend.exe"
