"""This assumes LVC was built with CONFIRM_VERSION_INPUT = false"""
"""WARNING: THIS TEST DELETES .git AND .lvc"""
import os
import random
import shutil
import subprocess
from pathlib import Path
from time import perf_counter

def prefix_hello_world(paths):
    for path in paths:
        original = path.read_bytes()
        path.write_bytes(b"Hello world\n" + original)

def pick_random_files(directory=".", count=40, limit=1000):
    files = []
    for root, dirs, names in os.walk(directory):
        dirs[:] = [name for name in dirs]
        for name in names:
            path = Path(root) / name
            if not path.is_symlink() and path.is_file():
                files.append(path)
            if len(files) == limit:
                return random.sample(files, count)

    if len(files) < count:
        raise ValueError(f"Need {count} files, found {len(files)}")

    return random.sample(files, count)

def remove_if_present(path):
    try:
        shutil.rmtree(path)
    except FileNotFoundError:
        pass

quiet = {"stdout": subprocess.DEVNULL, "stderr": subprocess.DEVNULL}

remove_if_present(".git")
remove_if_present(".lvc")

files = pick_random_files()

subprocess.run(["git", "init"], check=True, **quiet)
subprocess.run(["lvc", "create"], input="L\n\nbenchmark\nmain\n", text=True, check=True, **quiet)

start = perf_counter()
subprocess.run(["git", "add", "."], check=True, **quiet)
subprocess.run(["git", "commit", "-m", "time measure test"], check=True, **quiet)
print(f"git cold: {(perf_counter() - start) * 1000:.3f} ms")

prefix_hello_world(files[0:20])

start = perf_counter()
subprocess.run(["git", "add", "."], check=True, **quiet)
subprocess.run(["git", "commit", "-m", "time measure test"], check=True, **quiet)
print(f"git 20 changed files: {(perf_counter() - start) * 1000:.3f} ms")

shutil.rmtree(".git")

start = perf_counter()
subprocess.run(["lvc", "version", "desc", "time measure test", "*"], check=True, **quiet)
print(f"lvc cold: {(perf_counter() - start) * 1000:.3f} ms")

prefix_hello_world(files[20:40])

start = perf_counter()
subprocess.run(["lvc", "version", "desc", "time measure test", "*"], check=True, **quiet)
print(f"lvc 20 changed files: {(perf_counter() - start) * 1000:.3f} ms")