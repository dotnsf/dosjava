# Remove DEBUG printf/fprintf statements from source files

$files = @(
    "tools/compiler/semantic.c",
    "tools/compiler/parser.c",
    "tools/compiler/codegen.c",
    "tools/compiler/symtable.c",
    "src/format/djc.c",
    "src/vm/interpreter.c"
)

foreach ($file in $files) {
    if (Test-Path $file) {
        Write-Host "Processing: $file"
        
        # Read entire file
        $content = Get-Content $file -Raw
        
        # Remove printf statements containing DEBUG (multi-line aware)
        $content = $content -replace 'printf\s*\([^)]*DEBUG[^)]*\)\s*;', ''
        
        # Remove fprintf statements containing DEBUG (multi-line aware)
        $content = $content -replace 'fprintf\s*\([^)]*DEBUG[^)]*\)\s*;', ''
        
        # Remove standalone fflush(stdout);
        $content = $content -replace '(?m)^\s*fflush\s*\(\s*stdout\s*\)\s*;\s*$', ''
        
        # Remove excessive empty lines (more than 2 consecutive)
        $content = $content -replace '(\r?\n){3,}', "`n`n"
        
        # Write back to file without BOM
        $utf8NoBom = New-Object System.Text.UTF8Encoding $false
        [System.IO.File]::WriteAllText($file, $content, $utf8NoBom)
        
        Write-Host "  Done"
    } else {
        Write-Host "  File not found: $file" -ForegroundColor Yellow
    }
}

Write-Host "`nAll files processed."

# Made with Bob
