add_library(ratana_ui STATIC
  common/ratana_ui.cpp
  common/customization_api.cpp
)

target_include_directories(ratana_ui PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/common
)

target_link_libraries(ratana_ui PUBLIC Qt6::Widgets Qt6::Sql)

function(ratana_add_qt_app target source_file)
  add_executable(${target} ${source_file})
  target_link_libraries(${target} PRIVATE ratana_ui Qt6::Widgets)
endfunction()

ratana_add_qt_app(ratana-settings settings/main.cpp)
ratana_add_qt_app(ratana-software-center "software-center/main.cpp;software-center/package_manager.cpp")
ratana_add_qt_app(ratana-system-monitor system-monitor/main.cpp)
ratana_add_qt_app(ratana-update-manager update-manager/main.cpp)
ratana_add_qt_app(ratana-control-center control-center/main.cpp)
ratana_add_qt_app(ratana-installer ../installer/main.cpp)
ratana_add_qt_app(ratana-ai-assistant "ai-assistant/main.cpp;ai-assistant/ai_engine.cpp")
ratana_add_qt_app(ratana-terminal terminal/main.cpp)
ratana_add_qt_app(ratana-file-manager file-manager/main.cpp)
ratana_add_qt_app(ratana-welcome welcome/main.cpp)
ratana_add_qt_app(ratana-recovery ratana-recovery/main.cpp)
ratana_add_qt_app(ratana-profile-manager profile-manager/main.cpp)
ratana_add_qt_app(ratana-login-manager login-manager/main.cpp)
ratana_add_qt_app(ratana-appearance-center appearance-center/main.cpp)
ratana_add_qt_app(ratana-branding-tool branding-tool/main.cpp)
if(TARGET PkgConfig::GTK4)
  add_executable(ratana-gtk-bridge common/gtk_bridge.c)
  target_link_libraries(ratana-gtk-bridge PRIVATE PkgConfig::GTK4)
endif()
