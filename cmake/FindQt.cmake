include_guard(GLOBAL)

# ==================================================================================================
# Helpers
# ==================================================================================================

function(_qt_find_log _verbose _kind _text)
    if(_kind STREQUAL "STATUS")
        if(_verbose)
            message(STATUS "[qt_find] ${_text}")
        endif()
    elseif(_kind STREQUAL "WARNING")
        if(_verbose)
            message(WARNING "[qt_find] ${_text}")
        endif()
    elseif(_kind STREQUAL "FATAL")
        message(FATAL_ERROR "[qt_find] ${_text}")
    else()
        if(_verbose)
            message("[qt_find] ${_text}")
        endif()
    endif()
endfunction()

function(_qt_find_normalize_path _out _path)
    if(NOT _path)
        set(${_out} "" PARENT_SCOPE)
        return()
    endif()

    file(TO_CMAKE_PATH "${_path}" _norm)
    set(${_out} "${_norm}" PARENT_SCOPE)
endfunction()

function(_qt_find_make_cache_key _out)
    string(JOIN "|" _joined ${ARGN})
    string(MD5 _hash "${_joined}")
    set(${_out} "${_hash}" PARENT_SCOPE)
endfunction()

function(_qt_find_version_to_int _version _out)
    if(NOT _version)
        set(${_out} 0 PARENT_SCOPE)
        return()
    endif()

    string(REPLACE "." ";" _parts "${_version}")
    list(LENGTH _parts _len)
    while(_len LESS 3)
        list(APPEND _parts 0)
        list(LENGTH _parts _len)
    endwhile()

    list(GET _parts 0 _maj)
    list(GET _parts 1 _min)
    list(GET _parts 2 _pat)

    math(EXPR _value "${_maj} * 1000000 + ${_min} * 1000 + ${_pat}")
    set(${_out} "${_value}" PARENT_SCOPE)
endfunction()

function(_qt_find_version_matches _candidate _requested _out)
    if(NOT _requested)
        set(${_out} TRUE PARENT_SCOPE)
        return()
    endif()

    string(REPLACE "." ";" _req_parts "${_requested}")
    string(REPLACE "." ";" _cand_parts "${_candidate}")

    list(LENGTH _req_parts _req_len)
    list(LENGTH _cand_parts _cand_len)

    while(_cand_len LESS 3)
        list(APPEND _cand_parts 0)
        list(LENGTH _cand_parts _cand_len)
    endwhile()

    if(_req_len GREATER 3)
        set(${_out} FALSE PARENT_SCOPE)
        return()
    endif()

    set(_ok TRUE)
    math(EXPR _last "${_req_len} - 1")
    foreach(_idx RANGE 0 ${_last})
        list(GET _req_parts ${_idx} _r)
        list(GET _cand_parts ${_idx} _c)
        if(NOT _r STREQUAL _c)
            set(_ok FALSE)
            break()
        endif()
    endforeach()

    set(${_out} "${_ok}" PARENT_SCOPE)
endfunction()

function(_qt_find_normalize_tokens _input _out)
    string(TOLOWER "${_input}" _s)
    string(REPLACE "\\" " " _s "${_s}")
    string(REPLACE "/"  " " _s "${_s}")
    string(REPLACE "_"  " " _s "${_s}")
    string(REPLACE "-"  " " _s "${_s}")
    string(REGEX REPLACE "[ \t\r\n]+" " " _s "${_s}")
    string(STRIP "${_s}" _s)
    set(${_out} "${_s}" PARENT_SCOPE)
endfunction()

function(_qt_find_platform_matches _manifest _filter _out)
    if(NOT _filter)
        set(${_out} TRUE PARENT_SCOPE)
        return()
    endif()

    _qt_find_normalize_tokens("${_manifest}" _manifest_norm)
    _qt_find_normalize_tokens("${_filter}" _filter_norm)

    if(_filter_norm STREQUAL "")
        set(${_out} TRUE PARENT_SCOPE)
        return()
    endif()

    string(REPLACE " " ";" _tokens "${_filter_norm}")

    set(_ok TRUE)
    foreach(_tok IN LISTS _tokens)
        if(_tok STREQUAL "")
            continue()
        endif()

        string(FIND " ${_manifest_norm} " " ${_tok} " _pos)
        if(_pos EQUAL -1)
            set(_ok FALSE)
            break()
        endif()
    endforeach()

    set(${_out} "${_ok}" PARENT_SCOPE)
endfunction()

function(_qt_find_collect_roots _out _path _verbose)
    set(_roots)

    if(_path)
        _qt_find_normalize_path(_root "${_path}")
        if(EXISTS "${_root}" AND IS_DIRECTORY "${_root}")
            list(APPEND _roots "${_root}")
        endif()
    else()
        if(WIN32)
            foreach(_disk_letter RANGE 67 90) # C..Z
                string(ASCII ${_disk_letter} _drive)
                set(_candidate "${_drive}:/Qt")
                if(EXISTS "${_candidate}" AND IS_DIRECTORY "${_candidate}")
                    list(APPEND _roots "${_candidate}")
                endif()
            endforeach()
        endif()
    endif()

    list(REMOVE_DUPLICATES _roots)

    if(_roots)
        _qt_find_log("${_verbose}" "STATUS" "Qt roots: ${_roots}")
    else()
        _qt_find_log("${_verbose}" "STATUS" "No Qt root directories found")
    endif()

    set(${_out} "${_roots}" PARENT_SCOPE)
endfunction()

function(_qt_find_detect_build_profile _out_platform _out_compiler _out_arch _out_manifest)
    set(_platform "")
    set(_compiler "")
    set(_arch "")

    # Platform
    if(ANDROID)
        set(_platform "android")
    elseif(WIN32)
        set(_platform "windows")
    elseif(APPLE)
        set(_platform "macos")
    elseif(UNIX)
        set(_platform "linux")
    endif()

    # Compiler
    if(MSVC)
        set(_compiler "msvc")
    elseif(MINGW)
        set(_compiler "mingw")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(_compiler "clang")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(_compiler "gcc")
    endif()

    # Arch
    set(_arch_src "")
    if(CMAKE_GENERATOR_PLATFORM)
        set(_arch_src "${CMAKE_GENERATOR_PLATFORM}")
    elseif(CMAKE_VS_PLATFORM_NAME)
        set(_arch_src "${CMAKE_VS_PLATFORM_NAME}")
    elseif(CMAKE_SYSTEM_PROCESSOR)
        set(_arch_src "${CMAKE_SYSTEM_PROCESSOR}")
    elseif(CMAKE_HOST_SYSTEM_PROCESSOR)
        set(_arch_src "${CMAKE_HOST_SYSTEM_PROCESSOR}")
    endif()

    string(TOLOWER "${_arch_src}" _arch_src_l)

    if(_arch_src_l MATCHES "arm64|aarch64")
        set(_arch "arm64")
    elseif(_arch_src_l MATCHES "x86_64|amd64|win64|x64")
        set(_arch "x64")
    elseif(_arch_src_l MATCHES "(^|[^0-9])x86([^0-9]|$)|win32|i686|i386")
        set(_arch "x86")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(_arch "x64")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(_arch "x86")
    endif()

    set(_manifest "${_platform} ${_compiler} ${_arch}")
    string(STRIP "${_manifest}" _manifest)

    set(${_out_platform} "${_platform}" PARENT_SCOPE)
    set(${_out_compiler} "${_compiler}" PARENT_SCOPE)
    set(${_out_arch}     "${_arch}"     PARENT_SCOPE)
    set(${_out_manifest} "${_manifest}" PARENT_SCOPE)
endfunction()

function(_qt_find_compatibility_score _candidate_manifest _want_platform _want_compiler _want_arch _out_score)
    set(_score 0)

    _qt_find_normalize_tokens("${_candidate_manifest}" _m)

    if(_want_platform)
        string(FIND " ${_m} " " ${_want_platform} " _p_pos)
        if(NOT _p_pos EQUAL -1)
            math(EXPR _score "${_score} + 10000")
        else()
            math(EXPR _score "${_score} - 10000")
        endif()
    endif()

    if(_want_compiler)
        string(FIND " ${_m} " " ${_want_compiler} " _c_pos)
        if(NOT _c_pos EQUAL -1)
            math(EXPR _score "${_score} + 5000")
        else()
            # На desktop-сборке несовместимый компилятор — это почти критично
            math(EXPR _score "${_score} - 5000")
        endif()
    endif()

    if(_want_arch)
        string(FIND " ${_m} " " ${_want_arch} " _a_pos)
        if(NOT _a_pos EQUAL -1)
            math(EXPR _score "${_score} + 1000")
        else()
            math(EXPR _score "${_score} - 1000")
        endif()
    endif()

    set(${_out_score} "${_score}" PARENT_SCOPE)
endfunction()


function(_qt_find_safe_id _out _text)
    string(MD5 _id "${_text}")
    set(${_out} "${_id}" PARENT_SCOPE)
endfunction()

function(_qt_find_set_record_var _prefix _id _field _value)
    set("${_prefix}_${_id}_${_field}" "${_value}" PARENT_SCOPE)
endfunction()

function(_qt_find_discover _out_ids _roots _verbose _nocache)
    if(NOT _roots)
        set(${_out_ids} "" PARENT_SCOPE)
        return()
    endif()

    _qt_find_make_cache_key(_cache_key ${_roots})
    set(_cache_ids_var "QT_FIND_DISCOVERY_${_cache_key}_IDS")

    if((NOT _nocache) AND DEFINED CACHE{${_cache_ids_var}})
        _qt_find_log("${_verbose}" "STATUS" "Using cached Qt discovery data")
        set(${_out_ids} "$CACHE{${_cache_ids_var}}" PARENT_SCOPE)
        return()
    endif()

    set(_qmake_candidates)

    foreach(_root IN LISTS _roots)
        _qt_find_log("${_verbose}" "STATUS" "Scanning ${_root} for qmake")

        file(GLOB_RECURSE _root_qmakes
                LIST_DIRECTORIES false
                "${_root}/*/bin/qmake"
                "${_root}/*/bin/qmake.exe"
                "${_root}/*/*/bin/qmake"
                "${_root}/*/*/bin/qmake.exe"
                "${_root}/*/*/*/bin/qmake"
                "${_root}/*/*/*/bin/qmake.exe"
        )

        list(APPEND _qmake_candidates ${_root_qmakes})
    endforeach()

    list(REMOVE_DUPLICATES _qmake_candidates)
    list(LENGTH _qmake_candidates _count)
    _qt_find_log("${_verbose}" "STATUS" "Found ${_count} qmake candidate(s)")

    set(_ids)

    foreach(_qmake IN LISTS _qmake_candidates)
        if(NOT EXISTS "${_qmake}")
            continue()
        endif()

        _qt_find_log("${_verbose}" "STATUS" "Querying ${_qmake}")

        execute_process(
                COMMAND "${_qmake}" -query
                RESULT_VARIABLE _rc
                OUTPUT_VARIABLE _q
                ERROR_VARIABLE _qe
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        if(NOT _rc EQUAL 0)
            _qt_find_log("${_verbose}" "STATUS" "Skipping ${_qmake}: qmake -query returned ${_rc}")
            continue()
        endif()

        set(_QT_VERSION "")
        set(_QT_PREFIX "")
        set(_QT_HOST_PREFIX "")
        set(_QT_INSTALL_LIBS "")
        set(_QT_INSTALL_CMAKE "")
        set(_QT_INSTALL_HEADERS "")
        set(_QT_INSTALL_BINS "")
        set(_QT_INSTALL_PLUGINS "")
        set(_QT_INSTALL_QML "")
        set(_QT_SPEC "")
        set(_QT_XSPEC "")

        string(REPLACE "\r\n" "\n" _q "${_q}")
        string(REPLACE "\r" "\n" _q "${_q}")
        string(REGEX MATCHALL "[^\n]+" _lines "${_q}")

        foreach(_line IN LISTS _lines)
            if(_line MATCHES "^QT_VERSION:(.*)$")
                set(_QT_VERSION "${CMAKE_MATCH_1}")
            elseif(_line MATCHES "^QT_INSTALL_PREFIX:(.*)$")
                set(_QT_PREFIX "${CMAKE_MATCH_1}")
            elseif(_line MATCHES "^QT_HOST_PREFIX:(.*)$")
                set(_QT_HOST_PREFIX "${CMAKE_MATCH_1}")
            elseif(_line MATCHES "^QT_INSTALL_LIBS:(.*)$")
                set(_QT_INSTALL_LIBS "${CMAKE_MATCH_1}")
            elseif(_line MATCHES "^QT_INSTALL_HEADERS:(.*)$")
                set(_QT_INSTALL_HEADERS "${CMAKE_MATCH_1}")
            elseif(_line MATCHES "^QT_INSTALL_BINS:(.*)$")
                set(_QT_INSTALL_BINS "${CMAKE_MATCH_1}")
            elseif(_line MATCHES "^QT_INSTALL_PLUGINS:(.*)$")
                set(_QT_INSTALL_PLUGINS "${CMAKE_MATCH_1}")
            elseif(_line MATCHES "^QT_INSTALL_QML:(.*)$")
                set(_QT_INSTALL_QML "${CMAKE_MATCH_1}")
            elseif(_line MATCHES "^QT_INSTALL_CMAKE:(.*)$")
                set(_QT_INSTALL_CMAKE "${CMAKE_MATCH_1}")
            elseif(_line MATCHES "^QMAKE_SPEC:(.*)$")
                set(_QT_SPEC "${CMAKE_MATCH_1}")
            elseif(_line MATCHES "^QMAKE_XSPEC:(.*)$")
                set(_QT_XSPEC "${CMAKE_MATCH_1}")
            endif()
        endforeach()

        if(_QT_VERSION STREQUAL "" OR _QT_PREFIX STREQUAL "")
            _qt_find_log("${_verbose}" "STATUS" "Skipping ${_qmake}: no QT_VERSION or QT_INSTALL_PREFIX")
            continue()
        endif()

        _qt_find_normalize_path(_QT_PREFIX "${_QT_PREFIX}")
        _qt_find_normalize_path(_QT_HOST_PREFIX "${_QT_HOST_PREFIX}")
        _qt_find_normalize_path(_QT_INSTALL_LIBS "${_QT_INSTALL_LIBS}")
        _qt_find_normalize_path(_QT_INSTALL_CMAKE "${_QT_INSTALL_CMAKE}")
        _qt_find_normalize_path(_QT_INSTALL_HEADERS "${_QT_INSTALL_HEADERS}")
        _qt_find_normalize_path(_QT_INSTALL_BINS "${_QT_INSTALL_BINS}")
        _qt_find_normalize_path(_QT_INSTALL_PLUGINS "${_QT_INSTALL_PLUGINS}")
        _qt_find_normalize_path(_QT_INSTALL_QML "${_QT_INSTALL_QML}")

        string(REGEX MATCH "^([0-9]+)" _mv "${_QT_VERSION}")
        set(_QT_MAJOR "${CMAKE_MATCH_1}")

        set(_cmake_dir "")
        if(NOT _QT_INSTALL_CMAKE STREQUAL "" AND IS_DIRECTORY "${_QT_INSTALL_CMAKE}")
            set(_cmake_dir "${_QT_INSTALL_CMAKE}")
        elseif(NOT _QT_INSTALL_LIBS STREQUAL "" AND IS_DIRECTORY "${_QT_INSTALL_LIBS}/cmake")
            set(_cmake_dir "${_QT_INSTALL_LIBS}/cmake")
        elseif(IS_DIRECTORY "${_QT_PREFIX}/lib/cmake")
            set(_cmake_dir "${_QT_PREFIX}/lib/cmake")
        endif()

        set(_package_dir "")
        if(IS_DIRECTORY "${_cmake_dir}/Qt${_QT_MAJOR}")
            set(_package_dir "${_cmake_dir}/Qt${_QT_MAJOR}")
        elseif(EXISTS "${_cmake_dir}/Qt${_QT_MAJOR}Config.cmake")
            set(_package_dir "${_cmake_dir}")
        endif()

        set(_modules)
        if(NOT _cmake_dir STREQUAL "" AND IS_DIRECTORY "${_cmake_dir}")
            file(GLOB _pkgdirs LIST_DIRECTORIES true "${_cmake_dir}/Qt${_QT_MAJOR}*")
            foreach(_d IN LISTS _pkgdirs)
                if(IS_DIRECTORY "${_d}")
                    get_filename_component(_name "${_d}" NAME)
                    string(REGEX REPLACE "^Qt${_QT_MAJOR}" "" _mod "${_name}")

                    if(_QT_MAJOR STREQUAL "6")
                        if(NOT _mod STREQUAL "" AND
                                NOT _mod MATCHES "^(Config|ConfigVersion|BuildInternals|Platform|HostInfo|LinguistTools|Tools)$")
                            list(APPEND _modules "${_mod}")
                        endif()
                    else()
                        if(NOT _mod STREQUAL "")
                            list(APPEND _modules "${_mod}")
                        endif()
                    endif()
                endif()
            endforeach()
        endif()

        list(REMOVE_DUPLICATES _modules)
        list(SORT _modules)

        set(_kit_id "")
        foreach(_root IN LISTS _roots)
            file(RELATIVE_PATH _rel_try "${_root}" "${_qmake}")
            if(NOT _rel_try MATCHES "^[.][.]")
                set(_kit_id "${_rel_try}")
                break()
            endif()
        endforeach()
        if(_kit_id STREQUAL "")
            set(_kit_id "${_qmake}")
        endif()
        string(REPLACE "\\" "/" _kit_id "${_kit_id}")

        set(_manifest "${_kit_id} ${_QT_SPEC} ${_QT_XSPEC} ${_QT_PREFIX} ${_QT_HOST_PREFIX} ${_qmake}")

        if(_manifest MATCHES "android")
            string(APPEND _manifest " android")
        endif()
        if(_manifest MATCHES "mingw")
            string(APPEND _manifest " windows mingw")
        endif()
        if(_manifest MATCHES "msvc")
            string(APPEND _manifest " windows msvc")
        endif()
        if(_manifest MATCHES "clang")
            string(APPEND _manifest " clang")
        endif()
        if(_manifest MATCHES "win32|windows")
            string(APPEND _manifest " windows")
        endif()
        if(_manifest MATCHES "x86_64|_64|amd64")
            string(APPEND _manifest " x64")
        endif()
        if(_manifest MATCHES "(^|[^0-9])x86([^0-9]|$)|_32|i686")
            string(APPEND _manifest " x86")
        endif()
        if(_manifest MATCHES "arm64|aarch64|arm64_v8a")
            string(APPEND _manifest " arm64")
        endif()

        _qt_find_safe_id(_id "${_qmake}")

        list(APPEND _ids "${_id}")

        set(_prefix "QT_FIND_DISCOVERY_${_cache_key}")

        set(${_prefix}_${_id}_ID           "${_kit_id}"     CACHE INTERNAL "qt_find")
        set(${_prefix}_${_id}_QMAKE        "${_qmake}"      CACHE INTERNAL "qt_find")
        set(${_prefix}_${_id}_VERSION      "${_QT_VERSION}" CACHE INTERNAL "qt_find")
        set(${_prefix}_${_id}_MAJOR        "${_QT_MAJOR}"   CACHE INTERNAL "qt_find")
        set(${_prefix}_${_id}_PREFIX       "${_QT_PREFIX}"  CACHE INTERNAL "qt_find")
        set(${_prefix}_${_id}_HOST_PREFIX  "${_QT_HOST_PREFIX}" CACHE INTERNAL "qt_find")
        set(${_prefix}_${_id}_SPEC         "${_QT_SPEC}"    CACHE INTERNAL "qt_find")
        set(${_prefix}_${_id}_XSPEC        "${_QT_XSPEC}"   CACHE INTERNAL "qt_find")
        set(${_prefix}_${_id}_CMAKE_DIR    "${_cmake_dir}"  CACHE INTERNAL "qt_find")
        set(${_prefix}_${_id}_PACKAGE_DIR  "${_package_dir}" CACHE INTERNAL "qt_find")
        set(${_prefix}_${_id}_MANIFEST     "${_manifest}"   CACHE INTERNAL "qt_find")
        set(${_prefix}_${_id}_COMPONENTS   "${_modules}"    CACHE INTERNAL "qt_find")

        _qt_find_log("${_verbose}" "STATUS"
                "Found Qt${_QT_MAJOR} ${_QT_VERSION}; kit=${_kit_id}; spec=${_QT_SPEC}; xspec=${_QT_XSPEC}; cmake=${_cmake_dir}"
        )
    endforeach()

    if(NOT _nocache)
        set(${_cache_ids_var} "${_ids}" CACHE INTERNAL "qt_find discovery ids")
    endif()

    set(${_out_ids} "${_ids}" PARENT_SCOPE)
    set(QT_FIND_DISCOVERY_PREFIX "QT_FIND_DISCOVERY_${_cache_key}" PARENT_SCOPE)
endfunction()

function(qt_find)
    set(_opt VERBOSE NOCACHE REQUIRED)
    set(_one PATH VERSION PLATFORM)
    set(_multi COMPONENTS)
    cmake_parse_arguments(QTF "${_opt}" "${_one}" "${_multi}" ${ARGN})

    set(_verbose FALSE)
    if(QTF_VERBOSE)
        set(_verbose TRUE)
    endif()

    set(_requested_version "${QTF_VERSION}")
    set(_requested_platform "${QTF_PLATFORM}")
    set(_requested_components "${QTF_COMPONENTS}")

    _qt_find_detect_build_profile(
            _build_platform
            _build_compiler
            _build_arch
            _build_manifest
    )

    _qt_find_log("${_verbose}" "STATUS" "Build platform     : ${_build_platform}")
    _qt_find_log("${_verbose}" "STATUS" "Build compiler     : ${_build_compiler}")
    _qt_find_log("${_verbose}" "STATUS" "Build arch         : ${_build_arch}")
    _qt_find_log("${_verbose}" "STATUS" "Build manifest     : ${_build_manifest}")

    _qt_find_log("${_verbose}" "STATUS" "Requested version   : ${_requested_version}")
    _qt_find_log("${_verbose}" "STATUS" "Requested platform  : ${_requested_platform}")
    _qt_find_log("${_verbose}" "STATUS" "Requested components: ${_requested_components}")

    _qt_find_collect_roots(_roots "${QTF_PATH}" "${_verbose}")

    if(NOT _roots)
        if(QTF_REQUIRED)
            message(FATAL_ERROR "[qt_find] No Qt root directories found")
        else()
            _qt_find_log("${_verbose}" "WARNING" "No Qt root directories found")
            set(QT_FIND_FOUND FALSE PARENT_SCOPE)
            return()
        endif()
    endif()

    _qt_find_discover(_ids "${_roots}" "${_verbose}" "${QTF_NOCACHE}")

    if(NOT QT_FIND_DISCOVERY_PREFIX)
        _qt_find_make_cache_key(_cache_key ${_roots})
        set(QT_FIND_DISCOVERY_PREFIX "QT_FIND_DISCOVERY_${_cache_key}")
    endif()

    if(NOT _ids)
        if(QTF_REQUIRED)
            message(FATAL_ERROR "[qt_find] No Qt installations found")
        else()
            _qt_find_log("${_verbose}" "WARNING" "No Qt installations found")
            set(QT_FIND_FOUND FALSE PARENT_SCOPE)
            return()
        endif()
    endif()

    set(_best_id "")
    set(_best_score -1)

    foreach(_rid IN LISTS _ids)
        set(_prefix "${QT_FIND_DISCOVERY_PREFIX}")

        set(_version     "${${_prefix}_${_rid}_VERSION}")
        set(_major       "${${_prefix}_${_rid}_MAJOR}")
        set(_kit_id      "${${_prefix}_${_rid}_ID}")
        set(_manifest    "${${_prefix}_${_rid}_MANIFEST}")
        set(_package_dir "${${_prefix}_${_rid}_PACKAGE_DIR}")

        _qt_find_version_matches("${_version}" "${_requested_version}" _version_ok)
        if(NOT _version_ok)
            _qt_find_log("${_verbose}" "STATUS" "Reject ${_kit_id}: version ${_version} does not match ${_requested_version}")
            continue()
        endif()

        _qt_find_platform_matches("${_manifest}" "${_requested_platform}" _platform_ok)
        if(NOT _platform_ok)
            _qt_find_log("${_verbose}" "STATUS" "Reject ${_kit_id}: platform does not match '${_requested_platform}'")
            continue()
        endif()

        set(_compat_score 0)
        if(NOT _requested_platform)
            _qt_find_compatibility_score(
                    "${_manifest}"
                    "${_build_platform}"
                    "${_build_compiler}"
                    "${_build_arch}"
                    _compat_score
            )
        endif()

        if(_package_dir STREQUAL "")
            _qt_find_log("${_verbose}" "STATUS" "Reject ${_kit_id}: package dir not found")
            continue()
        endif()

        _qt_find_version_to_int("${_version}" _ver_score)
        set(_score "${_ver_score}")

        if(_requested_version)
            string(REPLACE "." ";" _req_parts "${_requested_version}")
            list(LENGTH _req_parts _req_len)
            if(_req_len EQUAL 3)
                math(EXPR _score "${_score} + 300000000")
            elseif(_req_len EQUAL 2)
                math(EXPR _score "${_score} + 200000000")
            elseif(_req_len EQUAL 1)
                math(EXPR _score "${_score} + 100000000")
            endif()
        endif()

        if(NOT _requested_platform)
            _qt_find_compatibility_score(
                    "${_manifest}"
                    "${_build_platform}"
                    "${_build_compiler}"
                    "${_build_arch}"
                    _compat_score
            )
            math(EXPR _score "${_score} + ${_compat_score}")
        endif()

        _qt_find_log("${_verbose}" "STATUS" "Accept ${_kit_id}: version=${_version}, score=${_score}")

        if(_score GREATER _best_score)
            set(_best_score "${_score}")
            set(_best_id "${_rid}")
        endif()
    endforeach()

    if(_best_id STREQUAL "")
        if(QTF_REQUIRED)
            message(FATAL_ERROR
                    "[qt_find] No suitable Qt installation found "
                    "(PATH='${QTF_PATH}', VERSION='${_requested_version}', PLATFORM='${_requested_platform}')"
            )
        else()
            _qt_find_log("${_verbose}" "WARNING"
                    "No suitable Qt installation found (PATH='${QTF_PATH}', VERSION='${_requested_version}', PLATFORM='${_requested_platform}')"
            )
            set(QT_FIND_FOUND FALSE PARENT_SCOPE)
            return()
        endif()
    endif()

    set(_prefix "${QT_FIND_DISCOVERY_PREFIX}")
    set(_sel_version     "${${_prefix}_${_best_id}_VERSION}")
    set(_sel_major       "${${_prefix}_${_best_id}_MAJOR}")
    set(_sel_id          "${${_prefix}_${_best_id}_ID}")
    set(_sel_qmake       "${${_prefix}_${_best_id}_QMAKE}")
    set(_sel_prefix      "${${_prefix}_${_best_id}_PREFIX}")
    set(_sel_cmake_dir   "${${_prefix}_${_best_id}_CMAKE_DIR}")
    set(_sel_package_dir "${${_prefix}_${_best_id}_PACKAGE_DIR}")
    set(_sel_spec        "${${_prefix}_${_best_id}_SPEC}")
    set(_sel_xspec       "${${_prefix}_${_best_id}_XSPEC}")
    set(_sel_components  "${${_prefix}_${_best_id}_COMPONENTS}")

    _qt_find_log("${_verbose}" "STATUS" "Selected kit       : ${_sel_id}")
    _qt_find_log("${_verbose}" "STATUS" "Selected version   : ${_sel_version}")
    _qt_find_log("${_verbose}" "STATUS" "Selected qmake     : ${_sel_qmake}")
    _qt_find_log("${_verbose}" "STATUS" "Selected prefix    : ${_sel_prefix}")
    _qt_find_log("${_verbose}" "STATUS" "Selected package   : ${_sel_package_dir}")

    set(_qt_pkg "Qt${_sel_major}")

    list(PREPEND CMAKE_PREFIX_PATH "${_sel_prefix}" "${_sel_cmake_dir}" "${_sel_package_dir}")
    list(REMOVE_DUPLICATES CMAKE_PREFIX_PATH)
    set(${_qt_pkg}_DIR "${_sel_package_dir}")

    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24")
        if(QTF_REQUIRED)
            find_package(${_qt_pkg} CONFIG REQUIRED GLOBAL COMPONENTS ${_requested_components})
        else()
            find_package(${_qt_pkg} CONFIG QUIET GLOBAL COMPONENTS ${_requested_components})
        endif()
    else()
        if(QTF_REQUIRED)
            find_package(${_qt_pkg} CONFIG REQUIRED COMPONENTS ${_requested_components})
        else()
            find_package(${_qt_pkg} CONFIG QUIET COMPONENTS ${_requested_components})
        endif()
    endif()
    set(_found_components)
    set(_missing_components)
    set(_targets)

    foreach(_comp IN LISTS _requested_components)
        if(TARGET ${_qt_pkg}::${_comp})
            list(APPEND _found_components "${_comp}")
            list(APPEND _targets "${_qt_pkg}::${_comp}")
        else()
            list(APPEND _missing_components "${_comp}")
        endif()
    endforeach()

    list(REMOVE_DUPLICATES _found_components)
    list(REMOVE_DUPLICATES _missing_components)
    list(REMOVE_DUPLICATES _targets)

    set(_ok TRUE)
    if(_requested_components AND _missing_components)
        set(_ok FALSE)
    endif()

    if(QTF_REQUIRED AND NOT _ok)
        message(FATAL_ERROR
                "[qt_find] Qt kit was found (${_sel_id}, ${_sel_version}), but some components are missing: ${_missing_components}"
        )
    endif()

    if((NOT QTF_REQUIRED) AND (NOT _ok))
        _qt_find_log("${_verbose}" "WARNING"
                "Qt kit was found (${_sel_id}, ${_sel_version}), but some components are missing: ${_missing_components}"
        )
    endif()

    set(QT_FIND_FOUND               "${_ok}"                PARENT_SCOPE)
    set(QT_FIND_VERSION             "${_sel_version}"       PARENT_SCOPE)
    set(QT_FIND_MAJOR               "${_sel_major}"         PARENT_SCOPE)
    set(QT_FIND_QMAKE               "${_sel_qmake}"         PARENT_SCOPE)
    set(QT_FIND_PREFIX              "${_sel_prefix}"        PARENT_SCOPE)
    set(QT_FIND_CMAKE_DIR           "${_sel_cmake_dir}"     PARENT_SCOPE)
    set(QT_FIND_PACKAGE_DIR         "${_sel_package_dir}"   PARENT_SCOPE)
    set(QT_FIND_KIT                 "${_sel_id}"            PARENT_SCOPE)
    set(QT_FIND_SPEC                "${_sel_spec}"          PARENT_SCOPE)
    set(QT_FIND_XSPEC               "${_sel_xspec}"         PARENT_SCOPE)
    set(QT_FIND_KIT_COMPONENTS      "${_sel_components}"    PARENT_SCOPE)
    set(QT_FIND_FOUND_COMPONENTS    "${_found_components}"  PARENT_SCOPE)
    set(QT_FIND_MISSING_COMPONENTS  "${_missing_components}" PARENT_SCOPE)
    set(QT_FIND_TARGETS             "${_targets}"           PARENT_SCOPE)

    set(_sel_bin_dir "")
    if(EXISTS "${_sel_prefix}/bin")
        set(_sel_bin_dir "${_sel_prefix}/bin")
    elseif(_sel_qmake)
        get_filename_component(_sel_bin_dir "${_sel_qmake}" DIRECTORY)
    endif()

    set(_sel_windeployqt "")
    if(WIN32)
        if(EXISTS "${_sel_bin_dir}/windeployqt.exe")
            set(_sel_windeployqt "${_sel_bin_dir}/windeployqt.exe")
        endif()
    endif()

    set(QT_FIND_BIN_DIR       "${_sel_bin_dir}"       PARENT_SCOPE)
    set(QT_FIND_WINDEPLOYQT   "${_sel_windeployqt}"   PARENT_SCOPE)

    list(PREPEND CMAKE_PREFIX_PATH "${_sel_cmake_dir}" "${_sel_prefix}")
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" PARENT_SCOPE)

    # Пробрасываем все QT_* переменные в родительский скоуп.
    # find_package(Qt6) определяет служебные переменные типа QT_KNOWN_POLICY_QTP0005,
    # которые нужны qt_add_qml_module — без этого они теряются при выходе из функции.
    get_cmake_property(_qt_find_all_vars VARIABLES)
    foreach(_qt_find_var IN LISTS _qt_find_all_vars)
        if(_qt_find_var MATCHES "^QT_")
            set(${_qt_find_var} "${${_qt_find_var}}" PARENT_SCOPE)
        endif()
    endforeach()

endfunction()



function(qt_windeploy QWD_TARGET)
    set(_opt VERBOSE QML NO_TRANSLATIONS NO_COMPILER_RUNTIME NO_SYSTEM_D3D_COMPILER NO_OPENGL_SW)
    set(_one )
    cmake_parse_arguments(QWD "${_opt}" "${_one}" "" ${ARGN})

    if(NOT QWD_TARGET)
        message(FATAL_ERROR "[qt_windeploy] TARGET is required")
    endif()

    if(NOT TARGET ${QWD_TARGET})
        message(FATAL_ERROR "[qt_windeploy] Target '${QWD_TARGET}' does not exist")
    endif()

    if(NOT WIN32)
        if(QWD_VERBOSE)
            message(STATUS "[qt_windeploy] Non-Windows platform, skipping for target ${QWD_TARGET}")
        endif()
        return()
    endif()

    get_target_property(_target_type ${QWD_TARGET} TYPE)
    if(NOT _target_type STREQUAL "EXECUTABLE")
        if(QWD_VERBOSE)
            message(STATUS "[qt_windeploy] Target '${QWD_TARGET}' has type '${_target_type}', skipping")
        endif()
        return()
    endif()

    set(_windeployqt "${QT_FIND_WINDEPLOYQT}")

    if(NOT _windeployqt AND QT_FIND_QMAKE)
        get_filename_component(_qt_bin_dir "${QT_FIND_QMAKE}" DIRECTORY)
        if(EXISTS "${_qt_bin_dir}/windeployqt.exe")
            set(_windeployqt "${_qt_bin_dir}/windeployqt.exe")
        endif()
    endif()

    if(NOT _windeployqt AND QT_FIND_BIN_DIR)
        if(EXISTS "${QT_FIND_BIN_DIR}/windeployqt.exe")
            set(_windeployqt "${QT_FIND_BIN_DIR}/windeployqt.exe")
        endif()
    endif()

    if(NOT _windeployqt)
        find_program(_windeployqt
                NAMES windeployqt windeployqt.exe
                HINTS "${QT_FIND_BIN_DIR}"
        )
    endif()

    if(NOT _windeployqt)
        message(WARNING "[qt_windeploy] windeployqt not found for target '${QWD_TARGET}'")
        return()
    endif()

    # Папка для вспомогательных скриптов
    set(_qt_deploy_dir "${CMAKE_CURRENT_BINARY_DIR}/qt_deploy")
    file(MAKE_DIRECTORY "${_qt_deploy_dir}")

    set(_runner_script "${_qt_deploy_dir}/qt_windeploy_runner_${QWD_TARGET}.cmake")

    set(_extra_args "")
    if(QWD_QML)
        string(APPEND _extra_args "list(APPEND ARGS \"--qmldir\" [=[${CMAKE_SOURCE_DIR}]=])\n")
    endif()
    if(QWD_NO_TRANSLATIONS)
        string(APPEND _extra_args "list(APPEND ARGS \"--no-translations\")\n")
    endif()
    if(QWD_NO_COMPILER_RUNTIME)
        string(APPEND _extra_args "list(APPEND ARGS \"--no-compiler-runtime\")\n")
    endif()
    if(QWD_NO_SYSTEM_D3D_COMPILER)
        string(APPEND _extra_args "list(APPEND ARGS \"--no-system-d3d-compiler\")\n")
    endif()
    if(QWD_NO_OPENGL_SW)
        string(APPEND _extra_args "list(APPEND ARGS \"--no-opengl-sw\")\n")
    endif()

    set(_verbose_block "")
    if(QWD_VERBOSE)
        string(APPEND _verbose_block
                "message(STATUS \"[qt_windeploy] config      = \${CONFIG}\")\n"
                "message(STATUS \"[qt_windeploy] executable  = \${TARGET_FILE}\")\n"
                "message(STATUS \"[qt_windeploy] windeployqt = \${WINDEPLOYQT}\")\n"
                "message(STATUS \"[qt_windeploy] args        = \${ARGS}\")\n")
    endif()

    file(WRITE "${_runner_script}" [=[
if(NOT DEFINED WINDEPLOYQT)
    message(FATAL_ERROR "[qt_windeploy] WINDEPLOYQT is not defined")
endif()

if(NOT DEFINED TARGET_FILE)
    message(FATAL_ERROR "[qt_windeploy] TARGET_FILE is not defined")
endif()

if(NOT DEFINED CONFIG)
    set(CONFIG "")
endif()

if(NOT EXISTS "${WINDEPLOYQT}")
    message(FATAL_ERROR "[qt_windeploy] windeployqt not found: ${WINDEPLOYQT}")
endif()

if(NOT EXISTS "${TARGET_FILE}")
    message(FATAL_ERROR "[qt_windeploy] target file does not exist: ${TARGET_FILE}")
endif()

string(TOLOWER "${CONFIG}" CONFIG_L)

set(ARGS)
if(CONFIG_L STREQUAL "debug")
    list(APPEND ARGS "--debug")
else()
    list(APPEND ARGS "--release")
endif()

# extra args injected here
]=])

    file(APPEND "${_runner_script}" "${_extra_args}")
    file(APPEND "${_runner_script}" [=[
list(APPEND ARGS "${TARGET_FILE}")
]=])
    file(APPEND "${_runner_script}" "${_verbose_block}")
    file(APPEND "${_runner_script}" [=[
execute_process(
    COMMAND "${WINDEPLOYQT}" ${ARGS}
    RESULT_VARIABLE rc
    OUTPUT_VARIABLE out
    ERROR_VARIABLE err
)

if(NOT rc EQUAL 0)
    message(FATAL_ERROR
        "[qt_windeploy] windeployqt failed with code ${rc}\n"
        "stdout:\n${out}\n"
        "stderr:\n${err}\n"
    )
endif()
]=])

    if(QWD_VERBOSE)
        message(STATUS "[qt_windeploy] Target       : ${QWD_TARGET}")
        message(STATUS "[qt_windeploy] Target type  : ${_target_type}")
        message(STATUS "[qt_windeploy] windeployqt  : ${_windeployqt}")
        message(STATUS "[qt_windeploy] Runner script: ${_runner_script}")
    endif()

    add_custom_command(TARGET ${QWD_TARGET} POST_BUILD
            COMMAND "${CMAKE_COMMAND}"
            "-DWINDEPLOYQT=${_windeployqt}"
            "-DTARGET_FILE=$<TARGET_FILE:${QWD_TARGET}>"
            "-DCONFIG=$<CONFIG>"
            -P "${_runner_script}"
            COMMENT "Running windeployqt for ${QWD_TARGET}"
            VERBATIM
    )
endfunction()