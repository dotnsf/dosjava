#!/usr/bin/env python3
import re
import sys

def remove_debug_output(content):
    # Remove printf/fprintf statements containing DEBUG (including multi-line)
    content = re.sub(r'printf\s*\([^)]*DEBUG[^)]*\)\s*;', '', content, flags=re.DOTALL)
    content = re.sub(r'fprintf\s*\([^)]*DEBUG[^)]*\)\s*;', '', content, flags=re.DOTALL)
    
    # Remove standalone fflush(stdout);
    content = re.sub(r'^\s*fflush\s*\(\s*stdout\s*\)\s*;\s*$', '', content, flags=re.MULTILINE)
    
    # Remove empty lines (more than 2 consecutive)
    content = re.sub(r'\n\n\n+', '\n\n', content)
    
    return content

if __name__ == '__main__':
    files = [
        'tools/compiler/semantic.c',
        'tools/compiler/parser.c',
        'tools/compiler/codegen.c',
        'tools/compiler/symtable.c',
        'src/format/djc.c'
    ]
    
    for filepath in files:
        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                content = f.read()
            
            new_content = remove_debug_output(content)
            
            with open(filepath, 'w', encoding='utf-8', newline='\n') as f:
                f.write(new_content)
            
            print(f"Processed: {filepath}")
        except Exception as e:
            print(f"Error processing {filepath}: {e}", file=sys.stderr)

# Made with Bob
