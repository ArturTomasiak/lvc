"""This assumes LVC was built with CONFIRM_VERSION_INPUT = false"""
import subprocess
from time import perf_counter

quiet = {"stdout": subprocess.DEVNULL, "stderr": subprocess.DEVNULL}

subprocess.run(["git", "init"], check=True, **quiet)
subprocess.run(["lvc", "create"], input="L\n\nbenchmark\nmain\n", text=True, check=True, **quiet)

start = perf_counter()
subprocess.run(["git", "add", "."], check=True, **quiet)
subprocess.run(["git", "commit", "-m", "time measure test"], check=True, **quiet)
print(f"git: {(perf_counter() - start) * 1000:.3f} ms")

start = perf_counter()
subprocess.run(["lvc", "version", "desc", "time measure test", "*", "!/.git"], check=True, **quiet)
print(f"lvc: {(perf_counter() - start) * 1000:.3f} ms")