#pragma once
#include <atomic>
#include <functional>
#include <string>
#include <thread>
#include <unordered_map>

namespace itacati::api {

struct HttpRequest {
    std::string method; // GET/POST
    std::string path;   // /api/health
    std::string body;
};

struct HttpResponse {
    int         status{200};
    std::string content_type{"application/json"};
    std::string body{"{}"};
};

using Handler = std::function<HttpResponse(const HttpRequest&)>;

class HttpServer {
public:
    bool start(const std::string& bind_ip, int port);
    void stop();

    void handle(const std::string& method, const std::string& path, Handler h);

private:
    std::atomic<bool> running_{false};
    std::thread       loop_;
    std::string       bind_ip_;
    int               port_{0};

    // method:path -> handler
    std::unordered_map<std::string, Handler> routes_;

    void run_loop();
};

} // namespace itacati::api