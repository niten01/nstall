import subprocess
import sys
from pathlib import Path
import tempfile

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage test.py <path/to/nstall-constructor> <dir/to/pack>")
        exit(1)

    print("Got args: ", sys.argv)
    repo_root = Path(__file__).parent.parent
    constructor_path = sys.argv[1]
    try:
        subprocess.run([constructor_path, "-n", "a", "-d", repo_root/"src"],
                       check=True)
        with tempfile.TemporaryDirectory() as tmp_dir:
            print("Installing to: ", tmp_dir)
            subprocess.run(["./a_Installer", tmp_dir], check=True)
    except subprocess.CalledProcessError as e:
        print("Test failed. Output: ")
        print(e)
        exit(1)
    print("Test passed")
