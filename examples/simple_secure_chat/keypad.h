class Keypad {
public:
    Keypad();
    void begin();
    void update();
    bool hasNewInput() const { return new_input; }
    const char* getInput();
    const bool isCommandReady();
    int takeScrollDelta();
    void clear();
    bool isPoweringDown() const { return power_down; }
    bool newBattStatus(){
        if (batt_status != -1) return true;
        return false;
    }
    int getBattStatus() {
        int s = batt_status;
        batt_status = -1; 
        return s; 
    } 
    
private:
    char str_in[522];
    int len = 0;
    bool new_input = false;
    int last_key = 0;
    int key_cycle = 0;
    int scroll_delta = 0;
    bool uppercase = false;
    bool command_ready = false;
    bool power_down = false;
    int batt_status = -1;

    const char _1_key[3] = {'.', ',', '1'};
    const char _2_key[4] = {'a', 'b', 'c', '2'};
    const char _3_key[4] = {'d', 'e', 'f', '3'};
    const char _4_key[4] = {'g', 'h', 'i', '4'};
    const char _5_key[4] = {'j', 'k', 'l', '5'};
    const char _6_key[4] = {'m', 'n', 'o', '6'};
    const char _7_key[5] = {'p', 'q', 'r', 's', '7'};
    const char _8_key[4] = {'t', 'u', 'v', '8'};
    const char _9_key[5] = {'w', 'x', 'y', 'z', '9'};
};