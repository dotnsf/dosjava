Set-Location "c:\Users\KEIKIMURA\src"

$utf8NoBom = New-Object System.Text.UTF8Encoding($false)

@(".\dosjava\src", ".\dosjava\tools") |
    ForEach-Object {
        Get-ChildItem $_ -Recurse -Include *.c, *.h |
            ForEach-Object {
                $path = $_.FullName
                $text = [System.IO.File]::ReadAllText($path)
                [System.IO.File]::WriteAllText($path, $text, $utf8NoBom)
            }
    }

# Made with Bob
