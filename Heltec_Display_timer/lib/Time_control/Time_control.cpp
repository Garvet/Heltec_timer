#include "Time_control.h"

namespace tctrl {
    // ----- Time -----
    Time::Time(uint16_t sum_seconds) {
        // Обнуление в случае ошибки
        if(set_hour(sum_seconds / 3600) || set_minute(sum_seconds % 3600 / 60) || set_second(sum_seconds % 60)) {
            hour_ = minute_ = second_ = 0;
        }
    }
    Time::Time(uint8_t hour, uint8_t minute, uint8_t second) {
        // Обнуление в случае ошибки
        if(set_hour(hour) || set_minute(minute) || set_second(second)) {
            hour_ = minute_ = second_ = 0;
        }
    }
    bool Time::set_hour(uint8_t value) {
        if(23 < value)
            return true;
        hour_ = value;
        return false;
    }
    bool Time::set_minute(uint8_t value) {
        if(59 < value)
            return true;
        minute_ = value;
        return false;
    }
    bool Time::set_second(uint8_t value) {
        if(59 < value)
            return true;
        second_ = value;
        return false;
    }
    uint8_t Time::get_hour() const {
        return hour_;
    }
    uint8_t Time::get_minute() const {
        return minute_;
    }
    uint8_t Time::get_second() const {
        return second_;
    }

    uint16_t Time::get_sum_second() const {
        return ((uint16_t)hour_) * 3600 + ((uint16_t)minute_) * 60 + second_;
    }

    Time& Time::operator+=(const Time& rhs) {
        uint8_t overflow;
        // second_ = (second_ + rhs.second_) % 60;
        // overflow = second_ < rhs.second_;
        // minute_ = (minute_ + rhs.minute_ + overflow) % 60;
        // overflow = (minute_) < (rhs.minute_+ overflow);
        // hour_ = (hour_ + rhs.hour_ + overflow) % 24;

        if((second_ + rhs.second_) < 60) {
            second_ += rhs.second_;
            overflow = 0;
        }
        else {
            second_ = second_ + rhs.second_ - 60;
            overflow = 1;
        }

        if((minute_ + rhs.minute_ + overflow) < 60) {
            minute_ += rhs.minute_ + overflow;
            overflow = 0;
        }
        else {
            minute_ = minute_ + rhs.minute_ + overflow - 60;
            overflow = 1;
        }

        if((hour_ + rhs.hour_ + overflow) < 24) {
            hour_ += rhs.hour_ + overflow;
        }
        else {
            hour_ = hour_ + rhs.hour_ + overflow - 24;
        }
        return *this;
    }
    Time& Time::operator-=(const Time& rhs) {
        uint8_t overflow;
        // second_ = 60 * (second_ < rhs.second_) + second_ - rhs.second_;
        // overflow = 60 <= (second_ + rhs.second_);
        // minute_ = 60 * (minute_ < (rhs.minute_ + overflow)) + minute_ - (rhs.minute_ + overflow);
        // overflow = 60 <= (minute_ + rhs.minute_ - overflow);
        // hour_ = 24 * (hour_ < (rhs.hour_ + overflow)) + hour_ - (rhs.hour_ + overflow);

        if(second_ < rhs.second_) {
            second_ += 60 - rhs.second_;
            overflow = 1;
        }
        else {
            second_ -= rhs.second_;
            overflow = 0;
        }

        if(minute_ < (rhs.minute_ + overflow)) {
            minute_ += 60 - rhs.minute_ - overflow;
            overflow = 1;
        }
        else {
            minute_ -= rhs.minute_ + overflow;
            overflow = 0;
        }

        if(hour_ < (rhs.hour_ + overflow)) {
            hour_ += 24 - rhs.hour_ - overflow;
        }
        else {
            hour_ -= rhs.hour_ + overflow;
        }
        return *this;
    }
    bool Time::operator<(const Time& rhs) const {
        return (  hour_   <  rhs.hour_)   ||
               (( hour_   == rhs.hour_)   && ((minute_ < rhs.minute_) ||
                ((minute_ == rhs.minute_) &&  (second_ < rhs.second_))));
    }
    bool operator>(const Time& lhs, const Time& rhs) {
        return rhs < lhs;
    }
    bool operator<=(const Time& lhs, const Time& rhs) {
        return !(rhs < lhs);
    }
    bool operator>=(const Time& lhs, const Time& rhs) {
        return !(lhs < rhs);
    }
    bool operator==(const Time& lhs, const Time& rhs) {
        return !(lhs < rhs) && !(rhs < lhs);
    }
    bool operator!=(const Time& lhs, const Time& rhs) {
        return (lhs < rhs) || (rhs < lhs);
    }
    Time operator+(const Time& lhs, const Time& rhs) {
        return (Time{lhs} += rhs);
    }
    Time operator-(const Time& lhs, const Time& rhs) {
        return (Time{lhs} -= rhs);
    }

    // ----- Time_control -----

    // Время начала сигнала
    void Time_control::set_start_time(Time time) {
        start_signal = time;
    }
    Time Time_control::get_start_time() {
        return start_signal;
    }
    const Time& Time_control::get_start_time() const {
        return start_signal;
    }

    // Время конца сигнала
    void Time_control::set_end_time(Time time) {
        end_signal = time;
    }
    bool Time_control::set_duration_signal(uint16_t duration) {
        if(23 < duration / 3600)
            return true;
        end_signal = Time{duration} + start_signal;
        return false;
    }
    Time Time_control::get_end_time() {
        return end_signal;
    }
    const Time& Time_control::get_end_time() const {
        return end_signal;
    }

    // Проверка есть ли сигнал
    bool Time_control::check_signal(Time current_time) {
        if(start_signal < end_signal) {
            // ---|=====|---
            return (start_signal <= current_time) && (current_time < end_signal);
        }
        else if(start_signal > end_signal) {
            // ===|-----|===
            return (current_time < start_signal) && (end_signal <= current_time);
        }
        //     ===|=========
        return true;
    }
}
