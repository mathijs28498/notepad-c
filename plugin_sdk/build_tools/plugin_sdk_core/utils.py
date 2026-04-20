from pathlib import Path
import sys

import json

# TODO: Check if this properly works for older versions
if sys.version_info >= (3, 11):
    import tomllib
else:
    import importlib.util

    if importlib.util.find_spec("tomli") is None:
        print(
            "Error: Your Python version is older than 3.11.\n"
            "To parse plugin manifests, you must either:\n"
            "  1. Upgrade to Python 3.11 or newer (Recommended)\n"
            "  2. Install the fallback library by running: pip install tomli",
            file=sys.stderr,
        )
        sys.exit(1)

    import tomli as tomllib

indent_prefix = "    "

def snake_to_pascal_case(snake_str: str):
    return "".join(x.capitalize() for x in snake_str.lower().split("_"))

def read_json(source_path: Path):
    print(f"-- Reading: {source_path}")
    with open(source_path, "r", encoding="utf-8") as f:
        return json.load(f)


def read_toml(source_path: Path):
    print(f"-- Reading: {source_path}")
    with open(source_path, "rb") as f:
        return tomllib.load(f)


def create_and_write_to_file(
    destination_path: Path,
    content: str,
    skip_unchanged_file: bool,
):
    if skip_unchanged_file and destination_path.exists():
        with open(destination_path, "r", encoding="utf-8") as f:
            if f.read() == content:
                print(
                    f"-- Skip writing as new content is the same as old: {destination_path}"
                )
                return
    print(f"-- Creating and writing to: {destination_path}")
    destination_path.parent.mkdir(exist_ok=True, parents=True)

    with open(destination_path, "w", encoding="utf-8") as f:
        f.write(content)


def configure_file(
    source_path: Path,
    destination_path: Path,
    replacements: list[tuple[str, str]],
    skip_unchanged_file: bool,
):
    print(f"-- Reading: {source_path}")
    with open(source_path, "r", encoding="utf-8") as f:
        generated_file = f.read()

    for placeholder, actual_value in replacements:
        generated_file = generated_file.replace(f"@{placeholder}@", actual_value)

    create_and_write_to_file(
        destination_path,
        generated_file,
        skip_unchanged_file,
    )
