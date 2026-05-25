#!/usr/bin/env python3

import re
import subprocess
import sys
import os
import shutil
import builtins
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
MODE = "release"
CTEST_DIR = PROJECT_ROOT / "target" / "ctest"
BIN_DIR = CTEST_DIR / "bin"

TESTS = {
    "webserv_test": ("test", PROJECT_ROOT / "tests" / "webserv_test"),
    "toml98_test": ("toml98-test", PROJECT_ROOT / "libs" / "toml98" / "tests" / "toml98_test"),
    "mon_cgi_test": ("mon-cgi-test", PROJECT_ROOT / "libs" / "mon-cgi" / "tests" / "mon_cgi_test"),
    "mon_http_test": ("mon-http-test", PROJECT_ROOT / "libs" / "mon-http" / "tests" / "mon_http_test"),
    "mon_net_test": ("mon-net-test", PROJECT_ROOT / "libs" / "mon-net" / "tests" / "mon_net_test"),
    "mon_router_test": ("mon-router-test", PROJECT_ROOT / "libs" / "mon-router" / "tests" / "mon_router_test"),
}

CRITERION_LIB = PROJECT_ROOT / "target" / MODE / "criterion" / "bin" / "lib"


def print(*args, **kwargs):
    kwargs.setdefault("flush", True)
    return builtins.print(*args, **kwargs)


def build_tests():
    print("==> Building test binaries...")
    for name, (target, path) in TESTS.items():
        print(f"  [{name}] ", end="")
        r = subprocess.run(
            ["make", target, f"MODE={MODE}", "--silent"],
            cwd=PROJECT_ROOT, capture_output=True, timeout=120,
        )
        print("\033[32mOK\033[0m" if r.returncode == 0 else "\033[31mFAILED\033[0m")


def list_tests(binary: Path, lib_dir: Path) -> list[str]:
    env = {**os.environ, "LD_LIBRARY_PATH": str(lib_dir)}
    r = subprocess.run([str(binary), "--list"], capture_output=True, text=True, timeout=30, env=env)
    if r.returncode != 0:
        print(f"    ERROR: {r.stderr.strip()}")
        return []
    tests = []
    suite = None
    for line in r.stdout.splitlines():
        m = re.match(r"^(.+?): \d+ tests?$", line)
        if m:
            suite = m.group(1)
            continue
        m = re.match(r"^[├└]── (.+)$", line)
        if m and suite:
            tests.append(f"{suite}:{m.group(1)}")
    return tests


def setup_ctest():
    print("==> Setting up CTest...")
    if CTEST_DIR.exists():
        shutil.rmtree(CTEST_DIR)
    BIN_DIR.mkdir(parents=True)

    # Copy binaries and libcriterion
    print("==> Copying test binaries...")
    for name, (_, src) in TESTS.items():
        if src.exists():
            shutil.copy2(src, BIN_DIR / name)
            print(f"  + {name}")

    if CRITERION_LIB.exists():
        for f in CRITERION_LIB.iterdir():
            if f.is_dir():
                continue
            dest = BIN_DIR / f.name
            if f.is_symlink():
                target = f.readlink()
                dest.symlink_to(target)
            else:
                shutil.copy2(f, dest)
        print("  + libcriterion.so (with symlinks)")

    return BIN_DIR


def discover_individual_tests(bin_dir: Path) -> dict[str, list[str]]:
    print("==> Discovering individual tests...")
    all_tests = {}
    for name in TESTS:
        binary = bin_dir / name
        if not binary.exists():
            continue
        tests = list_tests(binary, bin_dir)
        if tests:
            all_tests[name] = tests
            print(f"  + {name}: {len(tests)} tests")
        else:
            print(f"  ? {name}: no tests discovered")
    return all_tests


def write_cmakelists(bin_dir: Path, all_tests: dict[str, list[str]]):
    lines = [
        "cmake_minimum_required(VERSION 3.10)",
        "project(webserv-tests)",
        "enable_testing()",
        "",
    ]
    for binary_name, tests in all_tests.items():
        binary_path = bin_dir / binary_name
        for test_id in tests:
            safe_name = f"{binary_name}/{test_id}"
            lines.append(f'add_test(NAME "{safe_name}" COMMAND {binary_path} --filter {test_id})')
            lines.append(f'set_tests_properties("{safe_name}" PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH={bin_dir}")')
            lines.append("")

    (CTEST_DIR / "CMakeLists.txt").write_text("\n".join(lines) + "\n")


def run_ctest():
    print("==> Running ctest...")
    subprocess.run(
        ["cmake", "-S", str(CTEST_DIR), "-B", str(CTEST_DIR), "-Wno-dev"],
        capture_output=True,
    )
    r = subprocess.run(
        ["ctest", "--output-on-failure", "--test-dir", str(CTEST_DIR)] + sys.argv[1:],
    )
    return r.returncode


def main():
    build_tests()
    bin_dir = setup_ctest()
    all_tests = discover_individual_tests(bin_dir)
    if not all_tests:
        print("  ! No tests discovered.")
        sys.exit(1)
    write_cmakelists(bin_dir, all_tests)
    sys.exit(run_ctest())


if __name__ == "__main__":
    main()
