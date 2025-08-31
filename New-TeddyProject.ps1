param (
    [string]$Name
)

$EditorPath = ".\bin\Debug-windows-x86_64\TeddEditor\TeddEditor.exe"

if (-not (Test-Path $EditorPath)) {
    $EditorPath = ".\bin\Release-windows-x86_64\TeddEditor\TeddEditor.exe"
}

if (-not (Test-Path $EditorPath)) {
    Write-Error "Could not find TeddEditor.exe"
    exit 1
}

& $EditorPath --new $Name
