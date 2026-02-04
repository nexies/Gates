cmake_minimum_required(VERSION 4.1)
project(Gates)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "S:/Qt2/6.10.2/mingw_64")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Qml
        Quick
        QuickWidgets
        REQUIRED)

add_executable(Gates main.cpp

        cpp/Core/GatesFrameDispatcher.cpp
        cpp/Core/GatesFrameDispatcher.h

        cpp/CustomQmlTypes/DesktopService.cpp
        cpp/CustomQmlTypes/DesktopService.h
        cpp/CustomQmlTypes/DirEntryModel.cpp
        cpp/CustomQmlTypes/DirEntryModel.h
        cpp/CustomQmlTypes/FileSystemModel.cpp
        cpp/CustomQmlTypes/FileSystemModel.h
        cpp/CustomQmlTypes/GatesFrameState.cpp
        cpp/CustomQmlTypes/GatesFrameState.h
        cpp/CustomQmlTypes/IconProvider.cpp
        cpp/CustomQmlTypes/IconProvider.h
        cpp/CustomQmlTypes/ResizableFramelessWindow.cpp
        cpp/CustomQmlTypes/ResizableFramelessWindow.h
        cpp/CustomQmlTypes/RoundedRect.cpp
        cpp/CustomQmlTypes/RoundedRect.h


        cpp/FileSystem/FileSystemEntry.cpp
        cpp/FileSystem/FileSystemEntry.h

        cpp/Foreign/GatesFrameForeign.cpp
        cpp/Foreign/GatesFrameForeign.h

        cpp/Frames/Frame.cpp
        cpp/Frames/Frame.hpp

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
)

qt_add_resources(Gates qml_res
    PREFIX "/"
    FILES
        qml/BlurShader.qml
        qml/CustomButton.qml
        qml/Frame.qml
        qml/FrameNameBar.qml
        qml/GatesFrame.qml
        qml/GatesGridViewDelegate.qml
        qml/IconGridView.qml
        qml/Main.qml
        qml/NameBarCustomButton.qml
        qml/testFrame.qml
)

qt_add_resources(Gates app_res
    FILES

)

target_link_libraries(Gates
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Qml
        Qt::Quick
        Qt::QuickWidgets
)

target_include_directories(Gates PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

include(cmake/gates-deploy.cmake)
