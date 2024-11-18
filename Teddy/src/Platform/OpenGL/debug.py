import os

def write_files_to_txt(extensions):
    output_file = 'merged_output.txt'  # Output file name
    
    with open(output_file, 'w', encoding='utf-8') as outfile:
        # Walk through all directories and subdirectories
        for root, dirs, files in os.walk('.'):
            # Process each file in the current directory
            for file in files:
                # Check if file has desired extension or is a Makefile
                if file.endswith(tuple(extensions)) or file == "Makefile":
                    # Construct full file path
                    file_path = os.path.join(root, file)
                    try:
                        with open(file_path, 'r', encoding='utf-8') as infile:
                            outfile.write(f"\n{'='*40}\n")
                            outfile.write(f"Content of {file_path}:\n")
                            outfile.write(f"{'='*40}\n\n")
                            outfile.write(infile.read())
                            outfile.write("\n\n")
                    except Exception as e:
                        print(f"Could not read {file_path}: {e}")

if __name__ == "__main__":
    extensions = ['.cpp', '.ld', '.s', '.h']
    write_files_to_txt(extensions)