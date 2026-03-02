import json
import re
import argparse
from pathlib import Path

from plugin_manager_generate import (
    generate_plugin_registry_src,
    generate_plugin_registry_header,
    generate_plugin_registry_cmake,
)


def read_file(source_path: Path):
    print(f"-- Reading: {source_path}")
    with open(source_path, "r") as f:
        return json.loads(f.read())


def parse_plugin_registry(
    plugin_registry_json, build_platform: str
) -> tuple[str, int, int]:
    interface_definitions_array_text = ""
    plugin_definitions_max_len = 0

    interface_definitions = plugin_registry_json.get("interfaces", [])
    for interface_definition in interface_definitions:
        interface_name = interface_definition.get("interface_name")

        if "default_plugin" not in interface_definition:
            raise ValueError(
                f"Failed to parse default plugin for interface '{interface_name}' in registry"
            )

        default_plugin_obj = interface_definition["default_plugin"]
        default_plugin_name = ""

        if isinstance(default_plugin_obj, str):
            default_plugin_name = default_plugin_obj
        elif isinstance(default_plugin_obj, list):
            if len(default_plugin_obj) == 0:
                raise ValueError(
                    f"No default plugin found for interface '{interface_name}' in registry"
                )

            for plugin_choice in default_plugin_obj:
                target = plugin_choice.get("target")

                if target == "win32" and build_platform == "win32":
                    default_plugin_name = plugin_choice.get("plugin_name", "")
                    break

                # TODO Add other platforms

        plugin_definition_array_text = ""
        plugin_definitions = interface_definition.get("plugins", [])
        plugin_definitions_len = len(plugin_definitions)
        if plugin_definitions_len > plugin_definitions_max_len:
            plugin_definitions_max_len = plugin_definitions_len

        for plugin_definition in plugin_definitions:
            plugin_name = plugin_definition.get("plugin_name")
            plugin_module_path = plugin_definition.get("module_path")
            plugin_source_path = plugin_definition.get("source_path")
            plugin_definition_array_text += f"""
                    {{
                        .plugin_name = "{plugin_name}",
                        .module_path = "{plugin_module_path}",
                    }},"""

        interface_definitions_array_text += f"""
            {{
                .interface_name = "{interface_name}",
                .default_plugin = "{default_plugin_name}",
                .plugin_definitions = {{{plugin_definition_array_text}
                }},
                .plugin_definitions_len = {plugin_definitions_len},
            }},"""

    return (
        interface_definitions_array_text,
        len(interface_definitions),
        plugin_definitions_max_len,
    )


# TODO: Add error handling
def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("--target-name", required=True)
    parser.add_argument("--build-platform", required=True)

    parser.add_argument("--plugin-registry-json-path", required=True)

    parser.add_argument("--source-plugin-registry-src-path", required=True)

    parser.add_argument("--generated-plugin-registry-header-path", required=True)
    parser.add_argument("--generated-plugin-registry-src-path", required=True)
    parser.add_argument("--generated-cmake-path", required=True)

    args = parser.parse_args()

    plugin_registry_json = read_file(args.plugin_registry_json_path)
    (
        interface_definitions_array_text,
        interface_definitions_len,
        plugin_definitions_max_len,
    ) = parse_plugin_registry(plugin_registry_json, args.build_platform)

    generate_plugin_registry_src(
        Path(args.source_plugin_registry_src_path),
        Path(args.generated_plugin_registry_src_path),
        interface_definitions_array_text,
        interface_definitions_len,
    )

    generate_plugin_registry_header(
        Path(args.generated_plugin_registry_header_path),
        interface_definitions_len,
        plugin_definitions_max_len,
    )

    generate_plugin_registry_cmake(
        Path(args.generated_cmake_path),
        args.target_name,
        args.generated_plugin_registry_src_path,
    )


if __name__ == "__main__":
    main()
