#ifndef TIME_CONTROL_H_
#define TIME_CONTROL_H_

#include <Arduino.h>
// time control
namespace tctrl {
    
    class Time {
        uint8_t hour_{0};
        uint8_t minute_{0};
        uint8_t second_{0};
    public:
        Time() = default;
        Time(uint16_t sum_seconds) ;
        Time(uint8_t hour, uint8_t minute, uint8_t second);
        bool set_hour(uint8_t value);
        bool set_minute(uint8_t value);
        bool set_second(uint8_t value);
        uint8_t get_hour() const;
        uint8_t get_minute() const;
        uint8_t get_second() const;

        uint16_t get_sum_second() const;

        Time& operator+=(const Time& rhs);
        Time& operator-=(const Time& rhs);
        bool operator<(const Time& rhs) const;
    };
    bool operator>(const Time& lhs, const Time& rhs);
    bool operator<=(const Time& lhs, const Time& rhs);
    bool operator>=(const Time& lhs, const Time& rhs);
    bool operator==(const Time& lhs, const Time& rhs);
    bool operator!=(const Time& lhs, const Time& rhs);
    Time operator+(const Time& lhs, const Time& rhs);
    Time operator-(const Time& lhs, const Time& rhs);

    class Time_control {
        Time start_signal;
        Time end_signal;
    public:
        // Время начала сигнала
        void set_start_time(Time time);
        Time get_start_time();
        const Time& get_start_time() const;

        // Время конца сигнала
        void set_end_time(Time time);
        bool set_duration_signal(uint16_t duration);
        Time get_end_time();
        const Time& get_end_time() const;

        // Проверка есть ли сигнал
        bool check_signal(Time current_time);
    };
}

#endif // TIME_CONTROL_H_
