# Infrastructure for self-contained plugins.
#
# Each plugin lives in its own directory with a CMakeLists.txt calling mardyn_add_plugin().
# Such directories are picked up automatically by mardyn_add_plugin_subdirectories().
# Every plugin gets an option MARDYN_PLUGIN_<name> to enable or disable it individually.
#
# Plugins are collected in named registries (default: "plugins"). For each registry,
# mardyn_generate_plugin_registration() generates a function registering all enabled plugins of
# that registry with a PluginFactory. Registries allow plugin types with different base classes.

include(CMakeDependentOption)

set(MARDYN_PLUGIN_DEFAULT_REGISTRY "plugins")

# mardyn_add_plugin(<name>
#                   SOURCES <source>...
#                   HEADER <header>
#                   [CLASSES <class>...]
#                   [REGISTRY <registry>]
#                   [REQUIRES <condition>...]
#                   [LIBRARIES <library>...]
#                   [DEFAULT ON|OFF])
#
# Builds the plugin as OBJECT library mardyn_plugin_<name> and links it into MarDyn if the option
# MARDYN_PLUGIN_<name> is enabled. The plugin is compiled with the shared build settings of MarDyn
# (target mardyn_settings).
#   SOURCES    source files of the plugin, relative to the current directory
#   HEADER     header declaring the plugin classes, relative to the current directory
#   CLASSES    plugin classes to register (default: <name>); each class must provide
#              a static createInstance() method
#   REGISTRY   registry the plugin classes are added to (default: plugins)
#   REQUIRES   conditions that must be met for the plugin to be available, e.g. ENABLE_VTK
#              or "NOT MARDYN_AUTOPAS"; otherwise the option is hidden and forced OFF
#   LIBRARIES  additional libraries the plugin links against
#   DEFAULT    default value of the option (default: ON)
function(mardyn_add_plugin NAME)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "" "HEADER;REGISTRY;DEFAULT" "SOURCES;CLASSES;REQUIRES;LIBRARIES")
    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "mardyn_add_plugin(${NAME}): unknown arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    if(NOT ARG_SOURCES OR NOT ARG_HEADER)
        message(FATAL_ERROR "mardyn_add_plugin(${NAME}): SOURCES and HEADER are required")
    endif()
    if(NOT DEFINED ARG_DEFAULT)
        set(ARG_DEFAULT ON)
    endif()
    if(NOT ARG_CLASSES)
        set(ARG_CLASSES ${NAME})
    endif()
    if(NOT ARG_REGISTRY)
        set(ARG_REGISTRY ${MARDYN_PLUGIN_DEFAULT_REGISTRY})
    endif()

    set(option_name MARDYN_PLUGIN_${NAME})
    if(ARG_REQUIRES)
        cmake_dependent_option(${option_name} "Build plugin ${NAME}" ${ARG_DEFAULT} "${ARG_REQUIRES}" OFF)
    else()
        option(${option_name} "Build plugin ${NAME}" ${ARG_DEFAULT})
    endif()

    if(NOT ${option_name})
        message(STATUS "Plugin ${NAME} (${ARG_REGISTRY}): disabled")
        return()
    endif()
    message(STATUS "Plugin ${NAME} (${ARG_REGISTRY}): enabled")

    get_property(generated GLOBAL PROPERTY MARDYN_PLUGIN_REGISTRY_${ARG_REGISTRY}_GENERATED)
    if(generated)
        message(FATAL_ERROR "mardyn_add_plugin(${NAME}): registration code for registry '${ARG_REGISTRY}' "
                            "has already been generated; add the plugin before calling "
                            "mardyn_generate_plugin_registration(${ARG_REGISTRY} ...)")
    endif()

    set(target mardyn_plugin_${NAME})
    add_library(${target} OBJECT ${ARG_SOURCES})
    target_link_libraries(${target} PRIVATE mardyn_settings ${ARG_LIBRARIES})
    target_link_libraries(MarDyn PRIVATE ${target})

    # header path relative to src/, as used for includes in MarDyn
    file(RELATIVE_PATH header "${PROJECT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_HEADER}")
    set_property(GLOBAL APPEND PROPERTY MARDYN_PLUGIN_REGISTRY_${ARG_REGISTRY}_HEADERS "${header}")
    set_property(GLOBAL APPEND PROPERTY MARDYN_PLUGIN_REGISTRY_${ARG_REGISTRY}_CLASSES ${ARG_CLASSES})

    get_property(registries GLOBAL PROPERTY MARDYN_PLUGIN_REGISTRIES)
    if(NOT ARG_REGISTRY IN_LIST registries)
        set_property(GLOBAL APPEND PROPERTY MARDYN_PLUGIN_REGISTRIES ${ARG_REGISTRY})
    endif()
endfunction()

# mardyn_add_plugin_subdirectories()
#
# Adds all direct subdirectories of the current directory whose CMakeLists.txt calls
# mardyn_add_plugin() or mardyn_add_plugin_subdirectories(). The latter allows directories that
# only group plugins (e.g. io/formats/) as well as plugins with sub-plugins of their own; each
# level searches only its direct subdirectories. Other subdirectories are left alone and have to
# be added explicitly.
#
# Conventions for nested plugins:
#   - Plugin names must be unique across all directories and registries, as they define the
#     option MARDYN_PLUGIN_<name> (e.g. TcpConnector instead of tcp).
#   - Sub-plugins that are only useful together with their parent plugin should declare
#     REQUIRES MARDYN_PLUGIN_<parent>, so they are disabled together with the parent.
function(mardyn_add_plugin_subdirectories)
    file(GLOB children LIST_DIRECTORIES true CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*")
    foreach(dir IN LISTS children)
        if(IS_DIRECTORY "${dir}" AND EXISTS "${dir}/CMakeLists.txt")
            file(STRINGS "${dir}/CMakeLists.txt" plugin_calls
                 REGEX "^[ \t]*mardyn_add_plugin(_subdirectories)?[ \t]*\\(")
            if(plugin_calls)
                add_subdirectory("${dir}")
            endif()
        endif()
    endforeach()
endfunction()

# mardyn_generate_plugin_registration(<registry>
#                                     FUNCTION <function>
#                                     BASE <class>
#                                     BASE_HEADER <header>
#                                     OUTPUT <path>
#                                     [OUTPUT_DIR <dir>])
#
# Generates <path>.h declaring and <path>.cpp defining
#   void <function>(PluginFactory<<class>>& factory);
# which registers all enabled plugins of <registry>, and adds the source to MarDyn.
# Has to be called after all directories adding plugins to <registry>.
#   FUNCTION     name of the generated registration function
#   BASE         base class of the plugins, i.e. template argument of PluginFactory
#   BASE_HEADER  header declaring the base class, relative to src/
#   OUTPUT       path of the generated files without extension, relative to OUTPUT_DIR;
#                the header is included as "<path>.h"
#   OUTPUT_DIR   directory for the generated files, has to be on the include path of MarDyn
#                (default: binary directory of src/)
function(mardyn_generate_plugin_registration REGISTRY)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "" "FUNCTION;BASE;BASE_HEADER;OUTPUT;OUTPUT_DIR" "")
    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "mardyn_generate_plugin_registration(${REGISTRY}): unknown arguments: "
                            "${ARG_UNPARSED_ARGUMENTS}")
    endif()
    foreach(required FUNCTION BASE BASE_HEADER OUTPUT)
        if(NOT ARG_${required})
            message(FATAL_ERROR "mardyn_generate_plugin_registration(${REGISTRY}): ${required} is required")
        endif()
    endforeach()
    if(NOT ARG_OUTPUT_DIR)
        set(ARG_OUTPUT_DIR "${PROJECT_BINARY_DIR}/src")
    endif()

    get_property(headers GLOBAL PROPERTY MARDYN_PLUGIN_REGISTRY_${REGISTRY}_HEADERS)
    get_property(classes GLOBAL PROPERTY MARDYN_PLUGIN_REGISTRY_${REGISTRY}_CLASSES)

    # variables used in the templates
    set(REGISTRY_NAME ${REGISTRY})
    set(REGISTRY_FUNCTION ${ARG_FUNCTION})
    set(REGISTRY_BASE ${ARG_BASE})
    set(REGISTRY_BASE_HEADER ${ARG_BASE_HEADER})
    set(REGISTRY_HEADER "${ARG_OUTPUT}.h")
    string(MAKE_C_IDENTIFIER "${ARG_OUTPUT}_H_" REGISTRY_INCLUDE_GUARD)
    string(TOUPPER "${REGISTRY_INCLUDE_GUARD}" REGISTRY_INCLUDE_GUARD)
    set(REGISTRY_INCLUDES "")
    foreach(header IN LISTS headers)
        string(APPEND REGISTRY_INCLUDES "#include \"${header}\"\n")
    endforeach()
    set(REGISTRY_REGISTRATIONS "")
    foreach(class IN LISTS classes)
        string(APPEND REGISTRY_REGISTRATIONS "\tfactory.registerPlugin(&${class}::createInstance);\n")
    endforeach()

    set(templates "${CMAKE_CURRENT_FUNCTION_LIST_DIR}")
    configure_file("${templates}/PluginRegistration.h.in" "${ARG_OUTPUT_DIR}/${ARG_OUTPUT}.h" @ONLY)
    configure_file("${templates}/PluginRegistration.cpp.in" "${ARG_OUTPUT_DIR}/${ARG_OUTPUT}.cpp" @ONLY)
    target_sources(MarDyn PRIVATE "${ARG_OUTPUT_DIR}/${ARG_OUTPUT}.cpp")

    set_property(GLOBAL PROPERTY MARDYN_PLUGIN_REGISTRY_${REGISTRY}_GENERATED TRUE)
endfunction()

# Checks at the end of the configuration that registration code was generated for every registry
# that has plugins, so that no plugin is silently left unregistered.
function(_mardyn_check_plugin_registries)
    get_property(registries GLOBAL PROPERTY MARDYN_PLUGIN_REGISTRIES)
    foreach(registry IN LISTS registries)
        get_property(generated GLOBAL PROPERTY MARDYN_PLUGIN_REGISTRY_${registry}_GENERATED)
        if(NOT generated)
            message(FATAL_ERROR "Plugin registry '${registry}' has plugins, but no registration code was "
                                "generated. Call mardyn_generate_plugin_registration(${registry} ...).")
        endif()
    endforeach()
endfunction()
cmake_language(DEFER DIRECTORY "${PROJECT_SOURCE_DIR}" CALL _mardyn_check_plugin_registries)
