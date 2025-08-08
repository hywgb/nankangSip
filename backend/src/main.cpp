#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>
#include <atomic>
#include <cstdlib>

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
    static itacati::core::CallManager cm;
    http.handle("POST", "/call/make", [](const itacati::api::HttpRequest& req){
        std::string dst="";
        auto p = req.body.find("\"dst\"");
        if (p!=std::string::npos){ auto q=req.body.find(':',p); auto s=req.body.find('"',q+1); auto e=req.body.find('"',s+1); if (s!=std::string::npos&&e!=std::string::npos) dst=req.body.substr(s+1,e-s-1);}        
        auto id = cm.make_call(dst.empty()?"10086":dst);
        std::string body = std::string("{\"callId\":\"")+id+"\"}";
        return itacati::api::HttpResponse{200, "application/json", body};
    });
    http.handle("POST", "/call/hangup", [](const itacati::api::HttpRequest& req){
        std::string id=""; auto p=req.body.find("callId"); if (p!=std::string::npos){auto q=req.body.find(':',p); auto s=req.body.find('"',q+1); auto e=req.body.find('"',s+1); if(s!=std::string::npos&&e!=std::string::npos) id=req.body.substr(s+1,e-s-1);}        
        bool ok = cm.hangup(id);
        return itacati::api::HttpResponse{200, "application/json", ok?"{\"ok\":true}":"{\"ok\":false}"};
    });
    http.handle("POST", "/call/hold", [](const itacati::api::HttpRequest& req){
        std::string id=""; auto p=req.body.find("callId"); if (p!=std::string::npos){auto q=req.body.find(':',p); auto s=req.body.find('"',q+1); auto e=req.body.find('"',s+1); if(s!=std::string::npos&&e!=std::string::npos) id=req.body.substr(s+1,e-s-1);}        
        bool ok = cm.hold(id);
        return itacati::api::HttpResponse{200, "application/json", ok?"{\"ok\":true}":"{\"ok\":false}"};
    });
    http.handle("POST", "/call/unhold", [](const itacati::api::HttpRequest& req){
        std::string id=""; auto p=req.body.find("callId"); if (p!=std::string::npos){auto q=req.body.find(':',p); auto s=req.body.find('"',q+1); auto e=req.body.find('"',s+1); if(s!=std::string::npos&&e!=std::string::npos) id=req.body.substr(s+1,e-s-1);}        
        bool ok = cm.unhold(id);
        return itacati::api::HttpResponse{200, "application/json", ok?"{\"ok\":true}":"{\"ok\":false}"};
    });
    http.handle("POST", "/call/transfer", [](const itacati::api::HttpRequest& req){
        // demo 不实现咨询转，仅返回 ok
        return itacati::api::HttpResponse{200, "application/json", "{\"ok\":true}"};
    });
    http.handle("GET", "/events/poll", [](const itacati::api::HttpRequest& req){
        uint64_t since = 0; {
            auto sv = itacati::api::HttpServer::query(req.path, "since");
            if (!sv.empty()) since = std::strtoull(sv.c_str(), nullptr, 10);
        }
        uint32_t timeout = 0; {
            auto tv = itacati::api::HttpServer::query(req.path, "timeout");
            if (!tv.empty()) timeout = static_cast<uint32_t>(std::strtoul(tv.c_str(), nullptr, 10));
        }
        auto evs = (timeout>0) ? cm.wait_poll(since, timeout, 100) : cm.poll(since, 100);
        std::string out = "["; bool first=true; for (auto &e: evs){ if(!first) out+=","; first=false; out += "{\\\"id\\\":"+std::to_string(e.id)+",\\\"type\\\":\\\""+e.type+"\\\",\\\"callId\\\":\\\""+e.callId+"\\\",\\\"ts\\\":"+std::to_string(e.ts)+"}"; }
        out += "]";
        return itacati::api::HttpResponse{200, "application/json", out};
    });

    http.start("0.0.0.0", 8080);

    while (g_running.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    http.stop();
    std::cout << "[ITACATI] server stopping" << std::endl;
    return 0;
}