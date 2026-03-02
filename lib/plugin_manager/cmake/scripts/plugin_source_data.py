from dataclasses import dataclass

@dataclass
class PluginSourceData:
    target_name: str
    source_path: str
    register_path: str
