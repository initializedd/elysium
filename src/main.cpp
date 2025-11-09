#define GLFW_INCLUDE_NONE

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>

#include "systems/sound_system.hpp"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

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
        std::cout << "Failed to create Vulkan instance\n";
        return -1;
    }

    default_dispatch_loader_dynamic.init(instance);

    std::vector<vk::PhysicalDevice> physical_devices = instance.enumeratePhysicalDevices();
    assert(!physical_devices.empty());

    vk::Device device = physical_devices[0].createDevice({}, nullptr);
    default_dispatch_loader_dynamic.init(device);

    using enum elysium::system_t;
    elysium::system<sound> sound_system{};

    sound_system.load_sound("boing", "assets/sounds/boing.wav");
    sound_system.adjust_sound_volume("boing", 0.05f);

    std::jthread sound_thread{[&](std::stop_token stop_token) {
        sound_system.event_loop(stop_token);
    }};

    sound_system.play_sound("boing");

    if (!glfwInit()) {
        std::cout << "Failed to init glfw\n";
        return -1;
    }

    if (glfwPlatformSupported(GLFW_PLATFORM_WAYLAND)) {
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    } else {
        std::cout << "Platform does not support Wayland\n";
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Editor", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create window\n";
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    while(!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (!sound_thread.request_stop()) {
        std::cout << "Failed to stop the sound thread\n";
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

