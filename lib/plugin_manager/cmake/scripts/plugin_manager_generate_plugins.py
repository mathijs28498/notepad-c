import json
import re
import argparse
from pathlib import Path

from plugin_manager_generate import (
    generate_plugin_manager_cmake,
    generate_plugin_manager_header,
    generate_get_setup_context_src,
)

from plugin_source_data import PluginSourceData


def read_file(source_path: Path):
    print(f"-- Reading: {source_path}")
    with open(source_path, "r") as f:
        return json.loads(f.read())


# TODO: Check if this file exists, otherwise give error
# TODO: Do this a better way than regex to get the methods that exist
def read_internal_plugin_register_source(
    source_path: Path, interface_name: str, plugin_name: str
) -> tuple[str, str]:
    forward_declarations = ""

    get_interface_val = "NULL"
    init_val = "NULL"
    shutdown_val = "NULL"

    with open(source_path, "r") as f:
        plugin_register_source = f.read()
        get_interface_match = re.search(
            r"^[ \t]*PLUGIN_REGISTER_INTERFACE", plugin_register_source, re.MULTILINE
        )
        if get_interface_match is None:
            # TODO Add correct error
            raise ValueError()

        function_name = f"{interface_name}_get_interface"
        get_interface_val = f"(PluginGetInterface_Fn){function_name}"

        forward_declarations += f"\nvoid *{function_name}(void);"

        init_match = re.search(
            r"^[ \t]*PLUGIN_REGISTER_INIT", plugin_register_source, re.MULTILINE
        )
        if init_match is not None:
            function_name = f"{interface_name}_init"
            init_val = f"(PluginInit_Fn){function_name}"
            forward_declarations += f"\nint32_t {function_name}(void *context);"

        shutdown_match = re.search(
            r"^[ \t]*PLUGIN_REGISTER_SHUTDOWN", plugin_register_source, re.MULTILINE
        )
        if shutdown_match is not None:
            function_name = f"{interface_name}_shutdown"
            shutdown_val = f"(PluginShutdown_Fn){function_name}"
            forward_declarations += f"\nint32_t {function_name}(void *context);"

        # TODO: Handle dependencies
        return (
            forward_declarations,
            f"""
            {{
                .interface_name = "{interface_name}",
                .plugin_name = "{plugin_name}",
                .dependencies_len = 0,
                .get_interface = {get_interface_val},
                .init = {init_val},
                .shutdown = {shutdown_val},
                .is_explicit = false,
                .is_initialized = false,
            }},""",
        )


def parse_internal_plugins(
    plugin_registry_json,
) -> tuple[str, list[PluginSourceData], list[str]]:
    header_forward_declarations = ""
    source_data_list: list[PluginSourceData] = []
    internal_plugin_text_list: list[str] = []

    for internal_plugin in plugin_registry_json.get("internal_plugins", []):
        interface_name: str = internal_plugin.get("interface_name")
        plugin_name: str = internal_plugin.get("plugin_name")
        plugin_source_path: str = internal_plugin.get("plugin_source_path")
        plugin_target_name: str = internal_plugin.get("cmake_target_name")

        header_forward_declarations += "\n".join(
            internal_plugin.get("plugin_forward_declarations", [])
        )

        plugin_register_path_str = (
            f"{plugin_source_path}/{interface_name}_{plugin_name}_register.c"
        )
        plugin_register_path = Path(plugin_register_path_str)

        source_data_list.append(
            PluginSourceData(
                target_name=plugin_target_name,
                source_path=plugin_source_path,
                register_path=plugin_register_path_str,
            )
        )

        forward_declarations, internal_plugin_text = (
            read_internal_plugin_register_source(
                plugin_register_path, interface_name, plugin_name
            )
        )

        header_forward_declarations += f"{forward_declarations}\n\n"
        internal_plugin_text_list.append(internal_plugin_text)

    return (
        header_forward_declarations,
        source_data_list,
        internal_plugin_text_list,
    )


def parse_plugin_list(plugin_list_json) -> tuple[list[str], list[int], list[str]]:
    requested_plugin_text_list: list[str] = []
    sorted_plugin_providers_indices: list[int] = []
    plugin_provider_text_list: list[str] = []
    # TODO: Add error handling
    for requested_plugin in plugin_list_json.get("plugins", []):
        interface_name: str = requested_plugin.get("interface_name")
        plugin_name: str = requested_plugin.get("plugin_name") or ""

        requested_plugin_text_list.append(
            f"""
        {{
            .interface_name = "{interface_name}",
            .plugin_name = "{plugin_name}",
            .is_explicit = true,
            .is_resolved= false,
        }},
"""
        )

    return (
        requested_plugin_text_list,
        sorted_plugin_providers_indices,
        plugin_provider_text_list,
    )


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("--target-name", required=True)

    parser.add_argument("--plugin-registry-json-path", required=True)
    parser.add_argument("--plugin-list-json-path", required=True)

    parser.add_argument("--source-get-setup-context-src-path", required=True)

    parser.add_argument("--generated-plugin-manager-header-path", required=True)
    parser.add_argument("--generated-get-setup-context-src-path", required=True)
    parser.add_argument("--generated-cmake-path", required=True)

    args = parser.parse_args()

    plugin_registry_json = read_file(args.plugin_registry_json_path)
    plugin_list_json = read_file(args.plugin_list_json_path)

    (
        plugin_manager_header_forward_declarations,
        internal_plugin_source_data_list,
        internal_plugin_text_list,
    ) = parse_internal_plugins(plugin_registry_json)

    (
        requested_plugin_text_list,
        sorted_plugin_providers_indices,
        plugin_provider_text_list,
    ) = parse_plugin_list(plugin_list_json)

    generate_plugin_manager_header(
        Path(args.generated_plugin_manager_header_path),
        plugin_manager_header_forward_declarations,
    )

    generate_get_setup_context_src(
        Path(args.source_get_setup_context_src_path),
        Path(args.generated_get_setup_context_src_path),
        internal_plugin_text_list,
        requested_plugin_text_list,
        sorted_plugin_providers_indices,
        plugin_provider_text_list,
    )

    generate_plugin_manager_cmake(
        Path(args.generated_cmake_path),
        args.target_name,
        args.generated_get_setup_context_src_path,
        internal_plugin_source_data_list,
    )


if __name__ == "__main__":
    main()
