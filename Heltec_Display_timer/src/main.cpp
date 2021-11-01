// #define USE_STRING
// #define NOT_USE_STRING
#define NOT_USE_STRING_AND_CLAMPED


#if defined( NOT_USE_STRING_AND_CLAMPED )

#include <Arduino.h>
#include <heltec.h>
#include <Time_control.h>
#include <images.h>

#include <Display.h>

#define USE_SERIAL

#define AMT_BUTTON 4

#define LED_PIN  LED

#define BUTTON_1 GPIO_NUM_36
#define BUTTON_2 GPIO_NUM_37
#define BUTTON_3 GPIO_NUM_38
#define BUTTON_4 GPIO_NUM_39

#define LAMP_PIN GPIO_NUM_17

void button_1_press();
void button_2_press();
void button_3_press();
void button_4_press();
// void button_4_clamped();
void print_in_display();
// void display_init();
void display_begin();
#if defined( USE_SERIAL )
void read_button();
#endif

My_display display{0x3c, SDA_OLED, SCL_OLED, RST_OLED, GEOMETRY_128_64};

uint8_t button[4]{BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4};
bool button_state[4]{0, 0, 0, 0};
bool button_last_state[4]{0, 0, 0, 0};

tctrl::Time current_time{0, 0, 10};
tctrl::Time_control time_control;
// tctrl::Time set_time;

// bool start = false;
uint16_t duration = 4;
// uint32_t up_button_time = 0;
uint32_t press_button_time = 0;
uint32_t sec_millis = 0;
bool up_time = true;

bool input_data = false;
// uint8_t print_data = 32;
// char print_data[] = "hello";
#if defined( USE_SERIAL )
bool start_signal = false;
#endif

void setup() {
    // Heltec.begin(true /*Display Enable*/, false /*LoRa Disable*/, false /*Serial Enable*/);
    // Heltec.display->init();
    // Heltec.display->flipScreenVertically();
    // Heltec.display->setFont(ArialMT_Plain_16);
    display.init();
    display.flipScreenVertically();
    // display.setFont(ArialMT_Plain_16);
    display.setFont(ArialMT_Plain_24);
    time_control.set_start_time(tctrl::Time{0, 0, 0});
    time_control.set_duration_signal(4);
    delay(1000);
#if defined( USE_SERIAL )
    Serial.begin(115200);
#endif
    delay(1000);
    print_in_display();
    // Настройка выходов
    pinMode(LAMP_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    for(int i = 0; i < AMT_BUTTON; ++i)
        pinMode(button[i], INPUT_PULLDOWN);
}

void loop() {
    for(int i = 0; i < AMT_BUTTON; ++i)
        button_state[i] = digitalRead(button[i]);
    for(int i = 0; i < AMT_BUTTON; ++i) {
        if(millis() - press_button_time >= 50) {
            press_button_time += 50;
            if(button_last_state[i] != button_state[i]) {
                button_last_state[i] = button_state[i];
                if(!button_last_state[i]) {
                    switch (i)
                    {
                    case 0: button_1_press(); break;
                    case 1: button_2_press(); break;
                    case 2: button_3_press(); break;
                    case 3: button_4_press(); break;
                    default: break;
                    }
                }
                print_in_display();
            }
        }
    }
    if(millis() - sec_millis >= 1000) {
        sec_millis += 1000;
        current_time += tctrl::Time(1);
        if(time_control.check_signal(current_time)) {
            digitalWrite(LAMP_PIN, HIGH);
            digitalWrite(LED_PIN, HIGH);
#if defined( USE_SERIAL )
            if(!start_signal) {
                start_signal = true;
                Serial.println("Lamp = ON!");
            }
#endif
        }
        else {
            digitalWrite(LAMP_PIN, LOW);
            digitalWrite(LED_PIN, LOW);
#if defined( USE_SERIAL )
            if(start_signal) {
                start_signal = false;
                Serial.println("Lamp = OFF!");
            }
#endif
        }
        print_in_display();
    }

#if defined( USE_SERIAL )
    read_button();
#endif
}


void button_1_press() {
    if(up_time) {
        current_time.set_hour((tctrl::Time{current_time} += tctrl::Time(1, 0, 0)).get_hour());
    }
    else {
        current_time.set_hour((tctrl::Time{current_time} -= tctrl::Time(1, 0, 0)).get_hour());
    }
}
void button_2_press() {
    if(up_time) {
        current_time.set_minute((tctrl::Time{current_time} += tctrl::Time(0, 1, 0)).get_minute());
    }
    else {
        current_time.set_minute((tctrl::Time{current_time} -= tctrl::Time(0, 1, 0)).get_minute());
    }
}
void button_3_press() {
    if(up_time) {
        current_time.set_second((tctrl::Time{current_time} += tctrl::Time(0, 0, 1)).get_second());
    }
    else {
        current_time.set_second((tctrl::Time{current_time} -= tctrl::Time(0, 0, 1)).get_second());
    }
}
void button_4_press() {
    up_time = !up_time;
}

// String prt_str{"hello word!"};
char prt_str_ch_1[9] = {"00:00:00"};
char prt_str_ch_2[5] = {"(+)"};
void print_in_display() {
    display.clear();
    // Heltec.display.write((uint8_t)'0');
    // Heltec.display.write((uint8_t)'1');
    // Heltec.display.write((uint8_t)'2');
    // Heltec.display.write("012");
    prt_str_ch_1[0] = current_time.get_hour()/10 + '0';
    prt_str_ch_1[1] = current_time.get_hour()%10 + '0';
    prt_str_ch_1[3] = current_time.get_minute()/10 + '0';
    prt_str_ch_1[4] = current_time.get_minute()%10 + '0';
    prt_str_ch_1[6] = current_time.get_second()/10 + '0';
    prt_str_ch_1[7] = current_time.get_second()%10 + '0';

    if(up_time) {
        prt_str_ch_2[1] = '+';
    }
    else {
        prt_str_ch_2[1] = '-';
    }
    
    display.drawCharString(15, 0, prt_str_ch_1, 9);
    display.drawCharString(45, 30, prt_str_ch_2, 4);
    // display.drawCharString(0, 32, prt_str_ch, 19);
    // Heltec.display.write("456");
    // Heltec.display.write(print_data);
    display.display();
#if defined( OLD_CODE )
    String print_data{""};
    Heltec.display->clear();
    switch (type_input) {
    case Type_input::None: {
        switch (target_type_input) {
        case Type_input::None: {
            // Строка 1
            print_data = "Time = ";
            if(current_time.get_hour() < 10) print_data += '0'; print_data += String(current_time.get_hour()) + ":";
            if(current_time.get_minute() < 10) print_data += '0'; print_data += String(current_time.get_minute()) + ":";
            if(current_time.get_second() < 10) print_data += '0'; print_data += String(current_time.get_second());
            Heltec.display->drawString(0, 0, print_data);
            // Строка 2
            print_data = "Start = ";
            if(time_control.get_start_time().get_hour() < 10) print_data += '0'; print_data += String(time_control.get_start_time().get_hour()) + ":";
            if(time_control.get_start_time().get_minute() < 10) print_data += '0'; print_data += String(time_control.get_start_time().get_minute()) + ":";
            if(time_control.get_start_time().get_second() < 10) print_data += '0'; print_data += String(time_control.get_start_time().get_second());
            Heltec.display->drawString(0, 15, print_data);
            break; 
        }
        case Type_input::Current_time: {
            // Строка 1
            print_data = "Select input:";
            Heltec.display->drawString(0, 0, print_data);
            // Строка 2
            print_data = "- current time";
            Heltec.display->drawString(0, 15, print_data);
            break; 
        }
        case Type_input::Start_time: {
            // Строка 1
            print_data = "Select input:";
            Heltec.display->drawString(0, 0, print_data);
            // Строка 2
            print_data = "- start signal time";
            Heltec.display->drawString(0, 15, print_data);
            break; 
        }
        case Type_input::Duration: {
            // Строка 1
            print_data = "Select input:";
            Heltec.display->drawString(0, 0, print_data);
            // Строка 2
            print_data = "- duration signal";
            Heltec.display->drawString(0, 15, print_data);
            break; 
        }
        case Type_input::Start_program: {
            // Строка 1
            print_data = "Select input:";
            Heltec.display->drawString(0, 0, print_data);
            // Строка 2
            print_data = "- work system";
            Heltec.display->drawString(0, 15, print_data);
            break; 
        }
        default:
            break;
        }
        // Строка 3
        print_data = "Start work = ";
        if(start)
            print_data += "ON";
        else
            print_data += "OFF";
        Heltec.display->drawString(0, 30, print_data);
        // Строка 4
        print_data = "Signal = ";
        if(time_control.check_signal(current_time) && start)
            print_data += "ON";
        else
            print_data += "OFF";
        Heltec.display->drawString(0, 45, print_data);
        break; 
    }
    case Type_input::Current_time: {
        // Строка 1
        print_data = "Input current time";
        Heltec.display->drawString(0, 0, print_data);
        // Строка 2
        print_data = "";
        if(set_time.get_hour() < 10)   print_data += '0'; print_data += String(set_time.get_hour()) + ":";
        if(set_time.get_minute() < 10) print_data += '0'; print_data += String(set_time.get_minute()) + ":";
        if(set_time.get_second() < 10) print_data += '0'; print_data += String(set_time.get_second());
        Heltec.display->drawString(0, 17, print_data);
        // Строка 3
        switch (state_input) {
        case State_input::Hour:   Heltec.display->drawString( 6, 32, "^"); break;
        case State_input::Minute: Heltec.display->drawString(28, 32, "^"); break;
        case State_input::Second: Heltec.display->drawString(50, 32, "^"); break;
        default:
            break;
        }
        // Строка 4
        if(!input_data) {
            switch (state_input) {
            case State_input::Hour:
            case State_input::Minute:
            case State_input::Second:
                Heltec.display->drawString(0, 45, "Select or save");
                break;
            case State_input::Cancel:
                Heltec.display->drawString(0, 45, "Don't save?");
                break;
            default:
                break;
            }
        }
        else {
            switch (state_input) {
            case State_input::Hour:
            case State_input::Minute:
            case State_input::Second:
                Heltec.display->drawString(0, 45, "Input value");
                break;
            default:
                break;
            }
        }
        break;
    }
    case Type_input::Start_time: {
        // Строка 1
        print_data = "Input start time";
        Heltec.display->drawString(0, 0, print_data);
        // Строка 2
        print_data = "";
        if(set_time.get_hour() < 10)   print_data += '0'; print_data += String(set_time.get_hour()) + ":";
        if(set_time.get_minute() < 10) print_data += '0'; print_data += String(set_time.get_minute()) + ":";
        if(set_time.get_second() < 10) print_data += '0'; print_data += String(set_time.get_second());
        Heltec.display->drawString(0, 17, print_data);
        // Строка 3
        switch (state_input) {
        case State_input::Hour:   Heltec.display->drawString( 6, 32, "^"); break;
        case State_input::Minute: Heltec.display->drawString(28, 32, "^"); break;
        case State_input::Second: Heltec.display->drawString(50, 32, "^"); break;
        default:
            break;
        }
        // Строка 4
        if(!input_data) {
            switch (state_input) {
            case State_input::Hour:
            case State_input::Minute:
            case State_input::Second:
                Heltec.display->drawString(0, 45, "Select or save");
                break;
            case State_input::Cancel:
                Heltec.display->drawString(0, 45, "Don't save?");
                break;
            default:
                break;
            }
        }
        else {
            switch (state_input) {
            case State_input::Hour:
            case State_input::Minute:
            case State_input::Second:
                Heltec.display->drawString(0, 45, "Input value");
                break;
            default:
                break;
            }
        }
        break;
    }
    case Type_input::Duration: {
        // Строка 1
        print_data = "Input duration";
        Heltec.display->drawString(0, 0, print_data);
        // Строка 2
        print_data = "";
        if(duration < 100) print_data += '0';
        if(duration < 10)  print_data += '0';
        print_data += String(duration);
        print_data += " sec";
        Heltec.display->drawString(0, 17, print_data);
        // Строка 3
        switch (state_input) {
        case State_input::Duration_100: Heltec.display->drawString( 1, 32, "^"); break;
        case State_input::Duration_010: Heltec.display->drawString(10, 32, "^"); break;
        case State_input::Duration_001: Heltec.display->drawString(19, 32, "^"); break;
        default:
            break;
        }
        // Строка 4
        if(!input_data) {
            switch (state_input) {
            case State_input::Duration_001:
            case State_input::Duration_010:
            case State_input::Duration_100:
                Heltec.display->drawString(0, 45, "Select or save");
                break;
            case State_input::Cancel:
                Heltec.display->drawString(0, 45, "Don't save?");
                break;
            default:
                break;
            }
        }
        else {
            switch (state_input) {
            case State_input::Duration_001:
            case State_input::Duration_010:
            case State_input::Duration_100:
                Heltec.display->drawString(0, 45, "Input value");
                break;
            default:
                break;
            }
        }
        break;
    }
    case Type_input::Start_program: {
        // Строка 1
        print_data = "State work system";
        Heltec.display->drawString(0, 0, print_data);
        // Строка 2
        if(start)
            Heltec.display->drawString(0, 17, "Work");
        else
            Heltec.display->drawString(0, 17, "Sleep");

        // Строка 3
        switch (state_input) {
        case State_input::Cancel:
            Heltec.display->drawString( 0, 34, "Don't change state?");
            break;      
        default:
            Heltec.display->drawString( 0, 34, "Change state?");
            break;
        }
        break;
    }  
    default:
        break;
    }
    Heltec.display->display();
#endif
}


void display_begin() {
    display.init();
    //display->flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    // display.drawString(0, 0, "OLED initial done!");
    display.display();
}

#if defined( USE_SERIAL )
uint8_t get_byte_serial = 0;
void read_button() {
    if(Serial.available() != 0) {
        get_byte_serial = Serial.read();
        switch (get_byte_serial)
        {
        case '1':
            button_1_press();
            break;
        case '2':
            button_2_press();
            break;
        case '3':
            button_3_press();
            break;
        case '4':
            button_4_press();
            break;
        default:
            break;
        }
        switch (get_byte_serial)
        {
        case '1':
        case '2':
        case '3':
        case '4':
            print_in_display();
            Serial.println((char)get_byte_serial);
        default:
            break;
        }
    }
}
#endif

#endif











#if defined( NOT_USE_STRING )

#include <Arduino.h>
#include <heltec.h>
#include <Time_control.h>
#include <images.h>

#include <Display.h>

// #define USE_SERIAL

#define AMT_BUTTON 4

#define LED_PIN  LED

#define BUTTON_1 GPIO_NUM_36
#define BUTTON_2 GPIO_NUM_37
#define BUTTON_3 GPIO_NUM_38
#define BUTTON_4 GPIO_NUM_39

#define LAMP_PIN GPIO_NUM_17

void button_1_press();
void button_2_press();
void button_3_press();
void button_4_press();
void button_4_clamped();
void print_in_display();
// void display_init();
void display_begin();
#if defined( USE_SERIAL )
void read_button();
#endif

My_display display{0x3c, SDA_OLED, SCL_OLED, RST_OLED, GEOMETRY_128_64};

uint8_t button[4]{BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4};
bool button_state[4]{0, 0, 0, 0};
bool button_last_state[4]{0, 0, 0, 0};

tctrl::Time current_time;
tctrl::Time_control time_control;
// tctrl::Time set_time;

bool start = false;
uint16_t duration = 4;
uint32_t up_button_time = 0;
uint32_t press_button_time = 0;
uint32_t sec_millis = 0;
bool up_time = true;

bool input_data = false;
// uint8_t print_data = 32;
// char print_data[] = "hello";
#if defined( USE_SERIAL )
bool start_signal = false;
#endif

void setup() {
    // Heltec.begin(true /*Display Enable*/, false /*LoRa Disable*/, false /*Serial Enable*/);
    // Heltec.display->init();
    // Heltec.display->flipScreenVertically();
    // Heltec.display->setFont(ArialMT_Plain_16);
    display.init();
    display.flipScreenVertically();
    // display.setFont(ArialMT_Plain_16);
    display.setFont(ArialMT_Plain_24);
    time_control.set_start_time(tctrl::Time{0, 0, 0});
    time_control.set_duration_signal(4);
    delay(1000);
#if defined( USE_SERIAL )
    Serial.begin(115200);
#endif
    delay(1000);
    print_in_display();
    // Настройка выходов
    pinMode(LAMP_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    for(int i = 0; i < AMT_BUTTON; ++i)
        pinMode(button[i], INPUT_PULLDOWN);
}

void loop() {
    for(int i = 0; i < AMT_BUTTON; ++i)
        button_state[i] = digitalRead(button[i]);
    for(int i = 0; i < AMT_BUTTON; ++i) {
        if(millis() - press_button_time >= 50) {
            press_button_time += 50;
            if(button_last_state[i] != button_state[i]) {
                button_last_state[i] = button_state[i];
                if(!button_last_state[i]) {
                    switch (i)
                    {
                    case 0: button_1_press(); break;
                    case 1: button_2_press(); break;
                    case 2: button_3_press(); break;
                    case 3: {
                        up_button_time = millis();
                        if(up_button_time == 0)
                            up_button_time = 1;
                        break;
                    }
                    default: break;
                    }
                }
                else if(i == 3) {
                    if(up_button_time != 0) {
                        up_button_time = 0;
                        button_4_press();
                    }
                }
                print_in_display();
            }
        }
    }
    if(millis() - up_button_time >= 2000) {
        if(up_button_time != 0) {
            up_button_time = 0;
            button_4_clamped();
        }
    }
    if(millis() - sec_millis >= 1000) {
        sec_millis += 1000;
        current_time += tctrl::Time(1);
        if(time_control.check_signal(current_time) && start) {
            digitalWrite(LAMP_PIN, HIGH);
#if defined( USE_SERIAL )
            if(!start_signal) {
                start_signal = true;
                Serial.println("Lamp = ON!");
            }
#endif
        }
        else {
            digitalWrite(LAMP_PIN, LOW);
#if defined( USE_SERIAL )
            if(start_signal) {
                start_signal = false;
                Serial.println("Lamp = OFF!");
            }
#endif
        }
        print_in_display();
    }

#if defined( USE_SERIAL )
    read_button();
#endif
}


void button_1_press() {
    if(up_time) {
        current_time.set_hour((tctrl::Time{current_time} += tctrl::Time(1, 0, 0)).get_hour());
    }
    else {
        current_time.set_hour((tctrl::Time{current_time} -= tctrl::Time(1, 0, 0)).get_hour());
    }

#if defined( OLD_CODE )
    if(!input_data) {
        switch (type_input) {
        case Type_input::None:          // Выбор 
            type_input = target_type_input;
            switch (target_type_input) {
            case Type_input::None: break; 
            case Type_input::Current_time: 
                set_time = current_time;
                state_input = State_input::Second;
                break; 
            case Type_input::Start_time:
                set_time = time_control.get_start_time();
                state_input = State_input::Second;
                break; 
            case Type_input::Duration:
                duration = (time_control.get_end_time() - time_control.get_start_time()).get_sum_second();
                state_input = State_input::Duration_001;
                break; 
            case Type_input::Start_program:
                state_input = State_input::On_lamp;
                break;    
            default:
                break;
            }
            break; 
        case Type_input::Current_time:  // Ввод текущего времени
            current_time = set_time;
            type_input = Type_input::None;
            break; 
        case Type_input::Start_time:    // Ввод времени старта
            if(state_input != State_input::Cancel) {
                duration = (time_control.get_end_time() - time_control.get_start_time()).get_sum_second();
                time_control.set_start_time(set_time);
                time_control.set_duration_signal(duration);
            }
            type_input = Type_input::None;
            break; 
        case Type_input::Duration:      // Ввод длительности сигнала
            if(state_input != State_input::Cancel) {
                time_control.set_duration_signal(duration);
            }
            type_input = Type_input::None;
            break; 
        case Type_input::Start_program: // Управление стартом программы
            if(state_input != State_input::Cancel) {
                start = !start;
            }
            type_input = Type_input::None;
            break;    
        default:
            break;
        }
    }
    else {
        input_data = false;
    }
#endif
}
void button_2_press() {
    if(up_time) {
        current_time.set_minute((tctrl::Time{current_time} += tctrl::Time(0, 1, 0)).get_minute());
    }
    else {
        current_time.set_minute((tctrl::Time{current_time} -= tctrl::Time(0, 1, 0)).get_minute());
    }

#if defined( OLD_CODE )
    if(!input_data) {
        switch (type_input) {
        case Type_input::None:          // Выбор 
            switch (target_type_input) {
            case Type_input::None:          target_type_input = Type_input::Current_time;  break; 
            case Type_input::Current_time:  target_type_input = Type_input::Start_time;    break; 
            case Type_input::Start_time:    target_type_input = Type_input::Duration;      break; 
            case Type_input::Duration:      target_type_input = Type_input::Start_program; break; 
            case Type_input::Start_program: target_type_input = Type_input::None;          break;    
            default:
                break;
            }
            break; 
        case Type_input::Current_time:  // Ввод текущего времени
        case Type_input::Start_time:    // Ввод времени старта
            switch (state_input) {
            case State_input::Second: state_input = State_input::Minute; break;    
            case State_input::Minute: state_input = State_input::Hour;   break;    
            case State_input::Hour:   state_input = State_input::Cancel; break;    
            case State_input::Cancel: state_input = State_input::Second; break;    
            default:
                break;
            }
            break;
        case Type_input::Duration:      // Ввод длительности сигнала
            switch (state_input) {
            case State_input::Duration_001: state_input = State_input::Duration_010; break;    
            case State_input::Duration_010: state_input = State_input::Duration_100; break;    
            case State_input::Duration_100: state_input = State_input::Cancel;       break;    
            case State_input::Cancel:       state_input = State_input::Duration_001; break;    
            default:
                break;
            }
            break;
        case Type_input::Start_program: // Управление стартом программы
            switch (state_input) {
            case State_input::On_lamp: state_input = State_input::Cancel;  break;
            case State_input::Cancel:  state_input = State_input::On_lamp; break;    
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    else {
        switch (type_input) {
        case Type_input::None:
            break; 
        case Type_input::Current_time:  // Ввод текущего времени
        case Type_input::Start_time:    // Ввод времени старта
            switch (state_input) {
            default:
                break;
            }
            break; 
        case Type_input::Duration:      // Ввод длительности сигнала
            switch (state_input) {
            case State_input::Duration_001: duration = ((duration       % 10) != 9) ? (duration +   1) : (duration -   9); break;    
            case State_input::Duration_010: duration = ((duration /  10 % 10) != 9) ? (duration +  10) : (duration -  90); break;    
            case State_input::Duration_100: duration = ((duration / 100 % 10) != 9) ? (duration + 100) : (duration - 900); break;    
            default:
                break;
            }
            break; 
        case Type_input::Start_program: // Управление стартом программы
            break; 
        default:
            break;
        }
    }
#endif
}
void button_3_press() {
    if(up_time) {
        current_time.set_second((tctrl::Time{current_time} += tctrl::Time(0, 0, 1)).get_second());
    }
    else {
        current_time.set_second((tctrl::Time{current_time} -= tctrl::Time(0, 0, 1)).get_second());
    }

#if defined( OLD_CODE )
    if(!input_data) {
        switch (type_input) {
        case Type_input::None:          // Выбор 
            switch (target_type_input) {
            case Type_input::None:          target_type_input = Type_input::Start_program; break; 
            case Type_input::Current_time:  target_type_input = Type_input::None;          break; 
            case Type_input::Start_time:    target_type_input = Type_input::Current_time;  break; 
            case Type_input::Duration:      target_type_input = Type_input::Start_time;    break; 
            case Type_input::Start_program: target_type_input = Type_input::Duration;      break;    
            default:
                break;
            }
            break; 
        case Type_input::Current_time:  // Ввод текущего времени
        case Type_input::Start_time:    // Ввод времени старта
            switch (state_input) {
            case State_input::Second: state_input = State_input::Cancel; break;    
            case State_input::Minute: state_input = State_input::Second; break;    
            case State_input::Hour:   state_input = State_input::Minute; break;    
            case State_input::Cancel: state_input = State_input::Hour;   break;    
            default:
                break;
            }
            break; 
        case Type_input::Duration:      // Ввод длительности сигнала
            switch (state_input) {
            case State_input::Duration_001: state_input = State_input::Cancel;       break;    
            case State_input::Duration_010: state_input = State_input::Duration_001; break;    
            case State_input::Duration_100: state_input = State_input::Duration_010; break;    
            case State_input::Cancel:       state_input = State_input::Duration_100; break;    
            default:
                break;
            }
            break; 
        case Type_input::Start_program: // Управление стартом программы
            switch (state_input) {
            case State_input::On_lamp: state_input = State_input::Cancel;  break;
            case State_input::Cancel:  state_input = State_input::On_lamp; break;      
            default:
                break;
            }
            break; 
        default:
            break;
        }
    }
    else {
        switch (type_input) {
        case Type_input::None:
            break; 
        case Type_input::Current_time:  // Ввод текущего времени
        case Type_input::Start_time:    // Ввод времени старта
            switch (state_input) {
            default:
                break;
            }
            break; 
        case Type_input::Duration:      // Ввод длительности сигнала
            switch (state_input) {
            case State_input::Duration_001: duration = ((duration       % 10) != 0) ? (duration -   1) : (duration +   9); break;    
            case State_input::Duration_010: duration = ((duration /  10 % 10) != 0) ? (duration -  10) : (duration +  90); break;    
            case State_input::Duration_100: duration = ((duration / 100 % 10) != 0) ? (duration - 100) : (duration + 900); break;    
            default:
                break;
            }
            break; 
        case Type_input::Start_program: // Управление стартом программы
            break; 
        default:
            break;
        }
    }
#endif
}
void button_4_press() {
    up_time = !up_time;
#if defined( OLD_CODE )
    if(!input_data) {
        switch (type_input) {
        case Type_input::None:
            break; 
        case Type_input::Current_time:  // Ввод текущего времени
        case Type_input::Start_time:    // Ввод времени старта
        case Type_input::Duration:      // Ввод длительности сигнала
            if(state_input != State_input::Cancel)
                input_data = true;
            break;
        case Type_input::Start_program: // Управление стартом программы
            break; 
        default:
            break;
        }
    }
    else {
        input_data = !input_data;
    }
#endif
}
void button_4_clamped() {
    start = !start;
    if(start) {
        digitalWrite(LED_PIN, HIGH);
    }
    else {
        digitalWrite(LED_PIN, LOW);
    }
}

// String prt_str{"hello word!"};
char prt_str_ch_1[9] = {"00:00:00"};
char prt_str_ch_2[10] = {"t(+) w(+)"};
void print_in_display() {
    display.clear();
    // Heltec.display.write((uint8_t)'0');
    // Heltec.display.write((uint8_t)'1');
    // Heltec.display.write((uint8_t)'2');
    // Heltec.display.write("012");
    prt_str_ch_1[0] = current_time.get_hour()/10 + '0';
    prt_str_ch_1[1] = current_time.get_hour()%10 + '0';
    prt_str_ch_1[3] = current_time.get_minute()/10 + '0';
    prt_str_ch_1[4] = current_time.get_minute()%10 + '0';
    prt_str_ch_1[6] = current_time.get_second()/10 + '0';
    prt_str_ch_1[7] = current_time.get_second()%10 + '0';

    if(up_time) {
        prt_str_ch_2[2] = '+';
    }
    else {
        prt_str_ch_2[2] = '-';
    }
    if(start) {
        prt_str_ch_2[7] = '+';
    }
    else {
        prt_str_ch_2[7] = '-';
    }
    
    display.drawCharString(15, 0, prt_str_ch_1, 9);
    display.drawCharString(15, 30, prt_str_ch_2, 10);
    // display.drawCharString(0, 32, prt_str_ch, 19);
    // Heltec.display.write("456");
    // Heltec.display.write(print_data);
    display.display();
#if defined( OLD_CODE )
    String print_data{""};
    Heltec.display->clear();
    switch (type_input) {
    case Type_input::None: {
        switch (target_type_input) {
        case Type_input::None: {
            // Строка 1
            print_data = "Time = ";
            if(current_time.get_hour() < 10) print_data += '0'; print_data += String(current_time.get_hour()) + ":";
            if(current_time.get_minute() < 10) print_data += '0'; print_data += String(current_time.get_minute()) + ":";
            if(current_time.get_second() < 10) print_data += '0'; print_data += String(current_time.get_second());
            Heltec.display->drawString(0, 0, print_data);
            // Строка 2
            print_data = "Start = ";
            if(time_control.get_start_time().get_hour() < 10) print_data += '0'; print_data += String(time_control.get_start_time().get_hour()) + ":";
            if(time_control.get_start_time().get_minute() < 10) print_data += '0'; print_data += String(time_control.get_start_time().get_minute()) + ":";
            if(time_control.get_start_time().get_second() < 10) print_data += '0'; print_data += String(time_control.get_start_time().get_second());
            Heltec.display->drawString(0, 15, print_data);
            break; 
        }
        case Type_input::Current_time: {
            // Строка 1
            print_data = "Select input:";
            Heltec.display->drawString(0, 0, print_data);
            // Строка 2
            print_data = "- current time";
            Heltec.display->drawString(0, 15, print_data);
            break; 
        }
        case Type_input::Start_time: {
            // Строка 1
            print_data = "Select input:";
            Heltec.display->drawString(0, 0, print_data);
            // Строка 2
            print_data = "- start signal time";
            Heltec.display->drawString(0, 15, print_data);
            break; 
        }
        case Type_input::Duration: {
            // Строка 1
            print_data = "Select input:";
            Heltec.display->drawString(0, 0, print_data);
            // Строка 2
            print_data = "- duration signal";
            Heltec.display->drawString(0, 15, print_data);
            break; 
        }
        case Type_input::Start_program: {
            // Строка 1
            print_data = "Select input:";
            Heltec.display->drawString(0, 0, print_data);
            // Строка 2
            print_data = "- work system";
            Heltec.display->drawString(0, 15, print_data);
            break; 
        }
        default:
            break;
        }
        // Строка 3
        print_data = "Start work = ";
        if(start)
            print_data += "ON";
        else
            print_data += "OFF";
        Heltec.display->drawString(0, 30, print_data);
        // Строка 4
        print_data = "Signal = ";
        if(time_control.check_signal(current_time) && start)
            print_data += "ON";
        else
            print_data += "OFF";
        Heltec.display->drawString(0, 45, print_data);
        break; 
    }
    case Type_input::Current_time: {
        // Строка 1
        print_data = "Input current time";
        Heltec.display->drawString(0, 0, print_data);
        // Строка 2
        print_data = "";
        if(set_time.get_hour() < 10)   print_data += '0'; print_data += String(set_time.get_hour()) + ":";
        if(set_time.get_minute() < 10) print_data += '0'; print_data += String(set_time.get_minute()) + ":";
        if(set_time.get_second() < 10) print_data += '0'; print_data += String(set_time.get_second());
        Heltec.display->drawString(0, 17, print_data);
        // Строка 3
        switch (state_input) {
        case State_input::Hour:   Heltec.display->drawString( 6, 32, "^"); break;
        case State_input::Minute: Heltec.display->drawString(28, 32, "^"); break;
        case State_input::Second: Heltec.display->drawString(50, 32, "^"); break;
        default:
            break;
        }
        // Строка 4
        if(!input_data) {
            switch (state_input) {
            case State_input::Hour:
            case State_input::Minute:
            case State_input::Second:
                Heltec.display->drawString(0, 45, "Select or save");
                break;
            case State_input::Cancel:
                Heltec.display->drawString(0, 45, "Don't save?");
                break;
            default:
                break;
            }
        }
        else {
            switch (state_input) {
            case State_input::Hour:
            case State_input::Minute:
            case State_input::Second:
                Heltec.display->drawString(0, 45, "Input value");
                break;
            default:
                break;
            }
        }
        break;
    }
    case Type_input::Start_time: {
        // Строка 1
        print_data = "Input start time";
        Heltec.display->drawString(0, 0, print_data);
        // Строка 2
        print_data = "";
        if(set_time.get_hour() < 10)   print_data += '0'; print_data += String(set_time.get_hour()) + ":";
        if(set_time.get_minute() < 10) print_data += '0'; print_data += String(set_time.get_minute()) + ":";
        if(set_time.get_second() < 10) print_data += '0'; print_data += String(set_time.get_second());
        Heltec.display->drawString(0, 17, print_data);
        // Строка 3
        switch (state_input) {
        case State_input::Hour:   Heltec.display->drawString( 6, 32, "^"); break;
        case State_input::Minute: Heltec.display->drawString(28, 32, "^"); break;
        case State_input::Second: Heltec.display->drawString(50, 32, "^"); break;
        default:
            break;
        }
        // Строка 4
        if(!input_data) {
            switch (state_input) {
            case State_input::Hour:
            case State_input::Minute:
            case State_input::Second:
                Heltec.display->drawString(0, 45, "Select or save");
                break;
            case State_input::Cancel:
                Heltec.display->drawString(0, 45, "Don't save?");
                break;
            default:
                break;
            }
        }
        else {
            switch (state_input) {
            case State_input::Hour:
            case State_input::Minute:
            case State_input::Second:
                Heltec.display->drawString(0, 45, "Input value");
                break;
            default:
                break;
            }
        }
        break;
    }
    case Type_input::Duration: {
        // Строка 1
        print_data = "Input duration";
        Heltec.display->drawString(0, 0, print_data);
        // Строка 2
        print_data = "";
        if(duration < 100) print_data += '0';
        if(duration < 10)  print_data += '0';
        print_data += String(duration);
        print_data += " sec";
        Heltec.display->drawString(0, 17, print_data);
        // Строка 3
        switch (state_input) {
        case State_input::Duration_100: Heltec.display->drawString( 1, 32, "^"); break;
        case State_input::Duration_010: Heltec.display->drawString(10, 32, "^"); break;
        case State_input::Duration_001: Heltec.display->drawString(19, 32, "^"); break;
        default:
            break;
        }
        // Строка 4
        if(!input_data) {
            switch (state_input) {
            case State_input::Duration_001:
            case State_input::Duration_010:
            case State_input::Duration_100:
                Heltec.display->drawString(0, 45, "Select or save");
                break;
            case State_input::Cancel:
                Heltec.display->drawString(0, 45, "Don't save?");
                break;
            default:
                break;
            }
        }
        else {
            switch (state_input) {
            case State_input::Duration_001:
            case State_input::Duration_010:
            case State_input::Duration_100:
                Heltec.display->drawString(0, 45, "Input value");
                break;
            default:
                break;
            }
        }
        break;
    }
    case Type_input::Start_program: {
        // Строка 1
        print_data = "State work system";
        Heltec.display->drawString(0, 0, print_data);
        // Строка 2
        if(start)
            Heltec.display->drawString(0, 17, "Work");
        else
            Heltec.display->drawString(0, 17, "Sleep");

        // Строка 3
        switch (state_input) {
        case State_input::Cancel:
            Heltec.display->drawString( 0, 34, "Don't change state?");
            break;      
        default:
            Heltec.display->drawString( 0, 34, "Change state?");
            break;
        }
        break;
    }  
    default:
        break;
    }
    Heltec.display->display();
#endif
}


void display_begin() {
    display.init();
    //display->flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    // display.drawString(0, 0, "OLED initial done!");
    display.display();
}

#if defined( USE_SERIAL )
uint8_t get_byte_serial = 0;
void read_button() {
    if(Serial.available() != 0) {
        get_byte_serial = Serial.read();
        switch (get_byte_serial)
        {
        case '1':
            button_1_press();
            break;
        case '2':
            button_2_press();
            break;
        case '3':
            button_3_press();
            break;
        case '4':
            button_4_press();
            break;
        case '5':
            button_4_clamped();
            break;
        default:
            break;
        }
        switch (get_byte_serial)
        {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
            print_in_display();
            Serial.println((char)get_byte_serial);
        default:
            break;
        }
    }
}
#endif

#endif









#if defined( USE_STRING )

#include <Arduino.h>
#include <heltec.h>
#include <Time_control.h>

#define AMT_BUTTON 4

#define BUTTON_1 GPIO_NUM_36
#define BUTTON_2 GPIO_NUM_37
#define BUTTON_3 GPIO_NUM_38
#define BUTTON_4 GPIO_NUM_39

#define LAMP_PIN GPIO_NUM_17

void button_1_press();
void button_2_press();
void button_3_press();
void button_4_press();
void print_in_display();

enum class Type_input {
    None = 0,      // Ввод текущего времени
    Current_time,  // Ввод текущего времени
    Start_time,    // Ввод времени старта
    Duration,      // Ввод длительности сигнала
    Start_program, // Ввод длительности сигнала
};
enum class State_input {
    None = 0,     // отсутствует ввод
    Hour,         // значения часа
    Minute,       // значения минут
    Second,       // значения секунд
    Duration_100, // значения длительности
    Duration_010, // значения длительности
    Duration_001, // значения длительности
    On_lamp,      // включение света
    Cancel,       // отсутствует ввод
};

uint8_t button[4]{BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4};
bool button_state[4]{0, 0, 0, 0};
bool button_last_state[4]{0, 0, 0, 0};

tctrl::Time current_time;
tctrl::Time_control time_control;
tctrl::Time set_time;

bool start = false;
Type_input type_input{Type_input::None};
Type_input target_type_input{Type_input::None};
State_input state_input{State_input::None};
uint16_t duration = 0;
uint32_t sec_millis = 0;

bool input_data = false;

void setup() {
    Heltec.begin(true /*Display Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
    Heltec.display->init();
    Heltec.display->flipScreenVertically();
    Heltec.display->setFont(ArialMT_Plain_16);
    delay(1000);
    print_in_display();
    // Настройка выходов
    pinMode(LAMP_PIN, OUTPUT);
    for(int i = 0; i < AMT_BUTTON; ++i)
        pinMode(button[i], INPUT_PULLDOWN);
}

void loop() {
    for(int i = 0; i < AMT_BUTTON; ++i)
        button_state[i] = digitalRead(button[i]);
    for(int i = 0; i < AMT_BUTTON; ++i)
        if(button_last_state[i] != button_state[i]) {
            button_last_state[i] = button_state[i];
            if(!button_last_state[i]) {
                switch (i)
                {
                case 0: button_1_press(); break;
                case 1: button_2_press(); break;
                case 2: button_3_press(); break;
                case 3: button_4_press(); break;
                default: break;
                }
            }
            print_in_display();
        }
    if(millis() - sec_millis >= 1000) {
        sec_millis += 1000;
        current_time += tctrl::Time(1);
        if(time_control.check_signal(current_time) && start)
            digitalWrite(LAMP_PIN, HIGH);
        else
            digitalWrite(LAMP_PIN, LOW);
        print_in_display();
    }

    if(0) {
        Serial.print("Button = ");
        Serial.print(button_last_state[0]);
        Serial.print(", ");
        Serial.print(button_last_state[1]);
        Serial.print(", ");
        Serial.print(button_last_state[2]);
        Serial.print(", ");
        Serial.println(button_last_state[3]);

        Heltec.display->clear();
        // Heltec.display->drawString(0, 0, "Speed = " + String(pwm_percent) + "%");

        Heltec.display->drawString(10, 42, "{");
        for(int j = 0; j < AMT_BUTTON; ++j)
            if(button_last_state[j])
                Heltec.display->drawString(20 + j * 20, 30, "_");
            else
                Heltec.display->drawString(20 + j * 20, 40, "_");
        Heltec.display->drawString(94, 42, "}");
        Heltec.display->display();
    }
}


void button_1_press() {
    if(!input_data) {
        switch (type_input) {
        case Type_input::None:          // Выбор 
            type_input = target_type_input;
            switch (target_type_input) {
            case Type_input::None: break; 
            case Type_input::Current_time: 
                set_time = current_time;
                state_input = State_input::Second;
                break; 
            case Type_input::Start_time:
                set_time = time_control.get_start_time();
                state_input = State_input::Second;
                break; 
            case Type_input::Duration:
                duration = (time_control.get_end_time() - time_control.get_start_time()).get_sum_second();
                state_input = State_input::Duration_001;
                break; 
            case Type_input::Start_program:
                state_input = State_input::On_lamp;
                break;    
            default:
                break;
            }
            break; 
        case Type_input::Current_time:  // Ввод текущего времени
            current_time = set_time;
            type_input = Type_input::None;
            break; 
        case Type_input::Start_time:    // Ввод времени старта
            if(state_input != State_input::Cancel) {
                duration = (time_control.get_end_time() - time_control.get_start_time()).get_sum_second();
                time_control.set_start_time(set_time);
                time_control.set_duration_signal(duration);
            }
            type_input = Type_input::None;
            break; 
        case Type_input::Duration:      // Ввод длительности сигнала
            if(state_input != State_input::Cancel) {
                time_control.set_duration_signal(duration);
            }
            type_input = Type_input::None;
            break; 
        case Type_input::Start_program: // Управление стартом программы
            if(state_input != State_input::Cancel) {
                start = !start;
            }
            type_input = Type_input::None;
            break;    
        default:
            break;
        }
    }
    else
        input_data = false;
}
void button_2_press() {
    if(!input_data) {
        switch (type_input) {
        case Type_input::None:          // Выбор 
            switch (target_type_input) {
            case Type_input::None:          target_type_input = Type_input::Current_time;  break; 
            case Type_input::Current_time:  target_type_input = Type_input::Start_time;    break; 
            case Type_input::Start_time:    target_type_input = Type_input::Duration;      break; 
            case Type_input::Duration:      target_type_input = Type_input::Start_program; break; 
            case Type_input::Start_program: target_type_input = Type_input::None;          break;    
            default:
                break;
            }
            break; 
        case Type_input::Current_time:  // Ввод текущего времени
        case Type_input::Start_time:    // Ввод времени старта
            switch (state_input) {
            case State_input::Second: state_input = State_input::Minute; break;    
            case State_input::Minute: state_input = State_input::Hour;   break;    
            case State_input::Hour:   state_input = State_input::Cancel; break;    
            case State_input::Cancel: state_input = State_input::Second; break;    
            default:
                break;
            }
            break;
        case Type_input::Duration:      // Ввод длительности сигнала
            switch (state_input) {
            case State_input::Duration_001: state_input = State_input::Duration_010; break;    
            case State_input::Duration_010: state_input = State_input::Duration_100; break;    
            case State_input::Duration_100: state_input = State_input::Cancel;       break;    
            case State_input::Cancel:       state_input = State_input::Duration_001; break;    
            default:
                break;
            }
            break;
        case Type_input::Start_program: // Управление стартом программы
            switch (state_input) {
            case State_input::On_lamp: state_input = State_input::Cancel;  break;
            case State_input::Cancel:  state_input = State_input::On_lamp; break;    
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    else {
        switch (type_input) {
        case Type_input::None:
            break; 
        case Type_input::Current_time:  // Ввод текущего времени
        case Type_input::Start_time:    // Ввод времени старта
            switch (state_input) {
            case State_input::Second: set_time.set_second((tctrl::Time{set_time} += tctrl::Time(0, 0, 1)).get_second()); break;
            case State_input::Minute: set_time.set_minute((tctrl::Time{set_time} += tctrl::Time(0, 1, 0)).get_minute()); break;
            case State_input::Hour:   set_time.set_hour(  (tctrl::Time{set_time} += tctrl::Time(1, 0, 0)).get_hour()  ); break;
            default:
                break;
            }
            break; 
        case Type_input::Duration:      // Ввод длительности сигнала
            switch (state_input) {
            case State_input::Duration_001: duration = ((duration       % 10) != 9) ? (duration +   1) : (duration -   9); break;    
            case State_input::Duration_010: duration = ((duration /  10 % 10) != 9) ? (duration +  10) : (duration -  90); break;    
            case State_input::Duration_100: duration = ((duration / 100 % 10) != 9) ? (duration + 100) : (duration - 900); break;    
            default:
                break;
            }
            break; 
        case Type_input::Start_program: // Управление стартом программы
            break; 
        default:
            break;
        }
    }
}
void button_3_press() {
    if(!input_data) {
        switch (type_input) {
        case Type_input::None:          // Выбор 
            switch (target_type_input) {
            case Type_input::None:          target_type_input = Type_input::Start_program; break; 
            case Type_input::Current_time:  target_type_input = Type_input::None;          break; 
            case Type_input::Start_time:    target_type_input = Type_input::Current_time;  break; 
            case Type_input::Duration:      target_type_input = Type_input::Start_time;    break; 
            case Type_input::Start_program: target_type_input = Type_input::Duration;      break;    
            default:
                break;
            }
            break; 
        case Type_input::Current_time:  // Ввод текущего времени
        case Type_input::Start_time:    // Ввод времени старта
            switch (state_input) {
            case State_input::Second: state_input = State_input::Cancel; break;    
            case State_input::Minute: state_input = State_input::Second; break;    
            case State_input::Hour:   state_input = State_input::Minute; break;    
            case State_input::Cancel: state_input = State_input::Hour;   break;    
            default:
                break;
            }
            break; 
        case Type_input::Duration:      // Ввод длительности сигнала
            switch (state_input) {
            case State_input::Duration_001: state_input = State_input::Cancel;       break;    
            case State_input::Duration_010: state_input = State_input::Duration_001; break;    
            case State_input::Duration_100: state_input = State_input::Duration_010; break;    
            case State_input::Cancel:       state_input = State_input::Duration_100; break;    
            default:
                break;
            }
            break; 
        case Type_input::Start_program: // Управление стартом программы
            switch (state_input) {
            case State_input::On_lamp: state_input = State_input::Cancel;  break;
            case State_input::Cancel:  state_input = State_input::On_lamp; break;      
            default:
                break;
            }
            break; 
        default:
            break;
        }
    }
    else {
        switch (type_input) {
        case Type_input::None:
            break; 
        case Type_input::Current_time:  // Ввод текущего времени
        case Type_input::Start_time:    // Ввод времени старта
            switch (state_input) {
            case State_input::Second: set_time.set_second((tctrl::Time{set_time} -= tctrl::Time(0, 0, 1)).get_second()); break;
            case State_input::Minute: set_time.set_minute((tctrl::Time{set_time} -= tctrl::Time(0, 1, 0)).get_minute()); break;
            case State_input::Hour:   set_time.set_hour(  (tctrl::Time{set_time} -= tctrl::Time(1, 0, 0)).get_hour()  ); break;
            default:
                break;
            }
            break; 
        case Type_input::Duration:      // Ввод длительности сигнала
            switch (state_input) {
            case State_input::Duration_001: duration = ((duration       % 10) != 0) ? (duration -   1) : (duration +   9); break;    
            case State_input::Duration_010: duration = ((duration /  10 % 10) != 0) ? (duration -  10) : (duration +  90); break;    
            case State_input::Duration_100: duration = ((duration / 100 % 10) != 0) ? (duration - 100) : (duration + 900); break;    
            default:
                break;
            }
            break; 
        case Type_input::Start_program: // Управление стартом программы
            break; 
        default:
            break;
        }
    }
}
void button_4_press() {
    if(!input_data) {
        switch (type_input) {
        case Type_input::None:
            break; 
        case Type_input::Current_time:  // Ввод текущего времени
        case Type_input::Start_time:    // Ввод времени старта
        case Type_input::Duration:      // Ввод длительности сигнала
            if(state_input != State_input::Cancel)
                input_data = true;
            break;
        case Type_input::Start_program: // Управление стартом программы
            break; 
        default:
            break;
        }
    }
    else
        input_data = !input_data;
}
void print_in_display() {
    String print_data{""};
    Heltec.display->clear();

    switch (type_input) {
    case Type_input::None: {
        switch (target_type_input) {
        case Type_input::None: {
            // Строка 1
            print_data = "Time = ";
            if(current_time.get_hour() < 10) print_data += '0'; print_data += String(current_time.get_hour()) + ":";
            if(current_time.get_minute() < 10) print_data += '0'; print_data += String(current_time.get_minute()) + ":";
            if(current_time.get_second() < 10) print_data += '0'; print_data += String(current_time.get_second());
            Heltec.display->drawString(0, 0, print_data);
            // Строка 2
            print_data = "Start = ";
            if(time_control.get_start_time().get_hour() < 10) print_data += '0'; print_data += String(time_control.get_start_time().get_hour()) + ":";
            if(time_control.get_start_time().get_minute() < 10) print_data += '0'; print_data += String(time_control.get_start_time().get_minute()) + ":";
            if(time_control.get_start_time().get_second() < 10) print_data += '0'; print_data += String(time_control.get_start_time().get_second());
            Heltec.display->drawString(0, 15, print_data);
            break; 
        }
        case Type_input::Current_time: {
            // Строка 1
            print_data = "Select input:";
            Heltec.display->drawString(0, 0, print_data);
            // Строка 2
            print_data = "- current time";
            Heltec.display->drawString(0, 15, print_data);
            break; 
        }
        case Type_input::Start_time: {
            // Строка 1
            print_data = "Select input:";
            Heltec.display->drawString(0, 0, print_data);
            // Строка 2
            print_data = "- start signal time";
            Heltec.display->drawString(0, 15, print_data);
            break; 
        }
        case Type_input::Duration: {
            // Строка 1
            print_data = "Select input:";
            Heltec.display->drawString(0, 0, print_data);
            // Строка 2
            print_data = "- duration signal";
            Heltec.display->drawString(0, 15, print_data);
            break; 
        }
        case Type_input::Start_program: {
            // Строка 1
            print_data = "Select input:";
            Heltec.display->drawString(0, 0, print_data);
            // Строка 2
            print_data = "- work system";
            Heltec.display->drawString(0, 15, print_data);
            break; 
        }
        default:
            break;
        }
        // Строка 3
        print_data = "Start work = ";
        if(start)
            print_data += "ON";
        else
            print_data += "OFF";
        Heltec.display->drawString(0, 30, print_data);
        // Строка 4
        print_data = "Signal = ";
        if(time_control.check_signal(current_time) && start)
            print_data += "ON";
        else
            print_data += "OFF";
        Heltec.display->drawString(0, 45, print_data);
        break; 
    }
    case Type_input::Current_time: {
        // Строка 1
        print_data = "Input current time";
        Heltec.display->drawString(0, 0, print_data);
        // Строка 2
        print_data = "";
        if(set_time.get_hour() < 10)   print_data += '0'; print_data += String(set_time.get_hour()) + ":";
        if(set_time.get_minute() < 10) print_data += '0'; print_data += String(set_time.get_minute()) + ":";
        if(set_time.get_second() < 10) print_data += '0'; print_data += String(set_time.get_second());
        Heltec.display->drawString(0, 17, print_data);
        // Строка 3
        switch (state_input) {
        case State_input::Hour:   Heltec.display->drawString( 6, 32, "^"); break;
        case State_input::Minute: Heltec.display->drawString(28, 32, "^"); break;
        case State_input::Second: Heltec.display->drawString(50, 32, "^"); break;
        default:
            break;
        }
        // Строка 4
        if(!input_data) {
            switch (state_input) {
            case State_input::Hour:
            case State_input::Minute:
            case State_input::Second:
                Heltec.display->drawString(0, 45, "Select or save");
                break;
            case State_input::Cancel:
                Heltec.display->drawString(0, 45, "Don't save?");
                break;
            default:
                break;
            }
        }
        else {
            switch (state_input) {
            case State_input::Hour:
            case State_input::Minute:
            case State_input::Second:
                Heltec.display->drawString(0, 45, "Input value");
                break;
            default:
                break;
            }
        }
        break;
    }
    case Type_input::Start_time: {
        // Строка 1
        print_data = "Input start time";
        Heltec.display->drawString(0, 0, print_data);
        // Строка 2
        print_data = "";
        if(set_time.get_hour() < 10)   print_data += '0'; print_data += String(set_time.get_hour()) + ":";
        if(set_time.get_minute() < 10) print_data += '0'; print_data += String(set_time.get_minute()) + ":";
        if(set_time.get_second() < 10) print_data += '0'; print_data += String(set_time.get_second());
        Heltec.display->drawString(0, 17, print_data);
        // Строка 3
        switch (state_input) {
        case State_input::Hour:   Heltec.display->drawString( 6, 32, "^"); break;
        case State_input::Minute: Heltec.display->drawString(28, 32, "^"); break;
        case State_input::Second: Heltec.display->drawString(50, 32, "^"); break;
        default:
            break;
        }
        // Строка 4
        if(!input_data) {
            switch (state_input) {
            case State_input::Hour:
            case State_input::Minute:
            case State_input::Second:
                Heltec.display->drawString(0, 45, "Select or save");
                break;
            case State_input::Cancel:
                Heltec.display->drawString(0, 45, "Don't save?");
                break;
            default:
                break;
            }
        }
        else {
            switch (state_input) {
            case State_input::Hour:
            case State_input::Minute:
            case State_input::Second:
                Heltec.display->drawString(0, 45, "Input value");
                break;
            default:
                break;
            }
        }
        break;
    }
    case Type_input::Duration: {
        // Строка 1
        print_data = "Input duration";
        Heltec.display->drawString(0, 0, print_data);
        // Строка 2
        print_data = "";
        if(duration < 100) print_data += '0';
        if(duration < 10)  print_data += '0';
        print_data += String(duration);
        print_data += " sec";
        Heltec.display->drawString(0, 17, print_data);
        // Строка 3
        switch (state_input) {
        case State_input::Duration_100: Heltec.display->drawString( 1, 32, "^"); break;
        case State_input::Duration_010: Heltec.display->drawString(10, 32, "^"); break;
        case State_input::Duration_001: Heltec.display->drawString(19, 32, "^"); break;
        default:
            break;
        }
        // Строка 4
        if(!input_data) {
            switch (state_input) {
            case State_input::Duration_001:
            case State_input::Duration_010:
            case State_input::Duration_100:
                Heltec.display->drawString(0, 45, "Select or save");
                break;
            case State_input::Cancel:
                Heltec.display->drawString(0, 45, "Don't save?");
                break;
            default:
                break;
            }
        }
        else {
            switch (state_input) {
            case State_input::Duration_001:
            case State_input::Duration_010:
            case State_input::Duration_100:
                Heltec.display->drawString(0, 45, "Input value");
                break;
            default:
                break;
            }
        }
        break;
    }
    case Type_input::Start_program: {
        // Строка 1
        print_data = "State work system";
        Heltec.display->drawString(0, 0, print_data);
        // Строка 2
        if(start)
            Heltec.display->drawString(0, 17, "Work");
        else
            Heltec.display->drawString(0, 17, "Sleep");

        // Строка 3
        switch (state_input) {
        case State_input::Cancel:
            Heltec.display->drawString( 0, 34, "Don't change state?");
            break;      
        default:
            Heltec.display->drawString( 0, 34, "Change state?");
            break;
        }
        break;
    }  
    default:
        break;
    }
    Heltec.display->display();
}
#endif