include_guard(GLOBAL)

function(plugin_manager_setup TARGET_NAME PYTHON_SCRIPTS_PATH JSON_DIR_PATH GEN_DIR_PATH)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    set(BUILD_PLATFORM "")
    if(WIN32)
        set(BUILD_PLATFORM "win32")
    endif()

    set(GENERATED_INCLUDE_PATH "${GEN_DIR_PATH}/include")
    set(GENERATED_SRC_PATH "${GEN_DIR_PATH}/src")
    set(GENERATED_CMAKE_PATH "${GEN_DIR_PATH}/cmake")

    set(PLUGIN_REGISTRY_JSON_PATH "${JSON_DIR_PATH}/plugin_registry.json")
    set(PLUGIN_LIST_JSON_PATH "${JSON_DIR_PATH}/plugin_list.json")

    set(SOURCE_PLUGIN_REGISTRY_SRC_PATH "${CMAKE_CURRENT_LIST_DIR}/plugin_registry.c.in")
    set(SOURCE_GET_SETUP_CONTEXT_SRC_PATH "${CMAKE_CURRENT_LIST_DIR}/plugin_manager_get_setup_context.c.in")

    set(GENERATED_PLUGIN_REGISTRY_HEADER_PATH "${GENERATED_INCLUDE_PATH}/plugin_registry_generated.h")
    set(GENERATED_PLUGIN_MANAGER_HEADER_PATH "${GENERATED_INCLUDE_PATH}/plugin_manager_generated.h")
    set(GENERATED_PLUGIN_REGISTRY_SRC_PATH "${GENERATED_SRC_PATH}/plugin_registry.c")
    set(GENERATED_GET_SETUP_CONTEXT_SRC_PATH "${GENERATED_SRC_PATH}/plugin_manager_get_setup_context.c")
    set(GENERATED_PLUGIN_REGISTRY_CMAKE_PATH "${GENERATED_CMAKE_PATH}/PluginRegistry.cmake")
    set(GENERATED_PLUGIN_MANAGER_TARGETS_CMAKE_PATH "${GENERATED_CMAKE_PATH}/PluginManagerTargets.cmake")

    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS 
        "${PLUGIN_LIST_JSON_PATH}"
        "${PLUGIN_REGISTRY_JSON_PATH}"
        "${SOURCE_GET_SETUP_CONTEXT_SRC_PATH}"
        "${SOURCE_PLUGIN_REGISTRY_SRC_PATH}"
        "${PYTHON_SCRIPTS_PATH}/plugin_manager_generate_plugins.py"
        "${PYTHON_SCRIPTS_PATH}/plugin_manager_generate_registry.py"
        "${PYTHON_SCRIPTS_PATH}/plugin_manager_generate.py"
        "${PYTHON_SCRIPTS_PATH}/plugin_source_data.py"
    )
    
endfunction()


message("TODO: Create new code for statically loading the plugins with dependencies - insert this code under an #if defined(BUILD_STATIC) or something")
function(plugin_manager_load_registry TARGET_NAME JSON_DIR_PATH GEN_DIR_PATH)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    set(BUILD_PLATFORM "")
    if(WIN32)
        set(BUILD_PLATFORM "win32")
    endif()

    set(GENERATED_INCLUDE_PATH "${GEN_DIR_PATH}/include")
    set(GENERATED_SRC_PATH "${GEN_DIR_PATH}/src")
    set(GENERATED_CMAKE_PATH "${GEN_DIR_PATH}/cmake")

    set(PLUGIN_REGISTRY_JSON_PATH "${JSON_DIR_PATH}/plugin_registry.json")

    set(SOURCE_PLUGIN_REGISTRY_SRC_PATH "${CMAKE_CURRENT_LIST_DIR}/plugin_registry.c.in")

    set(GENERATED_PLUGIN_REGISTRY_HEADER_PATH "${GENERATED_INCLUDE_PATH}/plugin_registry_generated.h")
    set(GENERATED_PLUGIN_REGISTRY_SRC_PATH "${GENERATED_SRC_PATH}/plugin_registry.c")
    set(GENERATED_CMAKE_PATH "${GENERATED_CMAKE_PATH}/PluginRegistry.cmake")


    set(PLUGIN_MANAGER_GENERATE_PLUGINS_SCRIPT_PATH "${PYTHON_SCRIPTS_PATH}/plugin_manager_generate_registry.py")
    message(STATUS "Running script: ${PLUGIN_MANAGER_GENERATE_PLUGINS_SCRIPT_PATH}")
    execute_process(
        COMMAND "${Python3_EXECUTABLE}" "${PLUGIN_MANAGER_GENERATE_PLUGINS_SCRIPT_PATH}"
            --build-platform "${BUILD_PLATFORM}"
            --target-name "${TARGET_NAME}"

            --plugin-registry-json-path "${PLUGIN_REGISTRY_JSON_PATH}"

            --source-plugin-registry-src-path "${SOURCE_PLUGIN_REGISTRY_SRC_PATH}"

            --generated-plugin-registry-header-path "${GENERATED_PLUGIN_REGISTRY_HEADER_PATH}"
            --generated-plugin-registry-src-path "${GENERATED_PLUGIN_REGISTRY_SRC_PATH}"
            --generated-cmake-path "${GENERATED_CMAKE_PATH}"
        COMMAND_ERROR_IS_FATAL ANY
    )

    include("${GENERATED_CMAKE_PATH}")
endfunction()

function(plugin_manager_load_plugins TARGET_NAME PYTHON_SCRIPTS_PATH JSON_DIR_PATH GEN_DIR_PATH)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    set(GENERATED_INCLUDE_PATH "${GEN_DIR_PATH}/include")
    set(GENERATED_SRC_PATH "${GEN_DIR_PATH}/src")
    set(GENERATED_CMAKE_PATH "${GEN_DIR_PATH}/cmake")

    set(PLUGIN_REGISTRY_JSON_PATH "${JSON_DIR_PATH}/plugin_registry.json")
    set(PLUGIN_LIST_JSON_PATH "${JSON_DIR_PATH}/plugin_list.json")

    set(SOURCE_GET_SETUP_CONTEXT_SRC_PATH "${CMAKE_CURRENT_LIST_DIR}/plugin_manager_get_setup_context.c.in")

    set(GENERATED_PLUGIN_MANAGER_HEADER_PATH "${GENERATED_INCLUDE_PATH}/plugin_manager_generated.h")
    set(GENERATED_GET_SETUP_CONTEXT_SRC_PATH "${GENERATED_SRC_PATH}/plugin_manager_get_setup_context.c")
    set(GENERATED_CMAKE_PATH "${GENERATED_CMAKE_PATH}/PluginManagerTargets.cmake")

    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS 
        "${PLUGIN_LIST_JSON_PATH}"
        "${PLUGIN_REGISTRY_JSON_PATH}"
        "${SOURCE_GET_SETUP_CONTEXT_SRC_PATH}"
        "${PYTHON_SCRIPTS_PATH}/plugin_manager_generate_plugins.py"
        "${PYTHON_SCRIPTS_PATH}/plugin_manager_generate.py"
        "${PYTHON_SCRIPTS_PATH}/plugin_source_data.py"
    )

    set(PLUGIN_MANAGER_GENERATE_PLUGINS_SCRIPT_PATH "${PYTHON_SCRIPTS_PATH}/plugin_manager_generate_plugins.py")
    message(STATUS "Running script: ${PLUGIN_MANAGER_GENERATE_PLUGINS_SCRIPT_PATH}")
    execute_process(
        COMMAND "${Python3_EXECUTABLE}" "${PLUGIN_MANAGER_GENERATE_PLUGINS_SCRIPT_PATH}"
            --target-name "${TARGET_NAME}"
            --plugin-registry-json-path "${PLUGIN_REGISTRY_JSON_PATH}"
            --plugin-list-json-path "${PLUGIN_LIST_JSON_PATH}"

            --source-get-setup-context-src-path "${SOURCE_GET_SETUP_CONTEXT_SRC_PATH}"

            --generated-plugin-manager-header-path "${GENERATED_PLUGIN_MANAGER_HEADER_PATH}"
            --generated-get-setup-context-src-path "${GENERATED_GET_SETUP_CONTEXT_SRC_PATH}"
            --generated-cmake-path "${GENERATED_CMAKE_PATH}"
        COMMAND_ERROR_IS_FATAL ANY
    )

    include("${GENERATED_CMAKE_PATH}")
endfunction()