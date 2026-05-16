# Fix hardcoded values in codegen.c
$file = "tools/compiler/codegen.c"
$content = Get-Content $file -Raw

# Replace hardcoded values with enum constants
$content = $content -replace 'if \(arg_node->data\.binary_op\.op == 0\)\s*/\*\s*BINOP_ADD\s*\*/', 'if (arg_node->data.binary_op.op == BINOP_ADD)'
$content = $content -replace '\(left_node && left_node->type == 44\)', '(left_node && left_node->type == NODE_LITERAL_STRING)'
$content = $content -replace '\(right_node && right_node->type == 44\)', '(right_node && right_node->type == NODE_LITERAL_STRING)'
$content = $content -replace '\(left_node && left_node->type == 45\)', '(left_node && left_node->type == NODE_IDENTIFIER)'
$content = $content -replace '\(right_node && right_node->type == NODE_IDENTIFIER', '(right_node && right_node->type == NODE_IDENTIFIER'

# Add debug output after line 2066
$content = $content -replace '(ASTNode\* right_node = codegen_get_node\(codegen, right_idx\);)\s*\n\s*\n(\s*if \(arg_node->data\.binary_op\.op)', "`$1`n`n                printf(`"[DEBUG] BINARY_OP: op=%d (BINOP_ADD=%d), left_type=%d, right_type=%d\n`",`n                       arg_node->data.binary_op.op, BINOP_ADD,`n                       left_node ? left_node->type : 0,`n                       right_node ? right_node->type : 0);`n                `n`$2"

# Add debug output after setting first_arg_is_string in BINARY_OP
$content = $content -replace '(if \(\(left_node && left_node->type == NODE_LITERAL_STRING\) \|\|\s*\n\s*\(right_node && right_node->type == NODE_LITERAL_STRING\)\) \{\s*\n\s*first_arg_is_string = 1;)', "`$1`n                        printf(`"[DEBUG] String literal found in BINARY_OP!\n`");"

Set-Content $file $content -NoNewline
Write-Host "Fixed hardcoded values in $file"
