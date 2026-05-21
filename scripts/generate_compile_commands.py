#!/usr/bin/env python3
"""Generate compile_commands.json from Makefile variables without invoking make."""
import argparse
import json
import os


def _stem(path: str, root: str) -> str:
    """Return the relative path without extension, like Make's ``$*``."""
    p = os.path.normpath(path)
    r = os.path.normpath(root) if root else "."
    if p.startswith(r + "/"):
        p = p[len(r) + 1:]
    elif p == r:
        p = ""
    return os.path.splitext(p)[0]


def main() -> None:
    ap = argparse.ArgumentParser(
        description="Generate a Clang compilation database from Makefile variables. "
                    "Flags whose values start with a dash must use --flag=value syntax."
    )
    ap.add_argument("--output", "-o", default="compile_commands.json")
    ap.add_argument("--directory", default=os.getcwd())
    ap.add_argument("--compiler", default="c++")
    ap.add_argument("--cflags", action="append", default=[])
    ap.add_argument("--cppflags", action="append", default=[])
    ap.add_argument("--inc", action="append", default=[])
    ap.add_argument("--src", action="append", default=[])
    ap.add_argument("--obj-dir")
    ap.add_argument("--src-dir", default=".")
    ap.add_argument("--dep-dir")
    args = ap.parse_args()

    cwd = os.path.abspath(args.directory)
    src_dir = os.path.normpath(args.src_dir)
    obj_dir = os.path.normpath(args.obj_dir) if args.obj_dir else None
    dep_dir = os.path.normpath(args.dep_dir) if args.dep_dir else None

    flags: list[str] = []
    flags.extend(args.cflags)
    flags.extend(args.cppflags)
    for i in args.inc:
        i = i.strip()
        if i:
            flags.append(i if i.startswith("-I") else f"-I{i}")

    entries: list[dict[str, str | list[str]]] = []

    for src in args.src:
        src_norm = os.path.normpath(src)
        src_abs = os.path.abspath(os.path.join(cwd, src_norm))
        args_list: list[str] = [args.compiler]
        args_list.extend(flags)
        args_list.extend(["-c", src_norm])

        if obj_dir:
            obj = os.path.normpath(os.path.join(obj_dir, _stem(src_norm, src_dir) + ".o"))
            args_list.extend(["-o", obj])
            obj_abs = os.path.abspath(os.path.join(cwd, obj))
        else:
            obj_abs = None

        if dep_dir:
            dep = os.path.normpath(os.path.join(dep_dir, _stem(src_norm, src_dir) + ".d"))
            args_list.extend(["-MF", dep])

        entry: dict[str, str | list[str]] = {
            "arguments": args_list,
            "directory": cwd,
            "file": src_abs,
        }
        if obj_abs:
            entry["output"] = obj_abs
        entries.append(entry)

    with open(args.output, "w") as f:
        json.dump(entries, f, indent=2, sort_keys=False)
        f.write("\n")


if __name__ == "__main__":
    main()
