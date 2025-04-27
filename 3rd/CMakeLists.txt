add_subdirectory(raylib)

add_library(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_demo.cpp
)
target_include_directories(imgui PUBLIC imgui)

add_library(rlImGui STATIC
    rlImGui/rlImGui.cpp
)
target_include_directories(rlImGui PUBLIC rlImGui)
target_link_libraries(rlImGui PRIVATE imgui raylib)
