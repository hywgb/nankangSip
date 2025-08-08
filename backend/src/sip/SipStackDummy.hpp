#pragma once
#include "SipStack.hpp"
#include "../core/CallManager.hpp"

namespace itacati::sip {

class SipStackDummy : public SipStack {
public:
    void attach_call_manager(core::CallManager* cm) { cm_ = cm; }

    bool init(const std::string&, int, bool) { return true; }
    bool register_account(const std::string&, const std::string&, const std::string&) { return true; }

    std::string make_call(const std::string& dst_uri) {
        if (!cm_) return {};
        return cm_->make_call(dst_uri);
    }
    bool hangup(const std::string& id)  { return cm_? cm_->hangup(id):false; }
    bool hold(const std::string& id)    { return cm_? cm_->hold(id):false; }
    bool unhold(const std::string& id)  { return cm_? cm_->unhold(id):false; }
    bool dtmf(const std::string&, const std::string&) { return true; }
    bool transfer_blind(const std::string&, const std::string&) { return true; }

    void on_event(EventCallback) {}
    void shutdown() {}

private:
    core::CallManager* cm_{nullptr};
};

} // namespace itacati::sip