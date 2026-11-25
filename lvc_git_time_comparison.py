"""This assumes LVC was built with CONFIRM_VERSION_INPUT = false"""
"""WARNING: THIS TEST DELETES .git AND .lvc"""
import subprocess
import shutil
from time import perf_counter

def remove_if_present(path):
    try:
        shutil.rmtree(path)
    except FileNotFoundError:
        pass

quiet = {"stdout": subprocess.DEVNULL, "stderr": subprocess.DEVNULL}

remove_if_present(".git")
remove_if_present(".lvc")

subprocess.run(["git", "init"], check=True, **quiet)
subprocess.run(["lvc", "create"], input="L\n\nbenchmark\nmain\n", text=True, check=True, **quiet)

start = perf_counter()
subprocess.run(["git", "add", "."], check=True, **quiet)
subprocess.run(["git", "commit", "-m", "time measure test"], check=True, **quiet)
print(f"git: {(perf_counter() - start) * 1000:.3f} ms")

shutil.rmtree(".git")

start = perf_counter()
subprocess.run(["lvc", "version", "desc", "time measure test", "*"], check=True, **quiet)
print(f"lvc: {(perf_counter() - start) * 1000:.3f} ms")