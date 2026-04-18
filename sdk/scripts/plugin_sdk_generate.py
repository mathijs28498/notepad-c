from tools.generators.plugin_sdk_templates import (
    generate_register_inc,
    generate_plugin_dependencies,
)

from tools.parsers.manifest_parse import parse_plugin_manifest

from tools.utils import read_toml

from dataclasses import dataclass
from pathlib import Path
import argparse

@dataclass
class GenerateRegisterIncArguments:
    build_dynamic: bool
    manifest_toml: Path
    source_plugin_register_inc: Path
    source_plugin_dependencies: Path
    generated_plugin_register_inc: Path
    generated_plugin_dependencies: Path

    @classmethod
    def from_args(cls, args: argparse.Namespace) -> "GenerateRegisterIncArguments":
        return cls(
            build_dynamic=args.build_dynamic,
            manifest_toml=args.manifest_toml_path,
            source_plugin_register_inc=args.source_plugin_register_inc_path,
            source_plugin_dependencies=args.source_plugin_dependencies_path,
            generated_plugin_register_inc=args.generated_plugin_register_inc_path,
            generated_plugin_dependencies=args.generated_plugin_dependencies_path,
        )



def parse_register_inc_arguments() -> GenerateRegisterIncArguments:
    parser = argparse.ArgumentParser(description="Generate plugin files")

    parser.add_argument(
        "--build-dynamic",
        action="store_true",
    )
    parser.add_argument(
        "--manifest-toml-path",
        required=True,
        type=Path,
    )
    parser.add_argument(
        "--source-plugin-register-inc-path",
        required=True,
        type=Path,
    )
    parser.add_argument(
        "--source-plugin-dependencies-path",
        required=True,
        type=Path,
    )
    parser.add_argument(
        "--generated-plugin-register-inc-path",
        required=True,
        type=Path,
    )
    parser.add_argument(
        "--generated-plugin-dependencies-path",
        required=True,
        type=Path,
    )

    args = parser.parse_args()

    return GenerateRegisterIncArguments.from_args(args)

from pathlib import Path


def main():
    arguments = parse_register_inc_arguments()

    plugin_manifest_dict = read_toml(arguments.manifest_toml)
    plugin_manifest = parse_plugin_manifest(
        plugin_manifest_dict, arguments.manifest_toml
    )

    # TODO: Change names to have header in the naming
    generate_plugin_dependencies(
        arguments.source_plugin_dependencies,
        arguments.generated_plugin_dependencies,
        plugin_manifest,
    )

    generate_register_inc(
        arguments.source_plugin_register_inc,
        arguments.generated_plugin_register_inc,
        arguments.build_dynamic,
        plugin_manifest,
    )


if __name__ == "__main__":
    main()
