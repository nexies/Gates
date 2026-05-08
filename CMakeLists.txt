cmake_minimum_required(VERSION 4.1)
project(Gates)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(FindQt)

qt_find(
        VERBOSE
        NOCACHE
        VERSION 6
        COMPONENTS Core Gui Widgets Qml Quick QuickWidgets
        REQUIRED
        PLATFORM mingw 64
#        PATH "D:\\Qt2"
)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(Gates main.cpp

        cpp/Config/ConfigManager.cpp
        cpp/Config/ConfigManager.h

        cpp/Core/GatesFrameDispatcher.cpp
        cpp/Core/GatesFrameDispatcher.h

        cpp/Shell/DesktopShellManager.cpp
        cpp/Shell/DesktopShellManager.h

        cpp/Shell/WindowHelper.cpp
        cpp/Shell/WindowHelper.h

        cpp/Shell/DesktopLayer.cpp
        cpp/Shell/DesktopLayer.h

        cpp/Shell/DesktopIconModel.cpp
        cpp/Shell/DesktopIconModel.h

        cpp/Shell/ShellContextMenu.cpp
        cpp/Shell/ShellContextMenu.h

        cpp/CustomQmlTypes/DesktopService.cpp
        cpp/CustomQmlTypes/DesktopService.h
        cpp/CustomQmlTypes/DirEntryModel.cpp
        cpp/CustomQmlTypes/DirEntryModel.h
        cpp/CustomQmlTypes/IconProvider.cpp
        cpp/CustomQmlTypes/IconProvider.h
        cpp/CustomQmlTypes/DragDropService.cpp
        cpp/CustomQmlTypes/DragDropService.h
        cpp/CustomQmlTypes/SelectionService.cpp
        cpp/CustomQmlTypes/SelectionService.h

        cpp/FileSystem/FileSystemEntry.cpp
        cpp/FileSystem/FileSystemEntry.h

        cpp/Foreign/GatesFrameForeign.cpp
        cpp/Foreign/GatesFrameForeign.h

        cpp/utilities/BlurBehindHelper/BlurBehindHelper.cpp
        cpp/utilities/BlurBehindHelper/BlurBehindHelper.h

        cpp/utilities/common/iconprovider.h
        cpp/utilities/common/resizable.cpp
        cpp/utilities/common/resizable.h

        cpp/utilities/DesktopIconsHelper/DesktopIconsHelper.cpp
        cpp/utilities/DesktopIconsHelper/DesktopIconsHelper.h

        cpp/utilities/FileIconManager/FileIconProvider.cpp
        cpp/utilities/FileIconManager/FileIconProvider.h

        cpp/utilities/ResizableHelper/ResizableHelper.cpp
        cpp/utilities/ResizableHelper/ResizableHelper.h

        cpp/utilities/win/Functions.h
        cpp/utilities/win/shellqt.cpp
        cpp/utilities/win/shellqt.h
        cpp/utilities/win/WinStructs.h

        cpp/Widgets/defs.h
        cpp/Widgets/MainWindow.cpp
        cpp/Widgets/MainWindow.h

        QmlEngine.cpp
        QmlEngine.h

        resources.qrc
        cpp/Custom/qsettings_toml.cpp
        cpp/Custom/qsettings_toml.hpp
)

target_link_libraries(Gates PRIVATE
        ${QT_FIND_TARGETS}
        shlwapi     # ILClone, ILRemoveLastID, SHParseDisplayName
        comctl32    # SetWindowSubclass / DefSubclassProc
        ole32       # CoCreateInstance (IExplorerBrowser)
)

# Must be before qt_add_qml_module so qmltyperegistrar can find the headers
target_include_directories(Gates PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/CustomQmlTypes
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/Shell
)

# QML module: все QML-файлы и C++-типы, доступные из QML
qt_add_qml_module(Gates
    URI "Gates"
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    QML_FILES
        qml/BlurShader.qml
        qml/CustomButton.qml
        qml/Frame.qml
        qml/FrameNameBar.qml
        qml/GatesFrame.qml
        qml/IconDelegate.qml
        qml/IconGridView.qml
        qml/NameBarCustomButton.qml
        qml/testFrame.qml
        qml/VirtualDesktop.qml
        qml/DragGhost.qml
    SOURCES
        cpp/CustomQmlTypes/RoundedRect.cpp
        cpp/CustomQmlTypes/RoundedRect.h
        cpp/CustomQmlTypes/GatesFrameState.cpp
        cpp/CustomQmlTypes/GatesFrameState.h
        cpp/CustomQmlTypes/ResizableFramelessWindow.cpp
        cpp/CustomQmlTypes/ResizableFramelessWindow.h
        cpp/CustomQmlTypes/FileSystemModel.cpp
        cpp/CustomQmlTypes/FileSystemModel.h
        cpp/Shell/DesktopIconModel.cpp
        cpp/Shell/DesktopIconModel.h
        cpp/Shell/ShellFolderView.cpp
        cpp/Shell/ShellFolderView.h
)



#add_subdirectory(third_party/spdlog)

if(TARGET spdlog::spdlog)
    message(STATUS "-- Found SPDLOG!")
    target_compile_definitions(Gates PRIVATE GATES_USE_SPDLOG=1)
endif()

qt_windeploy(Gates QML)
