from pathlib import Path
from plugin_source_data import PluginSourceData


def create_and_write_to_file(destination_path: Path, content: str):
    print(f"-- Creating and writing to: {destination_path}")
    destination_path.parent.mkdir(exist_ok=True, parents=True)
    with open(destination_path, "w") as f:
        f.write(content)


def generate_plugin_manager_header(destination_path: Path, forward_declarations: str):
    create_and_write_to_file(
        destination_path,
        f"""#pragma once

#include <stdint.h>

{forward_declarations}""",
    )


def generate_plugin_manager_cmake(
    destination_path: Path,
    target_name: str,
    generated_get_setup_context_src_path: str,
    plugin_source_data_list: list[PluginSourceData],
):
    add_subdirectory_text = "\n".join(
        f"""if(NOT TARGET {source_data.target_name})
    add_subdirectory(
        "{source_data.source_path}"
        "${{CMAKE_CURRENT_BINARY_DIR}}/generated_plugins/{source_data.target_name}"
    )
endif()"""
        for source_data in plugin_source_data_list
    )

    plugin_target_cmake_list = ";".join(
        source_data.target_name for source_data in plugin_source_data_list
    )

    create_and_write_to_file(
        destination_path,
        f"""include_guard(GLOBAL)

{add_subdirectory_text}

message(STATUS "Linking: {plugin_target_cmake_list}")
target_link_libraries({target_name}
    PRIVATE
        {plugin_target_cmake_list}
)

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    {'\n\t'.join(f'"{source_data.register_path}"' for source_data in plugin_source_data_list)}
)

target_sources({target_name}
    PRIVATE
        "{generated_get_setup_context_src_path}"
)
""",
    )


def generate_get_setup_context_src(
    source_path: Path,
    destination_path: Path,
    internal_plugin_text_list: list[str],
    requested_plugin_text_list: list[str],
    sorted_plugin_providers_indices: list[int],
    plugin_provider_text_list: list[str],
):
    def format_c_array(list_to_format: list[str]) -> str:
        if len(list_to_format) == 0:
            return "0"
        return "\n".join(list_to_format)

    generated_file = ""
    print(f"-- Reading: {source_path}")
    with open(source_path, "r") as f:
        generated_file = f.read()

    replacements = {
        "@INTERNAL_PLUGINS_TEXT@": format_c_array(internal_plugin_text_list),
        "@INTERNAL_PLUGINS_LEN@": str(len(internal_plugin_text_list)),
        "@REQUESTED_PLUGINS_TEXT@": format_c_array(requested_plugin_text_list),
        "@REQUESTED_PLUGINS_LEN@": str(len(requested_plugin_text_list)),
        "@SORTED_PLUGIN_PROVIDERS_INDICES_TEXT@": format_c_array(
            [str(index) for index in sorted_plugin_providers_indices]
        ),
        "@PLUGIN_PROVIDERS_TEXT@": format_c_array(plugin_provider_text_list),
        "@PLUGIN_PROVIDERS_LEN@": str(len(plugin_provider_text_list)),
    }

    for placeholder, actual_value in replacements.items():
        generated_file = generated_file.replace(placeholder, actual_value)

    create_and_write_to_file(
        destination_path,
        generated_file,
    )


def generate_plugin_registry_src(
    source_path: Path,
    destination_path: Path,
    interface_definitions_array_text: str,
    interface_definitions_len: int,
):
    generated_file = ""
    print(f"-- Reading: {source_path}")
    with open(source_path, "r") as f:
        generated_file = f.read()

    replacements = {
        "@INTERFACE_DEFINITIONS_TEXT@": interface_definitions_array_text,
        "@INTERFACE_DEFINITIONS_LEN@": str(interface_definitions_len),
    }

    for placeholder, actual_value in replacements.items():
        generated_file = generated_file.replace(placeholder, actual_value)

    create_and_write_to_file(
        destination_path,
        generated_file,
    )

def generate_plugin_registry_header(
    destination_path: Path,
    interface_definitions_len: int,
    plugin_definitions_max_len: int,
):
    create_and_write_to_file(
        destination_path,
        f"""#pragma once

#include <stddef.h>

typedef struct PluginDefinition
{{
    const char *plugin_name;
    const char *module_path;
}} PluginDefinition;

typedef struct InterfaceDefinition
{{
    const char *interface_name;
    const char *default_plugin;
    PluginDefinition plugin_definitions[{str(plugin_definitions_max_len)}];
    size_t plugin_definitions_len;
}} InterfaceDefinition;

typedef struct PluginRegistry
{{
    InterfaceDefinition interface_definitions[{str(interface_definitions_len)}];
    size_t interface_definitions_len;
}} PluginRegistry;

const PluginRegistry *plugin_registry_get(void);
"""
    )

def generate_plugin_registry_cmake(
    destination_path: Path,
    target_name: str,
    generated_plugin_registry_src_path: str,
):
    create_and_write_to_file(
        destination_path,
        f"""include_guard(GLOBAL)

target_sources({target_name}
    PRIVATE
        "{generated_plugin_registry_src_path}"
)
"""
    )