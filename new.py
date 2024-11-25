import os
import shutil

def convert_source_to_txt(parent_dir, output_dir):
    """
    Recursively finds .c, .cpp, and .h files in the given parent directory,
    converts their content to .txt, and saves them in the output directory.

    Args:
        parent_dir (str): Path to the parent directory containing source files.
        output_dir (str): Path to the output directory for the .txt files.
    """
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    for root, _, files in os.walk(parent_dir):
        for file in files:
            if file.endswith(('.c', '.cpp', '.h')):
                # Full path of the current file
                source_file_path = os.path.join(root, file)
                
                # Create the corresponding output path
                relative_path = os.path.relpath(root, parent_dir)
                target_dir = os.path.join(output_dir, relative_path)
                os.makedirs(target_dir, exist_ok=True)
                
                # Target .txt file path
                target_file_path = os.path.join(target_dir, file + '.txt')
                
                # Read the source file and write to the target .txt file
                try:
                    with open(source_file_path, 'r', encoding='utf-8') as src_file:
                        content = src_file.read()
                    
                    with open(target_file_path, 'w', encoding='utf-8') as tgt_file:
                        tgt_file.write(content)
                    
                    print(f"Converted: {source_file_path} -> {target_file_path}")
                except Exception as e:
                    print(f"Error processing {source_file_path}: {e}")

# Example usage
parent_directory = "."  # Replace with your directory
output_directory = "./bin/txt_version/"  # Replace with your directory

convert_source_to_txt(parent_directory, output_directory)
