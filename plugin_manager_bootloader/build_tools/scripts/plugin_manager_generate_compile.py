from internal_core.generators.generate_compile_files import generate_plugin_manager_bootloader_generated_src, generate_plugin_manager_depfile
from internal_core.parsers.plugin_manager_parse import *
from plugin_manager_plugin_resolver import *
import argparse


@dataclass
class GenerateCCodeArguments:
    target_name: str
    build_platform: str
    build_dynamic_plugins: bool

    plugin_registry_toml: Path
    app_toml: Path
    statically_resolved_plugins_json: Path

    source_plugin_manager_bootloader_generated_src: Path
    generated_plugin_manager_bootloader_generated_src: Path

    depfile: Path

    @classmethod
    def from_args(cls, args: argparse.Namespace) -> "GenerateCCodeArguments":
        return cls(
            target_name=args.target_name,
            build_platform=args.build_platform,
            build_dynamic_plugins=args.build_dynamic_plugins,
            #
            plugin_registry_toml=args.plugin_registry_toml_path,
            app_toml=args.app_toml_path,
            statically_resolved_plugins_json=args.statically_resolved_plugins_json_path,
            #
            source_plugin_manager_bootloader_generated_src=args.source_plugin_manager_bootloader_generated_src_path,
            generated_plugin_manager_bootloader_generated_src=args.generated_plugin_manager_bootloader_generated_src_path,
            depfile=args.depfile_path,
        )


def parce_c_code_arguments() -> GenerateCCodeArguments:
    parser = argparse.ArgumentParser(description="Generate c code files")

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
        "--statically-resolved-plugins-json-path",
        required=True,
        type=Path,
    )

    parser.add_argument(
        "--source-plugin-manager-bootloader-generated-src-path",
        required=True,
        type=Path,
    )
    parser.add_argument(
        "--generated-plugin-manager-bootloader-generated-src-path",
        required=True,
        type=Path,
    )

    parser.add_argument(
        "--depfile-path",
        required=True,
        type=Path,
    )

    args = parser.parse_args()

    return GenerateCCodeArguments.from_args(args)


RECURSIVE_DEPENDENCY_SOLVER_MAX_DEPTH = 256


def main():
    arguments = parce_c_code_arguments()

    plugin_registry_dict = read_toml(arguments.plugin_registry_toml)
    app_dict = read_toml(arguments.app_toml)
    statically_resolved_plugins_dict = read_json(
        arguments.statically_resolved_plugins_json
    )

    plugin_registry = parse_plugin_registry(
        plugin_registry_dict,
        arguments.build_platform,
    )
    # TODO: Make a class for app_config that has all app configurations rather than just a list of requested plugins
    app_config = parse_app_dict(app_dict)

    plugin_providers = list(
        parse_statically_resolved_plugins(statically_resolved_plugins_dict)
    )

    interface_definitions = create_interface_definitions(plugin_registry)
    plugin_providers = sort_plugin_providers(plugin_providers)

    # TODO: Make sure this gets a proper name and gets filled appropiately
    plugin_manifests_for_metadata = [
        plugin_manifest
        for plugin_manifest in plugin_registry.plugin_manifests
        if plugin_manifest.interface_name == "plugin_manager"
    ]

    generate_plugin_manager_bootloader_generated_src(
        arguments.source_plugin_manager_bootloader_generated_src,
        arguments.generated_plugin_manager_bootloader_generated_src,
        interface_definitions,
        plugin_manifests_for_metadata,
        app_config.requested_plugins,
    )

    generate_plugin_manager_depfile(
        arguments.depfile,
        arguments.generated_plugin_manager_bootloader_generated_src,
        plugin_registry,
    )


if __name__ == "__main__":
    main()
