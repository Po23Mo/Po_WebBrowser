cmake_minimum_required(VERSION 3.5)

project(Po_WebBrowser VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets WebEngineWidgets WebEngineCore)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets WebEngineWidgets WebEngineCore)



set(PROJECT_SOURCES
        main.cpp
        useragent.h
        useragent.cpp
        securityutils.h
        securityutils.cpp
        addressbar.h
        addressbar.cpp
        urlcompletermodel.h
        urlcompletermodel.cpp
        urledit.h
        urledit.cpp
        securitystatusbutton.h
        securitystatusbutton.cpp
        securitystatusdialog.h
        securitystatusdialog.cpp
        cateogorydialog.h
        cateogorydialog.cpp
        bookmarkeditdialog.h
        bookmarkeditdialog.cpp
        bookmarkmanager.h
        bookmarkmanager.cpp
        bookmarktreewidget.h
        bookmarktreewidget.cpp
        bookmarkwidget.h
        bookmarkwidget.cpp
        notificationwidget.h
        notificationwidget.cpp
        powidget.h
        powidget.cpp
        browser.h
        webenginepage.h
        webenginepage.cpp
        profile.h
        profile.cpp
        webengineview.h
        webengineview.cpp
        webpagewidget.h
        webpagewidget.cpp
        downloadrequest.h
        downloadrequest.cpp
        downloadwidget.h
        downloadwidget.cpp
        pomainwindow.h
        pomainwindow.cpp
        powidget.h
        powidget.cpp
        dialog.h
        dialog.cpp
        version.h
        version.cpp
        urlschemehandler.h
        urlschemehandler.cpp
        tabbar.h
        tabbar.cpp
        tabdata.h
        tabdata.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Po_WebBrowser
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        PoWebBrowser.qrc
    )

else()
    if(ANDROID)
        add_library(Po_WebBrowser SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(Po_WebBrowser
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Po_WebBrowser
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE Qt${QT_VERSION_MAJOR}::WebEngineWidgets
    PRIVATE Qt${QT_VERSION_MAJOR}::WebEngineCore
)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Po_WebBrowser)
endif()
set_target_properties(Po_WebBrowser PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Po_WebBrowser
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Po_WebBrowser)
endif()
