"""Apply a scoped Metrowerks section order to the normal DOL link."""

from collections import Counter
import json
from pathlib import Path
import re
import shutil
import subprocess
import sys
from tempfile import TemporaryDirectory


def order_section(script, section, orders, objects, link_objects):
    anchors = {}
    for unit, sections in orders.items():
        anchor, = (index for index, path in enumerate(objects) if path.as_posix().endswith("/" + unit))
        anchors[anchor] = sections

    selectors = []
    for index, path in enumerate(link_objects[:max(anchors) + 1]):
        selectors.extend(f'            "{path.name}" ({name})' for name in anchors.get(index, [section]))
    selectors.append(f"            * ({section})")
    body = "\n".join(selectors)
    declaration = rf"(?m)^(        {re.escape(section)} ALIGN\(0x[0-9a-fA-F]+\):)\{{\}}$"
    updated, count = re.subn(declaration, lambda match: f"{match[1]}{{\n{body}\n        }}", script)
    if count != 1:
        raise ValueError(f"expected the default {section} declaration in the DOL linker script")
    return updated


def apply_layout(base_script, layout, objects, link_objects=None):
    if link_objects is None:
        link_objects = objects
    script = order_section(base_script, ".text", {layout["unit"]: layout["code_sections"]}, objects, link_objects)
    if data_sections := layout.get("data_sections"):
        script = order_section(script, ".data", data_sections, objects, link_objects)

    aliases = "".join(f"    {name} = {target};\n" for name, target in layout["aliases"].items())
    marker = "    _stack_end = "
    if script.count(marker) != 1:
        raise ValueError("expected the default stack declaration in the DOL linker script")
    return script.replace(marker, aliases + marker, 1)


def distinct_objects(objects, directory):
    """Give duplicate basenames unique link inputs for LCF filename selectors."""
    counts = Counter(path.name.casefold() for path in objects)
    basenames = set(counts)
    for index, path in enumerate(objects):
        if counts[path.name.casefold()] < 2:
            continue
        alias = f"link-{index}-{path.name}"
        if alias.casefold() in basenames:
            raise ValueError(f"duplicate link input name {alias}")
        basenames.add(alias.casefold())
        copy = directory / alias
        shutil.copyfile(path, copy)
        objects[index] = copy


def main():
    layout_path, script_path, response_path = map(Path, sys.argv[1:4])
    if sys.argv[4] != "--" or len(sys.argv) < 6:
        raise ValueError("expected '--' followed by the Metrowerks linker command")
    layout = json.loads(layout_path.read_text(encoding="utf-8"))
    original = script_path.read_text(encoding="utf-8")
    objects = [Path(line.strip('"\'')) for line in response_path.read_text(encoding="utf-8").splitlines() if line]

    with TemporaryDirectory(prefix="link-", dir=response_path.parent) as folder:
        directory = Path(folder)
        link_objects = objects.copy()
        distinct_objects(link_objects, directory)
        script = directory / "main.lcf"
        script.write_text(apply_layout(original, layout, objects, link_objects), encoding="utf-8")
        response = directory / "main.rsp"
        response.write_text("".join(f'"{path}"\n' for path in link_objects), encoding="utf-8")
        return subprocess.run([*sys.argv[5:], "-lcf", str(script), f"@{response}"]).returncode


if __name__ == "__main__":
    sys.exit(main())
