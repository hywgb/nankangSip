#include "HttpServer.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <sstream>

namespace itacati::api {

static std::string key(const std::string& m, const std::string& p) {
    return m + ":" + p;
}

bool HttpServer::start(const std::string& bind_ip, int port) {
    if (running_) return true;
    bind_ip_ = bind_ip; port_ = port; running_ = true;
    loop_ = std::thread(&HttpServer::run_loop, this);
    return true;
}

void HttpServer::stop() {
    running_ = false;
    if (loop_.joinable()) loop_.join();
}

void HttpServer::handle(const std::string& method, const std::string& path, Handler h) {
    routes_[key(method, path)] = std::move(h);
}

void HttpServer::run_loop() {
    int server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { std::cerr << "HttpServer socket error\n"; return; }

    int opt = 1; ::setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{}; addr.sin_family = AF_INET; addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = inet_addr(bind_ip_.c_str());
    if (::bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "HttpServer bind error\n"; ::close(server_fd); return; }
    if (::listen(server_fd, 64) < 0) {
        std::cerr << "HttpServer listen error\n"; ::close(server_fd); return; }

    std::cout << "[HTTP] listening on " << bind_ip_ << ":" << port_ << std::endl;

    while (running_) {
        fd_set fds; FD_ZERO(&fds); FD_SET(server_fd, &fds);
        timeval tv{1,0};
        int rv = ::select(server_fd+1, &fds, nullptr, nullptr, &tv);
        if (rv <= 0) continue;
        sockaddr_in caddr{}; socklen_t clen=sizeof(caddr);
        int cfd = ::accept(server_fd,(sockaddr*)&caddr,&clen);
        if (cfd < 0) continue;

        char buf[8192]; int n = ::recv(cfd, buf, sizeof(buf)-1, 0);
        if (n <= 0) { ::close(cfd); continue; }
        buf[n] = 0;
        std::string req(buf);

        // parse request line
        std::istringstream iss(req);
        std::string method, path, version; iss >> method >> path >> version;
        auto pos = req.find("\r\n\r\n");
        std::string body = (pos!=std::string::npos) ? req.substr(pos+4) : std::string{};
        HttpRequest hreq{method, path, body};

        HttpResponse hres; // default 200 {}
        auto it = routes_.find(key(method, path));
        if (it != routes_.end()) hres = it->second(hreq);
        else { hres.status=404; hres.body = "{\"error\":\"not found\"}"; }

        std::ostringstream oss;
        oss << "HTTP/1.1 " << hres.status << " OK\r\n";
        oss << "Content-Type: " << hres.content_type << "\r\n";
        oss << "Content-Length: " << hres.body.size() << "\r\n";
        oss << "Connection: close\r\n\r\n";
        oss << hres.body;
        auto out = oss.str();
        ::send(cfd, out.data(), out.size(), 0);
        ::close(cfd);
    }
    ::close(server_fd);
}

} // namespace itacati::api