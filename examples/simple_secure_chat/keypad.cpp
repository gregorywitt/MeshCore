#include "keypad.h"
#include <HardwareSerial.h>

Keypad::Keypad() {
    str_in[1] = '\0';
}

void Keypad::begin() {
    Serial1.begin(9600, SERIAL_8N1, D7, D6);
}

void Keypad::update() {
    while (Serial1.available()) {
        if(len >= 50){
            len = 0;
            str_in[0] = '\0';
        }

        int key = Serial1.read();
        //Serial.print("key: ");
        //Serial.println(key);

        if (key == 3) {
            scroll_delta++;
            key_cycle = 0;
            last_key = key;
            continue;
        }

        if (key == 0) {
            scroll_delta--;
            key_cycle = 0;
            last_key = key;
            continue;
        }

        if (key == 4) {
            uppercase = !uppercase;
            key_cycle = 0;
            new_input = true;
            last_key = key;
            continue;
        }

        // left arrow button
        if (key == 16) {
            if (str_in[len] != '\0') {
                str_in[len] = '\0';
                if (len > 0) {
                    len--;
                }
            }
            key_cycle = 0;
            new_input = true;
            last_key = key;
            continue;
        }
       
        // right arrow button
        if (key == 15) { 
            if(str_in[len] == '\0') str_in[len] = ' ';
            len++;
            key_cycle = 0;
            str_in[len] = '\0';
        }
        
        // advance cursor if change key
        if (str_in[len] != '\0' && key != last_key){
            len++;
            key_cycle = 0;
            str_in[len] = '\0';
        }
        
        if (key == 14) { str_in[len] = _1_key[key_cycle]; key_cycle++; if (key_cycle == 3) key_cycle = 0; }
        if (key == 13) { str_in[len] = _2_key[key_cycle]; key_cycle++; if (key_cycle == 4) key_cycle = 0; }
        if (key == 12) { str_in[len] = _3_key[key_cycle]; key_cycle++; if (key_cycle == 4) key_cycle = 0; }
        if (key == 11) { str_in[len] = _4_key[key_cycle]; key_cycle++; if (key_cycle == 4) key_cycle = 0; }
        if (key == 10) { str_in[len] = _5_key[key_cycle]; key_cycle++; if (key_cycle == 4) key_cycle = 0; }
        if (key ==  9) { str_in[len] = _6_key[key_cycle]; key_cycle++; if (key_cycle == 4) key_cycle = 0; }
        if (key ==  8) { str_in[len] = _7_key[key_cycle]; key_cycle++; if (key_cycle == 5) key_cycle = 0; }
        if (key ==  7) { str_in[len] = _8_key[key_cycle]; key_cycle++; if (key_cycle == 4) key_cycle = 0; }
        if (key ==  6) { str_in[len] = _9_key[key_cycle]; key_cycle++; if (key_cycle == 5) key_cycle = 0; }

        // change to uppercase if needed
        if (uppercase && key >= 6 && key <= 14 &&
            str_in[len] >= 'a' && str_in[len] <= 'z') {
            str_in[len] = str_in[len] - 'a' + 'A';
        }
        
        // enter button
        if (key ==  2) { key_cycle = 0; command_ready = true; }

        if (key == 99) power_down = true;

        if (key >= 100 && key <= 114){
            batt_status = key - 100;
        }

        new_input = true;

        last_key = key;

        str_in[len+1] = '\0';
    }
}

const char* Keypad::getInput() {
    new_input = false;
    return str_in;
}   

const bool Keypad::isCommandReady() {
    if (command_ready) {
        command_ready = false;
        return true;
    }
    return false;
}

int Keypad::takeScrollDelta() {
    const int delta = scroll_delta;
    scroll_delta = 0;
    return delta;
}

void Keypad::clear() {
    len = 0;
    str_in[0] = '\0';
    key_cycle = 0;
    scroll_delta = 0;
    command_ready = false;
}