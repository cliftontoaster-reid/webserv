import sys
import xml.etree.ElementTree as ET


def junit_escape(s: str) -> str:
    return (
        s.replace("&", "&amp;")
        .replace('"', "&quot;")
        .replace("<", "&lt;")
        .replace(">", "&gt;")
    )


for path in sys.argv[1:]:
    tree = ET.parse(path)
    suites = tree.getroot()

    out = ['<?xml version="1.0" encoding="UTF-8"?>']

    ts_attrs = []
    for k in ("tests", "failures", "errors", "disabled", "time"):
        v = suites.get(k, "")
        if v:
            ts_attrs.append(f'{k}="{junit_escape(v)}"')
    out.append(f'<testsuites {" ".join(ts_attrs)}>')

    for suite in suites.findall("testsuite"):
        sa = []
        for k in ("name", "tests", "failures", "errors", "disabled", "skipped", "time"):
            v = suite.get(k, "")
            if v:
                sa.append(f'{k}="{junit_escape(v)}"')
        out.append(f'  <testsuite {" ".join(sa)}>')

        for tc in suite.findall("testcase"):
            ca = []
            ca.append(f'classname="{junit_escape(suite.get("name", ""))}"')
            for k in ("name", "time"):
                v = tc.get(k, "")
                if v:
                    ca.append(f'{k}="{junit_escape(v)}"')
            out.append(f'    <testcase {" ".join(ca)}/>')

        out.append("  </testsuite>")

    out.append("</testsuites>")
    out.append("")

    with open(path, "w") as f:
        f.write("\n".join(out))
