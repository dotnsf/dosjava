# Dump hex bytes of a file
param(
    [string]$FilePath
)

if (Test-Path $FilePath) {
    $bytes = [System.IO.File]::ReadAllBytes($FilePath)
    Write-Host "File: $FilePath"
    Write-Host "Size: $($bytes.Length) bytes"
    Write-Host ""
    
    for ($i = 0; $i -lt [Math]::Min($bytes.Length, 512); $i += 16) {
        $line = "{0:X4}: " -f $i
        $end = [Math]::Min($i + 16, $bytes.Length)
        
        for ($j = $i; $j -lt $end; $j++) {
            $line += "{0:X2} " -f $bytes[$j]
        }
        
        Write-Host $line
    }
} else {
    Write-Host "File not found: $FilePath"
}
