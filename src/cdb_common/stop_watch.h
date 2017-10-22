#pragma once

#include <cstdint>
#include <chrono>

namespace cdb {

class stop_watch
{
public:
    stop_watch()
    {
        start();
    }
    uint64_t ellapsed()
    {
        stop();
        uint64_t ellapsed = std::chrono::duration_cast<std::chrono::milliseconds>(m_end - m_start).count();
        return ellapsed;
    }
    void start()
    {
        m_start = std::chrono::high_resolution_clock::now();
    }
    void stop()
    {
        m_end = std::chrono::high_resolution_clock::now();
    }
private:
    std::chrono::time_point<std::chrono::steady_clock> m_start;
    std::chrono::time_point<std::chrono::steady_clock> m_end;
};

}
