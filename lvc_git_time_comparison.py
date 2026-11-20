"""This assumes LVC was built with CONFIRM_VERSION_INPUT = false"""
import subprocess
from time import perf_counter

subprocess.run(["git", "init"], check=True)
subprocess.run(["lvc", "create"], input="L\n\benchmark\nmain\n", check=True)

start = perf_counter()
subprocess.run(["git", "add", "."], check=True)
subprocess.run(["git", "commit", "-m", "time measure test"], check=True)
print(f"git: {(perf_counter() - start) * 1000:.3f} ms")

start = perf_counter()
subprocess.run(["lvc", "version", "desc", "time measure test", "*"], check=True)
print(f"lvc: {(perf_counter() - start) * 1000:.3f} ms")