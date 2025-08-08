#pragma once
#include <string>
#include <functional>

// 占位：WebRTC-SIP 网关（与浏览器做 DTLS-SRTP/ICE SDP 协商，桥接至 pjsip RTP）
// 后续可选用 libwebrtc 或 mediasoup C++，并做 Opus<->G.711 转码（必要时）。

namespace itacati::webrtc {

struct Sdp {
    std::string type; // offer/answer
    std::string sdp;
};

class Bridge {
public:
    bool init();
    // 浏览器传来 offer，返回 answer（JSON/SDP 字符串由 API 层封装）
    Sdp  create_answer_from_offer(const Sdp& offer);
    // trickle ICE（占位）
    void add_ice_candidate(const std::string& mid, int mline, const std::string& cand);
    void shutdown();
};

} // namespace itacati::webrtc