import os
import subprocess
import sys
from pathlib import Path

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage test.py <path/to/nstall-constructor>")
        exit(1)

    repo_root = Path(__file__).parent.parent
    constructor_path = sys.argv[1]
    try:
        subprocess.run([constructor_path, "-n","a", "-d", repo_root / "src"], check=True)
        subprocess.run(["./a_Installer"], check=True)
    except subprocess.CalledProcessError: 
        print("Test failed")
        exit(1)
    print("Test passed")
