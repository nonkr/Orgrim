#pragma once

#include <mutex>
#include <condition_variable>

namespace swp
{
    class barrier
    {
    public:
        barrier(unsigned int count)
            : m_threshold(count), m_count(count), m_generation(0)
        {
            if (count == 0)
                throw std::invalid_argument("count cannot be zero.");
        }

        bool wait()
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            unsigned int                 gen = m_generation;

            if (--m_count == 0)
            {
                m_generation++;
                m_count = m_threshold;
                m_cond.notify_all();
                return true;
            }

            while (gen == m_generation)
                m_cond.wait(lock);

            return false;
        }

    private:
        std::mutex              m_mutex;
        std::condition_variable m_cond;
        unsigned int            m_threshold;
        unsigned int            m_count;
        unsigned int            m_generation;
    };

}
