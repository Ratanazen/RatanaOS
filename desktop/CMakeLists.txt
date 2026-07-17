add_executable(ratana-desktop-shell
  src/main.cpp
)

target_link_libraries(ratana-desktop-shell PRIVATE ratana_ui Qt6::Widgets)
