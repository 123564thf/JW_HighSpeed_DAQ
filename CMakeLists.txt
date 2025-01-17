cmake_minimum_required(VERSION 3.16)

project(root_qt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(${CMAKE_PREFIX_PATH} "${CMAKE_PREFIX_PATH};C:/Users/admin/Downloads/boost_1_86_0/stage-directory/lib/cmake")
#set(${CMAKE_PREFIX_PATH} "${CMAKE_PREFIX_PATH};D:/qt/6.8.0/Src/qtbase/cmake")

#set(ROOT_ROOT "C:/Users/admin/Downloads/root_install/root_install")

#set(Boost_NO_SYSTEM_PATHS ON)
#find_package(Boost COMPONENTS program_options REQUIRED )
#find_package(Boost COMPONENTS program_options REQUIRED)
#find_package(Boost REQUIRED)
find_package(ROOT REQUIRED)
find_program(ROOTCINT rootcint PATH_SUFFIXES bin REQUIRED)
#link_directories(${Boost_LIBRARY_DIRS})
message(STATUS ${PROJECT_SOURCE_DIR})
include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}
  ${ROOT_INCLUDE_DIRS}
  ${ROOT_INCLUDE_DIR}
  #$s{Boost_INCLUDE_DIR}
)
message(STATUS "WANGYING")
#find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS
#  Widgets
#Qt6WebEngineCore)
#find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core WebEngineCore WebEngineWidgets Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core WebEngineCore WebEngineWidgets Widgets)
#find_package(Qt6 COMPONENTS Core WebEngineCore WebEngineWidgets)
set(root_cint_out "root_cint_out.cpp")
set(data_struct "data_strcut_cint.h")
#execute_process(COMMAND "${ROOTCINT}" -I ${PROJECT_SOURCE_DIR} -f ${PROJECT_SOURCE_DIR}/${root_cint_out} -c ${data_struct} LinkDef.h)
#execute_process(COMMAND "${ROOTCINT}" -I ${PROJECT_SOURCE_DIR}  -f ${root_cint_out} -c ${data_struct} LinkDef.h)


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
                Client.h
                Client.cpp
                TCanvasWidget.h
                dialogmisc.h
                formdrawtrack.h
                DataAnalysis.h
                Welford.h
                dialogtrigger.h
                Draw.h
                data_struct_cint.h
                formdrawchannelwaves.h
                socketconfigurations.h
                LinkDef.h
                dialogbaseline.h
                formdraweventwaves.h
                unpack.h
                RCanvasWidget.h
                dialoghvcalculator.h
                formdrawseveralwaves.h
                wave_handler.h
                DataAnalysis.cpp
                Draw.cpp
                RCanvasWidget.cpp
                TCanvasWidget.cpp
                clusters.cpp
                 dialogbaseline.cpp
                 dialoghvcalculator.cpp
                 dialogmisc.cpp
                 dialogtrigger.cpp
                 formdrawchannelwaves.cpp
                 formdraweventwaves.cpp
                 formdrawseveralwaves.cpp
                 formdrawtrack.cpp main.cpp
                 socketconfigurations.cpp
                 unpack.cpp
                 util.cpp
                 dialogbaseline.ui
                 dialoghvcalculator.ui
                 dialogmisc.ui
                 dialogtrigger.ui
                 formdrawchannelwaves.ui
                 formdraweventwaves.ui
                 formdrawseveralwaves.ui
                 formdrawtrack.ui
                 socketconfigurations.ui

)

set(QT_VERSION_MAJOR 6)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(root_qt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
                ${root_cint_out}




    )
else()
    if(ANDROID)
        add_library(root_qt SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(root_qt
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(root_qt PRIVATE
  Qt6::Core
  Qt6::Widgets
  Qt6::WebEngineCore
  Qt6::WebEngineWidgets
  ROOT::ROOTWebDisplay
  ROOT::ROOTQt6WebDisplay
  ROOT::Gpad
  ROOT::WebGui6
  ROOT::ROOTGpadv7
  ROOT::ROOTCanvasPainter
  ROOT::Geom
  ROOT::ROOTBrowserv7
  ROOT::ROOTGeomViewer
)
target_link_libraries(root_qt PRIVATE ${ROOT_LIBRARIES})
target_link_libraries(root_qt PRIVATE ${Boost_LIBRARY_DIRS})

#if(${QT_VERSION} VERSION_LESS 6.1.0)
#  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.root_qt)
#endif()
set_target_properties(root_qt PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS root_qt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(root_qt)
endif()
