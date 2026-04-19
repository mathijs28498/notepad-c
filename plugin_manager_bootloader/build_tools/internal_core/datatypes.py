from dataclasses import dataclass
from pathlib import Path
from typing import Optional
from enum import Enum

from plugin_sdk_core.datatypes import PluginManifest, PluginLifetime


@dataclass
class PluginRegistryInterface:
    interface_name: str
    default_plugin_name: str


@dataclass
class PluginRegistry:
    plugin_manifests: list[PluginManifest]
    # TODO: Change this name to defaults, as thats what they actually are
    interfaces: list[PluginRegistryInterface]


@dataclass
class InterfaceDefinition:
    interface_name: str
    default_plugin_manifest: PluginManifest
    plugin_manifests: list[PluginManifest]


@dataclass
class RequestedPlugin:
    interface_name: str
    plugin_name: Optional[str]
    lifetime: PluginLifetime
    is_explicit: bool


@dataclass
class AppConfig:
    requested_plugins: list[RequestedPlugin]


@dataclass
class PluginProviderDependency:
    interface_name: str
    set_fn: str


@dataclass
class PluginProvider:
    plugin_manifest: PluginManifest
    framework_declarations: list[str]
    dependencies: list[PluginProviderDependency]
    get_interface_fn_text: str
    init_fn_text: str
    shutdown_fn_text: str
    is_explicit: bool
    is_initialized: bool
