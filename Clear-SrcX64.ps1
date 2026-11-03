#requires -Version 5.1
[CmdletBinding(SupportsShouldProcess = $true, ConfirmImpact = 'Medium')]
param()

$ErrorActionPreference = 'Stop'

$RepoRoot = Split-Path -Parent $PSCommandPath
$SrcPath = Join-Path $RepoRoot 'src'
$PackagesPath = Join-Path $SrcPath 'DataDryver\packages'
$PackagesPathPrefix = "$PackagesPath\"

if (-not (Test-Path -LiteralPath $SrcPath -PathType Container)) {
	throw "src directory not found at '$SrcPath'."
}

$x64Directories =
	Get-ChildItem -LiteralPath $SrcPath -Directory -Recurse -Force -Filter 'x64' |
	Where-Object { -not $_.FullName.StartsWith($PackagesPathPrefix, [System.StringComparison]::OrdinalIgnoreCase) } |
	Sort-Object { $_.FullName.Length } -Descending

if (-not $x64Directories) {
	Write-Host "No x64 directories found under '$SrcPath'."
	return
}

foreach ($directory in $x64Directories) {
	if (-not (Test-Path -LiteralPath $directory.FullName -PathType Container)) {
		continue
	}

	if ($PSCmdlet.ShouldProcess($directory.FullName, 'Remove directory recursively')) {
		Remove-Item -LiteralPath $directory.FullName -Recurse -Force
		Write-Host "Removed $($directory.FullName)"
	}
}
