#include "CallManager.hpp"

#include <chrono>
#include <ctime>
#include <random>

namespace itacati::core {

static uint64_t now_ms() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

static std::string rand_id() {
    static std::mt19937_64 rng{std::random_device{}()};
    static const char* c = "abcdef0123456789";
    std::string s(16,'0');
    for (auto &ch : s) ch = c[rng()%16];
    return s;
}

CallManager::CallManager() {}
CallManager::~CallManager() {}

void CallManager::emit(const std::string& type, const std::string& call_id, const std::string& data) {
    std::lock_guard<std::mutex> lk(ev_mtx_);
    Event e; e.id = ++ev_seq_; e.type=type; e.callId=call_id; e.data=data; e.ts=now_ms();
    events_.push_back(std::move(e));
}

std::string CallManager::make_call(const std::string& dst) {
    std::lock_guard<std::mutex> lk(mtx_);
    Call c; c.id = rand_id(); c.dst = dst; c.state = CallState::Ringing;
    calls_.emplace(c.id, c);
    emit("call.ringing", c.id, "{}");
    // 模拟 300ms 后接通
    std::thread([this, id=c.id]{ std::this_thread::sleep_for(std::chrono::milliseconds(300));
        {
            std::lock_guard<std::mutex> lk(mtx_);
            auto it = calls_.find(id); if (it==calls_.end()) return;
            if (it->second.state == CallState::Ringing) {
                it->second.state = CallState::Established;
                emit("call.established", id, "{}");
            }
        }
    }).detach();
    return c.id;
}

bool CallManager::hangup(const std::string& call_id) {
    std::lock_guard<std::mutex> lk(mtx_);
    auto it = calls_.find(call_id); if (it==calls_.end()) return false;
    it->second.state = CallState::Ended;
    emit("call.ended", call_id, "{}");
    calls_.erase(it);
    return true;
}

bool CallManager::hold(const std::string& call_id) {
    std::lock_guard<std::mutex> lk(mtx_);
    auto it = calls_.find(call_id); if (it==calls_.end()) return false;
    if (it->second.state != CallState::Established) return false;
    it->second.state = CallState::Held;
    emit("call.held", call_id, "{}");
    return true;
}

bool CallManager::unhold(const std::string& call_id) {
    std::lock_guard<std::mutex> lk(mtx_);
    auto it = calls_.find(call_id); if (it==calls_.end()) return false;
    if (it->second.state != CallState::Held) return false;
    it->second.state = CallState::Established;
    emit("call.unheld", call_id, "{}");
    return true;
}

std::vector<Event> CallManager::poll(uint64_t since_id, size_t maxn) {
    std::lock_guard<std::mutex> lk(ev_mtx_);
    std::vector<Event> out; out.reserve(maxn);
    for (auto &e : events_) {
        if (e.id > since_id) { out.push_back(e); if (out.size()>=maxn) break; }
    }
    return out;
}

} // namespace itacati::core