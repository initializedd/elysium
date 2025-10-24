#define GLFW_INCLUDE_NONE

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <iostream>

int main() {
    vk::detail::DispatchLoaderDynamic default_dispatch_loader_dynamic;
    default_dispatch_loader_dynamic.init();

    vk::ApplicationInfo app_info{};
    app_info.sType = vk::StructureType::eApplicationInfo;
    app_info.pApplicationName = "Editor";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "elysium";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    vk::InstanceCreateInfo create_info{};
    create_info.sType = vk::StructureType::eInstanceCreateInfo;
    create_info.pApplicationInfo = &app_info;

    vk::Instance instance;
    if (vk::createInstance(&create_info, nullptr, &instance) != vk::Result::eSuccess) {
        std::cout << "Failed to create Vulkan instance.\n";
        return -1;
    }

    default_dispatch_loader_dynamic.init(instance);

    std::vector<vk::PhysicalDevice> physical_devices = instance.enumeratePhysicalDevices();
    assert(!physical_devices.empty());

    vk::Device device = physical_devices[0].createDevice({}, nullptr);
    default_dispatch_loader_dynamic.init(device);

    if (!glfwInit()) {
        std::cout << "Failed to init glfw.\n";
        return -1;
    }

    if (glfwPlatformSupported(GLFW_PLATFORM_WAYLAND)) {
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    } else {
        std::cout << "Platform does not support Wayland.\n";
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Editor", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create window.\n";
        return -1;
    }

    glfwMakeContextCurrent(window);

    while(!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

