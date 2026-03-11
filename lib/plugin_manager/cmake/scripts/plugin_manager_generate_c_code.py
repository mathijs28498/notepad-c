from plugin_manager_generate_templates import *
from plugin_manager_parse import *
from plugin_manager_generate_arguments import parce_c_code_arguments
from plugin_manager_plugin_resolver import *
from plugin_manager_generate_init_contexts import generate_init_contexts_src

RECURSIVE_DEPENDENCY_SOLVER_MAX_DEPTH = 256


def main():
    arguments = parce_c_code_arguments()

    plugin_registry_dict = read_toml(arguments.plugin_registry_toml)
    plugin_list_dict = read_toml(arguments.plugin_list_toml)

    plugin_registry = parse_plugin_registry(
        plugin_registry_dict,
        arguments.build_platform,
    )
    requested_plugins = parse_plugin_list(plugin_list_dict)

    plugin_providers = create_static_plugin_providers(
        plugin_registry,
        arguments.build_dynamic_plugins,
    )
    interface_definitions = create_interface_definitions(plugin_registry)

    # TODO: Get the plugin_providers from json, dont figure it out yourself here
    if not arguments.build_dynamic_plugins:
        # Make sure the same plugin is not requested more than once as this is not supported
        requested_plugins.extend(
            ensure_core_plugins_requested(plugin_providers, requested_plugins)
        )

        requested_plugin_providers: list[PluginProvider] = []
        for _ in range(RECURSIVE_DEPENDENCY_SOLVER_MAX_DEPTH):
            if not requested_plugins:
                break
            new_requested_plugin_providers = resolve_requested_plugin_providers(
                plugin_providers, requested_plugins
            )

            requested_plugins = check_resolved_requested_plugin_providers(
                new_requested_plugin_providers,
                requested_plugin_providers,
                requested_plugins,
            )

            requested_plugin_providers.extend(new_requested_plugin_providers)
        else:
            print(
                f"Hit maximum recursive dependency solver depth, maximum ({RECURSIVE_DEPENDENCY_SOLVER_MAX_DEPTH})"
            )
            return -1
        
        plugin_providers = requested_plugin_providers
        requested_plugins = []

    #     TODO: Configure time:
    #     - Save these plugins with their dependencies to json for compile time
    #     """

    plugin_providers = sort_plugin_providers(plugin_providers)

    generate_plugin_registry_header(
        arguments.source_plugin_registry_header,
        arguments.generated_plugin_registry_header,
        interface_definitions,
    )

    generate_plugin_manager_header(
        arguments.source_plugin_manager_header,
        arguments.generated_plugin_manager_header,
        plugin_providers,
    )

    generate_plugin_registry_src(
        arguments.source_plugin_registry_src,
        arguments.generated_plugin_registry_src,
        interface_definitions,
    )

    generate_init_contexts_src(
        arguments.source_init_contexts_src,
        arguments.generated_init_contexts_src,
        requested_plugins,
        # The plugin_providers list is assumed to be properly sorted already, so sorted plugin indices can be created like this
        [i for i in range(len(plugin_providers))],
        plugin_providers,
    )


if __name__ == "__main__":
    main()
