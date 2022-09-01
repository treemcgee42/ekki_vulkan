#include "gui.hpp"

static void check_vk_result(VkResult err) {
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}

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

ImDrawData* EkGui::get_imgui_draw_data(bool *show_demo_window, bool *show_another_window, ImVec4 *clear_color) {
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

    ImGui::Render();
    return ImGui::GetDrawData();
}
