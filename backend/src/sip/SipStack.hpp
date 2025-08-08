#pragma once
#include <string>
#include <functional>

// 占位：pjsip 封装接口（注册/呼叫/保持/转接/DTMF/订阅/会话定时器）
// 后续提供实现文件，并链接 pjsip/pjmedia/pjnath/libsrtp。

namespace itacati { namespace core { class CallManager; } }

namespace itacati::sip {

struct CallEvent {
    std::string call_id;
    std::string state;      // ringing/established/held/ended/...
    int         code{0};    // 失败码或状态码
    std::string reason;
};

using EventCallback = std::function<void(const CallEvent&)>;

class SipStack {
public:
    // 仅用于 demo：让 Dummy 实现能驱动内存 CallManager
    void attach_call_manager(core::CallManager* cm);

    bool init(const std::string& bind_ip, int port, bool tls);
    bool register_account(const std::string& uri, const std::string& user,
                          const std::string& password);

    std::string make_call(const std::string& dst_uri);
    bool        hangup(const std::string& call_id);
    bool        hold(const std::string& call_id);
    bool        unhold(const std::string& call_id);
    bool        dtmf(const std::string& call_id, const std::string& digits);
    bool        transfer_blind(const std::string& call_id, const std::string& target_uri);

    void        on_event(EventCallback cb);
    void        shutdown();
};

} // namespace itacati::sip