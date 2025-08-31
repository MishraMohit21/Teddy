param (
    [string]$Configuration = "Debug"
)

$ScriptCoreProject = "..\ScriptCore\ScriptCore.csproj"

dotnet build $ScriptCoreProject -c $Configuration
