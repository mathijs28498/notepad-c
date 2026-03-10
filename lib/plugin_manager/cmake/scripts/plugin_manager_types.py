from dataclasses import dataclass
from pathlib import Path
from typing import Optional


@dataclass
class PluginDependency:
    interface_name: str
    variable_name: str


@dataclass
class PluginManifest:
    target_name: str
    static_only: bool
    interface_name: str
    plugin_name: str
    dependencies: list[PluginDependency]
    get_interface_fn: str
    init_fn: Optional[str]
    shutdown_fn: Optional[str]
    source_path: Path
    module_path: Optional[Path]
    exported_declarations: list[str]


@dataclass
class PluginRegistryInterface:
    interface_name: str
    default_plugin_name: str


@dataclass
class PluginRegistry:
    plugin_manifests: list[PluginManifest]
    interfaces: list[PluginRegistryInterface]


@dataclass
class InterfaceDefinition:
    interface_name: str
    default_plugin_manifest: PluginManifest
    plugin_manifests: list[PluginManifest]


# @dataclass
# class InternalPlugin:
#     source_data: PluginSourceData
#     forward_declarations: list[str]
#     plugin_provider: PluginProvider


@dataclass
class RequestedPlugin:
    interface_name: str
    plugin_name: str


# @dataclass
# class PluginDefinition:
#     plugin_name: str
#     module_path: Optional[Path]
#     source_path: Path


@dataclass
class PluginProvider:
    plugin_manifest: PluginManifest
    framework_declarations: list[str]
    # interface_name: str
    # plugin_name: str
    # dependencies: list[PluginDependency]
    get_interface_fn_text: str
    init_fn_text: str
    shutdown_fn_text: str
    is_explicit: bool
    is_initialized: bool
