#include "includes.h"
bool keybindingsVisible = false;
int keybindId = 0;

void keybindingsWindow() {
    Input* input = &inputs[keybindId];

    char windowTitle[128];
    sprintf_s(windowTitle, "Keybinds for %s", input->full_name);
    ImGui::Begin(windowTitle, NULL);

    for (size_t i = 0; i < input->keybinds.size(); i++) {
        InputKeybind* keybind = &input->keybinds[i];
        switch (keybind->type) {
        case InputKeybindType::CONTROLLER_BUTTON: {
            const char* key = SDL_GameControllerGetStringForButton(keybind->controllerButton);
            ImGui::Text("(Controller Button) %s", key);
            break;
        }
        case InputKeybindType::CONTROLLER_AXIS: {
            const char* key = SDL_GameControllerGetStringForAxis(keybind->controllerAxis);
            ImGui::Text("(Controller Axis) %s", key);
            ImGui::SameLine(); ImGui::Checkbox("Full Axis", &keybind->controllerAxisFull);
            ImGui::SameLine(); ImGui::Checkbox("Reverse Axis", &keybind->controllerAxisReversed);
            ImGui::SameLine(); ImGui::Checkbox("Reverse Axis (Half)", &keybind->controllerAxisReversedHalf);
            break;
        }
        case InputKeybindType::KEYBOARD: {
            const char* key = SDL_GetKeyName(keybind->keyboard);
            ImGui::Text("(Keyboard) %s", key);
            break;
        }
        }
        ImGui::SameLine();
        char id[128]; sprintf_s(id, "Delete###%d", i);
        if (ImGui::Button(id)) {
            input->keybinds.erase(input->keybinds.begin() + i);
            break;
        }
    }

    if (ImGui::Button("Close"))
        keybindingsVisible = false;
    ImGui::End();
}

void controlsItems() {
    if (input_ext_get_adding_keybind() != NULL) {
        ImGui::Text("Press any input to bind!");
        if (ImGui::Button("Cancel"))
            input_ext_set_adding_keybind(NULL);
    }
    else {
        for (size_t i = 0; i < inputs.size(); i++) {
            ImGui::Text("%s", inputs[i].full_name);
            char addId[128]; sprintf_s(addId, "Add###add%s", inputs[i].name);
            char editId[128]; sprintf_s(editId, "Edit###edit%s", inputs[i].name);

            if (ImGui::Button(addId))
                input_ext_set_adding_keybind(&inputs[i]);

            ImGui::SameLine();
            if (ImGui::Button(editId)) {
                keybindingsVisible = true;
                keybindId = i;
            }
        }
        ImGui::Dummy(ImVec2(0.0f, 0.5f)); // to make save config button less awkward
    }
}

void mainRenderer() {
    bool show_demo_window = false;
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    if (keybindingsVisible)
        keybindingsWindow();

	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
	ImGui::Begin("###window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);

    if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Controls"))
        {
            controlsItems();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    // show save config button everytime except for when binding
    if (input_ext_get_adding_keybind() == NULL) {
        if (ImGui::Button("Save Config")) {
            toml::table config = input_config_write();

            std::ofstream file;
            file.open("input.toml");
            if (file.is_open())
            {
                file << config;
                file.close();
            }
            else
            {
                printf("Error writing input.toml\n");
            }
        }
    }

	ImGui::End();

}