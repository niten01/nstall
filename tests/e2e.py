import os
import subprocess
import sys
from pathlib import Path
import tempfile


def create_big_dir(name):
    big_dir = Path(name)
    for i in range(100):
        os.mkdir(big_dir / str(i))
        for j in range(1000):
            with open(big_dir / str(i) / str(j), 'w') as f:
                f.seek(512 * 1024)
                f.write("\0")
    return big_dir


if __name__ == "__main__":
    if len(sys.argv) != 3 and len(sys.argv) != 2:
        print("Usage test.py <path/to/nstall-constructor> [dir/to/pack]")
        exit(1)

    print("Got args: ", sys.argv)
    repo_root = Path(__file__).parent.parent
    constructor_path = sys.argv[1]
    with tempfile.TemporaryDirectory() as big_dir :
        dir_to_pack = sys.argv[2] if len(
            sys.argv) == 3 else create_big_dir(big_dir)
        files_to_pack = sum([len(files) for _, _, files in os.walk(dir_to_pack)])
        print("Files to pack: ", files_to_pack)
        try:
            subprocess.run([constructor_path, "-n", "a", "-d", dir_to_pack],
                           check=True)
            with tempfile.TemporaryDirectory() as tmp_dir:
                print("Installing to: ", tmp_dir)
                subprocess.run(["./a_Installer", tmp_dir], check=True)
                extracted_files = sum([len(files)
                                      for _, _, files in os.walk(tmp_dir)])
                print("Extracted files: ", extracted_files)
                assert extracted_files == files_to_pack
        except subprocess.CalledProcessError as e:
            print("Test failed. Output: ")
            print(e)
            exit(1)
        print("Test passed")
