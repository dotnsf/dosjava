# Add BOM to file
param([string]$FilePath)

$content = Get-Content -Path $FilePath -Raw
$utf8WithBom = New-Object System.Text.UTF8Encoding $true
[System.IO.File]::WriteAllText((Resolve-Path $FilePath), $content, $utf8WithBom)
Write-Host "Added BOM to $FilePath"
