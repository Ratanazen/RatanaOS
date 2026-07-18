import os
import sys

def check_file(filepath):
    errors = []
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            lines = f.readlines()
    except Exception as e:
        return [f"Failed to read: {e}"]

    # Brace counting
    stack = []
    for i, line in enumerate(lines):
        line_num = i + 1
        for char in line:
            if char in "{[(":
                stack.append((char, line_num))
            elif char in "}])":
                if not stack:
                    errors.append(f"Line {line_num}: Unmatched closing brace '{char}'")
                else:
                    last_char, last_line = stack.pop()
                    expected = {'{':'}', '[':']', '(':')'}[last_char]
                    if char != expected:
                        errors.append(f"Line {line_num}: Mismatched closing brace '{char}', expected '{expected}' to match '{last_char}' on line {last_line}")
    if stack:
        for char, line_num in stack:
            errors.append(f"Line {line_num}: Unmatched opening brace '{char}'")
    return errors

def main():
    has_errors = False
    for root, dirs, files in os.walk('apps'):
        for file in files:
            if file.endswith(('.cpp', '.h')):
                filepath = os.path.join(root, file)
                errors = check_file(filepath)
                if errors:
                    has_errors = True
                    print(f"Errors in {filepath}:")
                    for err in errors:
                        print("  " + err)
    
    for root, dirs, files in os.walk('installer'):
        for file in files:
            if file.endswith(('.cpp', '.h')):
                filepath = os.path.join(root, file)
                errors = check_file(filepath)
                if errors:
                    has_errors = True
                    print(f"Errors in {filepath}:")
                    for err in errors:
                        print("  " + err)

    if not has_errors:
        print("All C++ files passed brace/bracket balance check.")

if __name__ == '__main__':
    main()
