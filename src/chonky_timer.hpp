#pragma once
#include <chrono>

namespace prim
{
    typedef std::chrono::seconds seconds;
    typedef std::chrono::milliseconds milliseconds;
    typedef std::chrono::microseconds microseconds;

    class Timer
    {

    private:
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point lastPeek;
        bool counting = false;
    public:
        void start()
        {
            startTime = std::chrono::steady_clock::now();
            lastPeek = startTime;
            counting = true;
        }

        template<typename typeOfTime = milliseconds>
        float peek()
        {
            if (counting)
            {
                auto peekTime = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<typeOfTime>(peekTime - startTime);
                lastPeek = peekTime;
                return duration.count();
            }
            else
            {
                return 0.0f;
            }
        }

        template<typename typeOfTime = milliseconds>
        float peekSinceLastPeek()
        {
            if (counting)
            {
                auto peekTime = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<typeOfTime>(peekTime - lastPeek);
                lastPeek = peekTime;
                return duration.count();
            }
            else
            {
                return 0.0f;
            }
        }

        void reset()
        {
            counting = false;
        }
    };

}
