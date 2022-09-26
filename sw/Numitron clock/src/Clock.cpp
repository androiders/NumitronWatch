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
  
void Clock::tickSeconds()
{
    m_seconds++;
    if(m_seconds > 59)
        tickMinutes();
}

void Clock::tickMinutes()
{
    m_minutes++;
    m_seconds = 0;
    if( m_minutes > 59)
        tickHours();
}

void Clock::tickHours()
{
    m_hours++;
    m_minutes = 0;
    m_seconds = 0;
    if(m_hours > 23)
        m_hours = 0;
}
