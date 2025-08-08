#pragma once
#include <string>
#include <functional>

// 占位：控制面 API（建议 gRPC + gRPC-Web / 或 REST + WSS）
// 提供：健康检查、WebRTC SDP 交换、呼叫控制（外呼/挂断/保持/恢复/转接/DTMF）。

namespace itacati::api {

class ApiServer {
public:
    bool start(const std::string& bind, int port);
    void stop();
};

} // namespace itacati::api