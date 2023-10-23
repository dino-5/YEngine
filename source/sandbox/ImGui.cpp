#include "ImGui.h"
#include "graphics/GraphicsModule.h"
#include "graphics/Queue.h"
#include "system/Window.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_vulkan.h"

static void check_vk_result(VkResult err)
{
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}

void ImGuiManager::Release()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager::Initialize()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();
    graphics::GraphicsModule* module = graphics::GraphicsModule::GetInstance();

    ImGui_ImplGlfw_InitForVulkan(Window::GetWindow()->getWindow(), true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = VulkanInstance::GetInstance();
    init_info.PhysicalDevice = module->getDevice().getPhysicalDevice().getDevice();
    init_info.Device = module->getDevice().getLogicalDevice().getDevice();
	QueueFamilyIndices indices = FindQueueFamilies(module->getDevice().getPhysicalDevice().getDevice(), 
        module->getSurface().getSurface());
    init_info.QueueFamily = indices.graphicsFamily.value();
    init_info.Queue = module->getDevice().getLogicalDevice().getGraphicsQueue();
    init_info.PipelineCache = nullptr;
    init_info.DescriptorPool = module->getDescriptorPool().getDescriptorPool();
    init_info.Subpass = 0;
    init_info.MinImageCount = MAX_FRAMES_IN_FLIGHT;
    init_info.ImageCount = module->getSwapChain().getImageCount();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, module->getSwapChain().getRenderPass());

    // load fonts
    {
        // Use any command queue
		VkCommandPool command_pool = module->getCommandPool().getPool();
		CommandBuffer cmdBuffer;

        VkResult err = vkResetCommandPool(module->getDevice().getLogicalDevice().getDevice(), command_pool, 0);
        check_vk_result(err);

		cmdBuffer.initAsSingleTimeCmdBuffer();
        ImGui_ImplVulkan_CreateFontsTexture(cmdBuffer.getCmdBuffer(0));

		cmdBuffer.endSingleTimeCommands(0, module->getDevice().getLogicalDevice().getGraphicsQueue());
        err = vkDeviceWaitIdle(module->getDevice().getLogicalDevice().getDevice());
        check_vk_result(err);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
}

void ImGuiManager::StartFrame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::Draw(VkCommandBuffer& cmdBuffer)
{
    drawInternal();
    ImDrawData* draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data, cmdBuffer);
}

void ImGuiManager::AddImGuiEntry(ImGuiEntry entry)
{
    s_entries.push_back(entry);
}

void ImGuiManager::displayEntry(ImGuiEntry entry)
{
    if (entry.type == ImGuiType::FLOAT3)
        ImGui::SliderFloat3(entry.name, static_cast<float*>(entry.address), entry.min, entry.max);
    else
        throw std::runtime_error("unrecognized type");
}

void ImGuiManager::drawInternal()
{
	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
    for (auto entry : s_entries)
    {
        displayEntry(entry);
    }
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
	ImGui::End();
	ImGui::Render();
}
