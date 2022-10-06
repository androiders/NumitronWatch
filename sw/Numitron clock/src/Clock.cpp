#include "Clock.h"


Clock::Clock()
:m_seconds(0)
,m_minutes(0)
,m_hours(0)
{
}

void Clock::setMins(uint8_t mins)
{
    if(mins > 59)
        return;

    m_minutes = mins;
}

void Clock::setHours(uint8_t hours)
{
    if(hours > 23)
        return;

    m_hours = hours;
}

void Clock::setSeconds(uint8_t secs)
{
    if(secs > 59)
        return;
    
    m_seconds = secs;
} 

void Clock::set(uint8_t h, uint8_t m, uint8_t s)
{
    setHours(h);
    setMins(m);
    setSeconds(s);
}

void Clock::tickSeconds()
{
    if(m_seconds == 59)
    {
        m_seconds = 0;
        tickMinutes();
    }
    else
        m_seconds++;
}

void Clock::tickMinutes()
{
    if( m_minutes == 59)
    {
        m_minutes = 0;
        tickHours();
    }
    else
        m_minutes++;

}
    

void Clock::tickHours()
{
    if(m_hours == 23)
    {
        m_hours = 0;
        m_seconds = m_compensationSeconds;
    }
    else
        m_hours++;

}
