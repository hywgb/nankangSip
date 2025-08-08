#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>
#include <atomic>

namespace {
std::atomic<bool> g_running{true};
void on_signal(int) { g_running = false; }
}

int main() {
    std::signal(SIGINT, on_signal);
    std::signal(SIGTERM, on_signal);

    std::cout << "[ITACATI] server starting (stub)" << std::endl;
    std::cout << "- C++17 runtime OK" << std::endl;
    std::cout << "- TODO: init config, logger, metrics, pjsip, webrtc-bridge, api server" << std::endl;

    // 占位主循环（后续替换为 io_context/dispatcher）
    while (g_running.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::cout << "[ITACATI] server stopping" << std::endl;
    return 0;
}