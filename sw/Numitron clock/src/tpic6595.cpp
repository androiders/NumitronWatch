#include "tpic6595.h"


Tpic6595::Tpic6595(uint8_t serck, uint8_t srclr, uint8_t rclk, uint8_t g1, uint8_t s1, uint8_t g2, uint8_t s2, uint8_t g3, uint8_t s3, uint8_t g4, uint8_t s4)
:m_g1(g1)
,m_serIn1(s1)
,m_g2(g2)
,m_serIn2(s2)
,m_g3(g3)
,m_serIn3(s3)
,m_g4(g4)
,m_serIn4(s4)
,m_serck(serck)
,m_srclr(srclr)
,m_rclk(rclk)
,m_enabled(true)

{
    pinMode(m_serIn1,OUTPUT);
    pinMode(m_serIn2,OUTPUT);
    pinMode(m_serIn3,OUTPUT);
    pinMode(m_serIn4,OUTPUT);
    
    pinMode(m_g1,OUTPUT);
    pinMode(m_g2,OUTPUT);
    pinMode(m_g3,OUTPUT);
    pinMode(m_g4,OUTPUT);

    pinMode(m_serck,OUTPUT);
    pinMode(m_srclr,OUTPUT);
    pinMode(m_rclk,OUTPUT);
    
    digitalWrite(m_serIn1,LOW);
    digitalWrite(m_serIn2,LOW);
    digitalWrite(m_serIn3,LOW);
    digitalWrite(m_serIn4,LOW);
    
    digitalWrite(m_g1,HIGH);
    digitalWrite(m_g2,HIGH);
    digitalWrite(m_g3,HIGH);
    digitalWrite(m_g4,HIGH);
    
    digitalWrite(m_srclr,LOW);
    digitalWrite(m_serck,LOW);
    digitalWrite(m_rclk,LOW);
}


void Tpic6595::writeBytes(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)
{
    digitalWrite(m_srclr,HIGH);
    digitalWrite(m_g1,HIGH);
    digitalWrite(m_g2,HIGH);
    digitalWrite(m_g3,HIGH);
    digitalWrite(m_g4,HIGH);

    //shiftOut(m_serIn,m_serck,LSBFIRST, data);

     for(uint8_t i = 0; i < 8; ++i)
     {
        uint8_t BIT1 = !!(b1 & (0x01 << i));
        uint8_t BIT2 = !!(b2 & (0x01 << i));
        uint8_t BIT3 = !!(b3 & (0x01 << i));
        uint8_t BIT4 = !!(b4 & (0x01 << i));
        this->shiftOut(BIT1, BIT2, BIT3, BIT4);
     }

    digitalWrite(m_rclk,HIGH);
    //delayMicroseconds(10);
    digitalWrite(m_rclk,LOW);

    digitalWrite(m_g1,LOW);
    digitalWrite(m_g2,LOW);
    digitalWrite(m_g3,LOW);
    digitalWrite(m_g4,LOW);
    digitalWrite(m_srclr,LOW);
}

void Tpic6595::clear()
{
    digitalWrite(m_srclr,LOW);
    delayMicroseconds(10);
    digitalWrite(m_srclr,HIGH);
}

void Tpic6595::toggle()
{
    setEnabled(!m_enabled);
}

void Tpic6595::setEnabled(bool enable)
{
    m_enabled = enable;
    digitalWrite(m_g1,m_enabled);
    digitalWrite(m_g2,m_enabled);
    digitalWrite(m_g3,m_enabled);
    digitalWrite(m_g4,m_enabled);
}

// void Tpic6595::disableOutput()
// {
//     digitalWrite(m_g,HIGH);
// }

// void Tpic6595::enableOutput()
// {
//     digitalWrite(m_g,LOW);
// }


void Tpic6595::shiftOut(uint8_t BIT1,uint8_t BIT2,uint8_t BIT3,uint8_t BIT4)
{
    digitalWrite(m_serIn1, BIT1);
    digitalWrite(m_serIn2, BIT2);
    digitalWrite(m_serIn3, BIT3);
    digitalWrite(m_serIn4, BIT4);
    digitalWrite(m_serck, HIGH);
    digitalWrite(m_serck, LOW);   
}