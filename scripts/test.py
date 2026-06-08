#!/usr/bin/env python3

import subprocess
import sys
import os
import builtins
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
MODE = os.environ.get("MODE", "release")
TARGET_DIR = PROJECT_ROOT / "target" / MODE / "webserv"

TEST_TARGETS = {
    "webserv_test": "test",
    "toml98_test": "toml98-test",
    "mon_cgi_test": "mon-cgi-test",
    "mon_http_test": "mon-http-test",
    "mon_net_test": "mon-net-test",
    "mon_router_test": "mon-router-test",
}

TEST_BINARIES = {
    "webserv_test": PROJECT_ROOT / "tests" / "webserv_test",
    "toml98_test": PROJECT_ROOT / "libs" / "toml98" / "tests" / "toml98_test",
    "mon_cgi_test": PROJECT_ROOT / "libs" / "mon-cgi" / "tests" / "mon_cgi_test",
    "mon_http_test": PROJECT_ROOT / "libs" / "mon-http" / "tests" / "mon_http_test",
    "mon_net_test": PROJECT_ROOT / "libs" / "mon-net" / "tests" / "mon_net_test",
    "mon_router_test": PROJECT_ROOT / "libs" / "mon-router" / "tests" / "mon_router_test",
}

def print(*args, **kwargs):
    kwargs.setdefault("flush", True)
    return builtins.print(*args, **kwargs)


def build_tests():
    print("==> Building test binaries...")
    for name, target in TEST_TARGETS.items():
        print(f"  [{name}] ", end="")
        r = subprocess.run(
            ["make", target, f"MODE={MODE}", f"TARGET_DIR={TARGET_DIR}"],
            cwd=PROJECT_ROOT, capture_output=True, timeout=120,
        )
        print("\033[32mOK\033[0m" if r.returncode == 0 else "\033[31mFAILED\033[0m")
        if r.returncode != 0:
            sys.stderr.buffer.write(r.stderr)
            sys.stdout.buffer.write(r.stdout)
            sys.exit(r.returncode)


def run_tests():
    print("==> Running test binaries...")
    for name, binary in TEST_BINARIES.items():
        if not binary.exists():
            print(f"  [{name}] \033[31mMISSING\033[0m")
            return 1
        print(f"  [{name}] ", end="")
        r = subprocess.run([str(binary), "--verbose"], cwd=PROJECT_ROOT)
        print("\033[32mOK\033[0m" if r.returncode == 0 else "\033[31mFAILED\033[0m")
        if r.returncode != 0:
            return r.returncode
    return 0


def main():
    build_tests()
    sys.exit(run_tests())


if __name__ == "__main__":
    main()
