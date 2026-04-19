from dataclasses import dataclass
from enum import Enum
from pathlib import Path

@dataclass
class PluginDependency:
    variable_name: str
    is_deferred: bool

class PluginLifetime(Enum):
    UNKNOWN = "unknown"
    SINGLETON = "singleton"
    SCOPED = "scoped"
    TRANSIENT = "transient"

@dataclass
class PluginManifest:
    target_name: str
    interface_name: str
    plugin_name: str
    supported_lifetimes: list[PluginLifetime]
    preferred_lifetime: PluginLifetime
    has_init: bool
    has_shutdown: bool
    dependencies: dict[str, PluginDependency]
    manifest_path: Path
    source_path: Path
    module_path: Path