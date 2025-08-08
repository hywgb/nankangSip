#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>
#include <atomic>

#include "api/HttpServer.hpp"

namespace {
std::atomic<bool> g_running{true};
void on_signal(int) { g_running = false; }
}

int main() {
    std::signal(SIGINT, on_signal);
    std::signal(SIGTERM, on_signal);

    std::cout << "[ITACATI] server starting (stub)" << std::endl;
    std::cout << "- C++17 runtime OK" << std::endl;

    itacati::api::HttpServer http;
    http.handle("GET", "/api/health", [](const itacati::api::HttpRequest&){
        return itacati::api::HttpResponse{200, "application/json", "{\"status\":\"ok\"}"};
    });
    http.handle("POST", "/webrtc/offer", [](const itacati::api::HttpRequest& req){
        // TODO: 调用 webrtc::Bridge 生成 answer，这里先回显占位
        std::string body = req.body.empty()?"{}":req.body;
        return itacati::api::HttpResponse{200, "application/json",
            std::string("{\"type\":\"answer\",\"sdp\":\"") + "stub" + "\"}"};
    });
    http.handle("POST", "/call/make", [](const itacati::api::HttpRequest&){
        return itacati::api::HttpResponse{200, "application/json", "{\"ok\":true}"};
    });
    http.handle("POST", "/call/hangup", [](const itacati::api::HttpRequest&){
        return itacati::api::HttpResponse{200, "application/json", "{\"ok\":true}"};
    });
    http.handle("POST", "/call/hold", [](const itacati::api::HttpRequest&){
        return itacati::api::HttpResponse{200, "application/json", "{\"ok\":true}"};
    });
    http.handle("POST", "/call/unhold", [](const itacati::api::HttpRequest&){
        return itacati::api::HttpResponse{200, "application/json", "{\"ok\":true}"};
    });
    http.handle("POST", "/call/transfer", [](const itacati::api::HttpRequest&){
        return itacati::api::HttpResponse{200, "application/json", "{\"ok\":true}"};
    });

    http.start("0.0.0.0", 8080);

    while (g_running.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    http.stop();
    std::cout << "[ITACATI] server stopping" << std::endl;
    return 0;
}