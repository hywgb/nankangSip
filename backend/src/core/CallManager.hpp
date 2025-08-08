#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace itacati::core {

enum class CallState { Ringing, Established, Held, Ended };

struct Call {
    std::string id;
    std::string dst;
    CallState   state{CallState::Ringing};
};

struct Event {
    uint64_t    id{0};
    std::string type;   // call.ringing/established/held/ended
    std::string callId;
    std::string data;   // JSON or brief text
    uint64_t    ts{0};
};

class CallManager {
public:
    CallManager();
    ~CallManager();

    std::string make_call(const std::string& dst);
    bool hangup(const std::string& call_id);
    bool hold(const std::string& call_id);
    bool unhold(const std::string& call_id);

    // 拉取自 sinceId 之后的事件（最多 n 条）
    std::vector<Event> poll(uint64_t since_id, size_t maxn=100);
    // 阻塞等待直到出现新事件或达到超时（毫秒）；返回新增事件
    std::vector<Event> wait_poll(uint64_t since_id, uint32_t timeout_ms, size_t maxn=100);

private:
    std::mutex mtx_;
    std::unordered_map<std::string, Call> calls_;

    std::mutex ev_mtx_;
    std::vector<Event> events_;
    std::atomic<uint64_t> ev_seq_{0};
    std::condition_variable ev_cv_;

    void emit(const std::string& type, const std::string& call_id, const std::string& data);
};

} // namespace itacati::core