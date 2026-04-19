from internal_core.generators.generate_configure_files import (
    generate_statically_resolved_plugin_providers_json,
    generate_plugin_manager_cmake,
)
from internal_core.datatypes import *
from internal_core.parsers.plugin_manager_parse import *
from internal_core.compile_time_plugin_resolver import resolve_compile_time_plugins, sort_plugin_providers

# from plugin_manager_plugin_resolver import (
#     resolve_requested_plugin_providers,
#     check_resolved_requested_plugin_providers,
#     sort_plugin_providers,
# )

import argparse


@dataclass
class GenerateCmakeArguments:
    target_name: str
    build_platform: str
    build_dynamic_plugins: bool

    plugin_registry_toml: Path
    app_toml: Path

    source_cmake: Path

    generated_include_dir: Path
    generated_plugin_manager_bootloader_generated_src: Path
    generated_cmake: Path
    generated_statically_resolved_plugins_json: Path

    @classmethod
    def from_args(cls, args: argparse.Namespace) -> "GenerateCmakeArguments":
        return cls(
            target_name=args.target_name,
            build_platform=args.build_platform,
            build_dynamic_plugins=args.build_dynamic_plugins,
            #
            plugin_registry_toml=args.plugin_registry_toml_path,
            app_toml=args.app_toml_path,
            #
            source_cmake=args.source_cmake_path,
            #
            generated_include_dir=args.generated_include_dir_path,
            generated_plugin_manager_bootloader_generated_src=args.generated_plugin_manager_bootloader_generated_src_path,
            generated_cmake=args.generated_cmake_path,
            generated_statically_resolved_plugins_json=args.generated_statically_resolved_plugins_json_path,
        )


def parse_cmake_arguments() -> GenerateCmakeArguments:
    parser = argparse.ArgumentParser(description="Generate cmake files")

    parser.add_argument(
        "--target-name",
        required=True,
    )
    parser.add_argument(
        "--build-platform",
        required=True,
    )
    parser.add_argument(
        "--build-dynamic-plugins",
        action="store_true",
    )

    parser.add_argument(
        "--plugin-registry-toml-path",
        required=True,
        type=Path,
    )
    parser.add_argument(
        "--app-toml-path",
        required=True,
        type=Path,
    )

    parser.add_argument(
        "--source-cmake-path",
        required=True,
        type=Path,
    )

    parser.add_argument(
        "--generated-include-dir-path",
        required=True,
        type=Path,
    )
    parser.add_argument(
        "--generated-plugin-manager-bootloader-generated-src-path",
        required=True,
        type=Path,
    )
    parser.add_argument(
        "--generated-cmake-path",
        required=True,
        type=Path,
    )
    parser.add_argument(
        "--generated-statically-resolved-plugins-json-path",
        required=True,
        type=Path,
    )

    args = parser.parse_args()

    return GenerateCmakeArguments.from_args(args)





# TODO: Look into static and dynamic loaded at same time
def main():
    arguments = parse_cmake_arguments()

    plugin_registry_dict = read_toml(arguments.plugin_registry_toml)

    plugin_registry = parse_plugin_registry(
        plugin_registry_dict, arguments.build_platform
    )

    # plugin_providers = create_static_plugin_providers(
    #     plugin_registry, arguments.build_dynamic_plugins
    # )

    if not arguments.build_dynamic_plugins:
        app_dict = read_toml(arguments.app_toml)
        app_config = parse_app_dict(app_dict)

        plugin_manifests = resolve_compile_time_plugins(app_config, plugin_registry)
        print([pm.interface_name for pm in plugin_manifests])

        plugin_manifests = sort_plugin_providers(plugin_manifests)
        print([pm.interface_name for pm in plugin_manifests])



       

    #     # Make sure the same plugin is not requested more than once as this is not supported

    #     requested_plugin_providers: list[PluginProvider] = []
    #     for _ in range(RECURSIVE_DEPENDENCY_SOLVER_MAX_DEPTH):
    #         if not requested_plugins:
    #             break
    #         new_requested_plugin_providers = resolve_requested_plugin_providers(
    #             plugin_providers, requested_plugins
    #         )

    #         requested_plugins = check_resolved_requested_plugin_providers(
    #             new_requested_plugin_providers,
    #             requested_plugin_providers,
    #             requested_plugins,
    #         )

    #         requested_plugin_providers.extend(new_requested_plugin_providers)
    #     else:
    #         print(
    #             f"Hit maximum recursive dependency solver depth, maximum ({RECURSIVE_DEPENDENCY_SOLVER_MAX_DEPTH})"
    #         )
    #         return -1

    #     plugin_providers = requested_plugin_providers
    plugin_providers = []

    generate_statically_resolved_plugin_providers_json(
        arguments.generated_statically_resolved_plugins_json, plugin_providers
    )

    print(f"providers: {plugin_providers}")
    generate_plugin_manager_cmake(
        arguments.source_cmake,
        arguments.generated_cmake,
        arguments.target_name,
        [
            arguments.generated_include_dir,
        ],
        [
            arguments.generated_plugin_manager_bootloader_generated_src,
        ],
        plugin_providers,
    )


if __name__ == "__main__":
    main()
