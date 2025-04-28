#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

bool running = true;
bool showAbout = false;
bool showDemo = false;

void renderToolbar() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Quit", "Ctrl+Q")) {
        running = false;
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("Show Demo Window (Test)")) {
        showDemo = !showDemo;
      }
      if (ImGui::MenuItem("About")) {
        showAbout = true;
      }
      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Q)) {
    running = false;
  }
}

void renderDemo() {
  if (showDemo) {
    ImGui::ShowDemoWindow(&showDemo);
  }
}

void renderAbout() {
  if (showAbout) {
    if (ImGui::Begin("About", &showAbout, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::Text("MultiEmu Rewrite");
      ImGui::Text("A multi-system emulator framework");
      if (ImGui::Button("GitHub")) {
        OpenURL("https://github.com/atirut-w/multiemu");
      }
    }
    ImGui::End();
  }
}

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(1280, 720, "MultiEmu");
  SetTargetFPS(60);
  rlImGuiSetup(true);

  while (!WindowShouldClose() && running) {
    BeginDrawing();
    ClearBackground(GRAY);

    rlImGuiBegin();
    renderToolbar();
    renderDemo();
    renderAbout();
    rlImGuiEnd();

    EndDrawing();
  }

  rlImGuiShutdown();
  CloseWindow(); // Close window and OpenGL context
}
