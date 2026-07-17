add_library(ratana_ui STATIC
  common/ratana_ui.cpp
)

target_include_directories(ratana_ui PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/common
)

target_link_libraries(ratana_ui PUBLIC Qt6::Widgets)

function(ratana_add_qt_app target source_file)
  add_executable(${target} ${source_file})
  target_link_libraries(${target} PRIVATE ratana_ui Qt6::Widgets)
endfunction()

ratana_add_qt_app(ratana-settings settings/main.cpp)
ratana_add_qt_app(ratana-software-center software-center/main.cpp)
ratana_add_qt_app(ratana-system-monitor system-monitor/main.cpp)
ratana_add_qt_app(ratana-update-manager update-manager/main.cpp)
ratana_add_qt_app(ratana-control-center control-center/main.cpp)
ratana_add_qt_app(ratana-ai-assistant ai-assistant/main.cpp)

if(TARGET PkgConfig::GTK4)
  add_executable(ratana-gtk-bridge common/gtk_bridge.c)
  target_link_libraries(ratana-gtk-bridge PRIVATE PkgConfig::GTK4)
endif()
