/*
 * linechk.c - Line Content Checker for DOS
 * 
 * Usage: linechk.exe <filename> <line_number> <expected_content>
 * 
 * Checks if the specified line in a file matches the expected content.
 * Returns 0 if match, 1 if mismatch or error.
 * 
 * Example:
 *   linechk.exe output.txt 5 "Hello World"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

int main(int argc, char *argv[]) {
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    int target_line;
    int current_line = 1;
    char *expected;
    int found = 0;
    
    /* Check arguments */
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <filename> <line_number> <expected_content>\n", argv[0]);
        fprintf(stderr, "Example: %s output.txt 5 \"Hello World\"\n", argv[0]);
        return 1;
    }
    
    /* Parse line number */
    target_line = atoi(argv[2]);
    if (target_line < 1) {
        fprintf(stderr, "ERROR: Invalid line number: %s\n", argv[2]);
        return 1;
    }
    
    expected = argv[3];
    
    /* Open file */
    fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "ERROR: Cannot open file: %s\n", argv[1]);
        return 1;
    }
    
    /* Read lines until target line */
    while (fgets(line, sizeof(line), fp)) {
        /* Remove trailing newline/CR */
        {
            int len = strlen(line);
            while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
                line[len-1] = '\0';
                len--;
            }
        }
        
        if (current_line == target_line) {
            /* Compare with expected content */
            if (strcmp(line, expected) == 0) {
                found = 1;
            } else {
                fprintf(stderr, "MISMATCH at line %d:\n", target_line);
                fprintf(stderr, "  Expected: [%s]\n", expected);
                fprintf(stderr, "  Actual:   [%s]\n", line);
                fclose(fp);
                return 1;
            }
            break;
        }
        current_line++;
    }
    
    fclose(fp);
    
    if (!found) {
        if (current_line < target_line) {
            fprintf(stderr, "ERROR: File has only %d lines, cannot check line %d\n", 
                    current_line - 1, target_line);
        } else {
            fprintf(stderr, "ERROR: Line %d not found\n", target_line);
        }
        return 1;
    }
    
    /* Success - line matches */
    return 0;
}
