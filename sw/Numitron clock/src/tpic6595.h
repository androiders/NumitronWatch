#pragma once

#include <Arduino.h>

class Tpic6595
{
    public:
        Tpic6595(uint8_t serck, uint8_t srclr, uint8_t rclk, uint8_t g1, uint8_t s1, uint8_t g2, uint8_t s2, uint8_t g3, uint8_t s3, uint8_t g4, uint8_t s4);
        ~Tpic6595() = default;

        void writeBytes(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);
        void clear();
//        void enableOutput();
//        void disableOutput();

    private:

        void shiftOut(uint8_t BIT1,uint8_t BIT2,uint8_t BIT3,uint8_t BIT4);

        uint8_t m_serck;
        uint8_t m_srclr;
        uint8_t m_rclk;
        uint8_t m_g1;
        uint8_t m_g2;
        uint8_t m_g3;
        uint8_t m_g4;

        uint8_t m_serIn1;
        uint8_t m_serIn2;
        uint8_t m_serIn3;
        uint8_t m_serIn4;

};