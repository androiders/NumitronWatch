#pragma once
#include <Arduino.h>

class Clock
{
    public:

        Clock();

        ~Clock() = default;

        void setMins(uint8_t mins);

        void setHours(uint8_t hours);

        void setSeconds(uint8_t secs);

        void set(uint8_t h, uint8_t m, uint8_t s);

        void tickSeconds();

        void tickMinutes();

        void tickHours();

        uint8_t getMinuts() const
        {
            return m_minutes;
        }

        uint8_t getHours() const
        {
            return m_hours;
        }

        uint8_t getSeconds() const
        {
            return m_seconds;
        }

    private:

        uint8_t m_seconds;

        uint8_t m_minutes;

        uint8_t m_hours;
};

