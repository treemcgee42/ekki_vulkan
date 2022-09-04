#include "gui.hpp"
#include <iostream>

static void check_vk_result(VkResult err) {
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}

// -------------------------------------------------------------------------------------------
// ----- Initialization helpers --------------------------------------------------------------
// -------------------------------------------------------------------------------------------

void EkGui::setup_imgui_context() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)*io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
}

void EkGui::setup_imgui_style() {
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
}

void EkGui::setup_imgui_backend() {
    ImGui_ImplGlfw_InitForVulkan(engine.vkbe_window.window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = engine.vkbe_device.get_instance();
    init_info.PhysicalDevice = engine.vkbe_device.get_physical_device();
    init_info.Device = engine.vkbe_device.get_logical_device();
    init_info.QueueFamily = engine.vkbe_device.queue_family_info.graphics_queue_family_index;
    init_info.Queue = engine.vkbe_device.get_graphics_queue();
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = engine.vkbe_device.get_descriptor_pool();
    init_info.Subpass = 0;
    init_info.MinImageCount = engine.vkbe_renderer.get_min_swap_chain_image_count();
    init_info.ImageCount = engine.vkbe_renderer.get_swap_chain_image_count();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = VK_NULL_HANDLE;
    init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, engine.vkbe_renderer.get_imgui_render_pass());
}

void EkGui::load_imgui_fonts() {
    // Use any command queue
    VkCommandPool command_pool = engine.vkbe_device.get_command_pool();
    VkCommandBuffer command_buffer = engine.vkbe_renderer.get_command_buffer(0);

    vkbe::vkbe_check_vk_result_panic(vkResetCommandPool(engine.vkbe_device.get_logical_device(), command_pool, 0), "failed to reset command pool!");

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkbe::vkbe_check_vk_result_panic(vkBeginCommandBuffer(command_buffer, &begin_info), "failed to begin command buffer to upload fonts!");

    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

    VkSubmitInfo end_info = {};
    end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    end_info.commandBufferCount = 1;
    end_info.pCommandBuffers = &command_buffer;
    vkbe::vkbe_check_vk_result_panic(vkEndCommandBuffer(command_buffer), "failed to end command buffer to upload fonts!");

    vkbe::vkbe_check_vk_result_panic(vkQueueSubmit(engine.vkbe_device.get_graphics_queue(), 1, &end_info, VK_NULL_HANDLE), "failed to submit upload fonts to queue!");

    vkbe::vkbe_check_vk_result_panic(vkDeviceWaitIdle(engine.vkbe_device.get_logical_device()), "failed to wait idly for upload fonts!");

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

// -------------------------------------------------------------------------------------------
// ----- Window creations --------------------------------------------------------------------
// -------------------------------------------------------------------------------------------

void EkGui::create_object_list_window(eklib::Scene& scene) {
    ImGui::Begin("objects");

    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

    ImGui::CheckboxFlags("ImGuiTableFlags_ScrollY", &flags, ImGuiTableFlags_ScrollY);

//    static int selected = -1;
//    for (int n = 0; n < 5; n++)
//    {
//        char buf[32];
//        sprintf(buf, "Object %d", n);
//        if (ImGui::Selectable(buf, selected == n))
//            selected = n;
//    }


    // When using ScrollX or ScrollY we need to specify a size for our table container!
    // Otherwise by default the table will fit all available space, like a BeginChild() call.
    ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 8);
    if (ImGui::BeginTable("table_scrolly", 1, flags, outer_size))
    {


        // Demonstrate using clipper for large vertical lists
        ImGuiListClipper clipper;
        clipper.Begin(scene.get_num_active_objects());
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                auto active_object_id = row;
                const bool item_is_selected = (active_object_id == selected_object_index_in_active_objects);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                auto label = scene.get_active_object_name_by_index(static_cast<uint32_t>(row)).c_str();
                if (ImGui::Selectable(label, item_is_selected)) {
                    std::cout << "object " << active_object_id << " is selected\n";
                    selected_object_index_in_active_objects = active_object_id;
                }

            }
        }
        ImGui::EndTable();
    }

    ImGui::End();
}

void EkGui::create_object_editor_window(eklib::Scene& scene) {
    ImGui::Begin("object editor");

    if (selected_object_index_in_active_objects < 0) {
        ImGui::Text("no selected object");
    } else {
        auto active_object = scene.get_active_object_by_index(selected_object_index_in_active_objects);

        obj_editor_scale = active_object->getScale();
        auto active_object_position = active_object->getTranslation();
        obj_editor_position2[0] = active_object_position[0];
        obj_editor_position2[1] = active_object_position[1];
        auto active_object_color = active_object->get_color();
        obj_editor_color4[0] = active_object_color[0];
        obj_editor_color4[1] = active_object_color[1];
        obj_editor_color4[2] = active_object_color[2];
        obj_editor_color4[3] = active_object_color[3];

        ImGui::Text("name: %s", scene.get_active_object_name_by_index(selected_object_index_in_active_objects).c_str());
        ImGui::InputFloat("scale", &obj_editor_scale, 0.1f);
        ImGui::InputFloat2("position", obj_editor_position2);
        ImGui::ColorEdit4("color", obj_editor_color4);

        active_object->scale_absolute(obj_editor_scale);
        active_object->translate_absolute(obj_editor_position2[0], obj_editor_position2[1]);
        active_object->set_color(obj_editor_color4[0], obj_editor_color4[1], obj_editor_color4[2], obj_editor_color4[3]);
    }

    ImGui::End();
}

// -------------------------------------------------------------------------------------------
// ----- Public functions --------------------------------------------------------------------
// -------------------------------------------------------------------------------------------

EkGui::EkGui(eklib::Engine& engine_): engine{engine_} {
    setup_imgui_context();
    setup_imgui_style();
    setup_imgui_backend();
    load_imgui_fonts();
}

EkGui::~EkGui() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

ImDrawData* EkGui::get_imgui_draw_data(bool *show_demo_window, bool *show_another_window, ImVec4 *clear_color, eklib::Scene& scene) {
    // Start the Dear ImGui frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (*show_demo_window)
        ImGui::ShowDemoWindow(show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            *show_another_window = false;
        ImGui::End();
    }

    create_object_list_window(scene);
    create_object_editor_window(scene);

    ImGui::Render();
    return ImGui::GetDrawData();
}
