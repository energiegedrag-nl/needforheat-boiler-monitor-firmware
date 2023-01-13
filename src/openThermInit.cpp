#include <string>
#include <stdlib.h>
#include "opentherm.h"
#include "driver/timer.h"
#include <cmath>
//#include "../lib/generic_esp_32.h" //Huub: copied from twomes-opentherm-monitor-firmware
#include "generic_esp_32.h" //Huub: copied from twomes-opentherm-monitor-firmware

#include "openThermInit.h"

int regular_timer_count = 0;
int boiler_timer_count = 0;
int room_temp_timer_count = 0;
int upload_timer_count = 0;

int bufferSlaveCount = 0;
char dataBufferSlave[10][10];
int bufferMasterCount = 0;
char dataBufferMaster[10][10];
char hex[5];     // Max 4 hex chars (two bytes) + null-terminator
int ch_mode = -1 ;                          // Central heating mode (ON/OFF)
int dhw_mode = -1;                          // Domestic hot water mode (ON/OFF)
int flame = -1;                             // Burner status (ON/OFF)
int relModLvl = -1;                         // Relative modulation level
char boilerSupplyTemp[FLOAT_STRING_LENGTH]; // Boiler CH supply water temperature
char boilerReturnTemp[FLOAT_STRING_LENGTH]; // Boiler CH return water temperature
char boilerMaxSupplyTemp[FLOAT_STRING_LENGTH]; // Maximum water temperature
int maxRelModLvl = -1;                      // Maximum relative modulation level
char roomSetpoint[FLOAT_STRING_LENGTH];     // Room temperature setpoint
char roomTemp[FLOAT_STRING_LENGTH];         // Current room temperature
int maxBoilerCap = -1;                      // Maximum boiler capacity
int minModLvl = -1;                         // Minimum modulation level
char variable_upload_payload_template[] = "{\"upload_time\": \"%d\",\"property_measurements\":[ "
    "%s"
    "]}";

int buffered_cycles_count = 0;
int pointer = 0; // Used to keep track of position of received character
int regular_measurement_count = 0;
int room_temp_count = 0;
int boiler_measurement_count = 0;

void ICACHE_RAM_ATTR mHandleInterrupt(void *) {
    mOT.handleInterrupt(NULL);
}

void ICACHE_RAM_ATTR sHandleInterrupt(void *) {
    sOT.handleInterrupt(NULL);
}

bool IRAM_ATTR timer_group_isr_callback(void *args) {
    BaseType_t high_task_awoken = pdFALSE;
    // example_timer_info_t *info = (example_timer_info_t *) args;

    // uint64_t timer_counter_value = timer_group_get_counter_value_in_isr(info->timer_group, info->timer_idx);

    /* Prepare basic event data that will be then sent back to task */
    // example_timer_event_t evt = {
    //     .info.timer_group = info->timer_group,
    //     .info.timer_idx = info->timer_idx,
    //     .info.auto_reload = info->auto_reload,
    //     .info.alarm_interval = info->alarm_interval,
    //     .timer_counter_value = timer_counter_value
    // };

    // if (!info->auto_reload) {
    //     timer_counter_value += info->alarm_interval * TIMER_SCALE;
    //     timer_group_set_alarm_value_in_isr(info->timer_group, info->timer_idx, timer_counter_value);
    // }

    /* Now just send the event data back to the main program task */
    // xQueueSendFromISR(s_timer_queue, &evt, &high_task_awoken);

    regular_timer_count++;
    boiler_timer_count++;
    room_temp_timer_count++;
    upload_timer_count++;
    return high_task_awoken == pdTRUE; // return whether we need to yield at the end of ISR
}

//int timer_interval in microseconds
void initialize_opentherm_timer(timer_group_t group, timer_idx_t timer, bool auto_reload, int timer_interval) {
    /* Select and initialize basic parameters of the timer */
    timer_config_t config = {
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = (timer_autoreload_t)auto_reload,
        .divider = TIMER_DIVIDER }; // default clock source is APB

    timer_init(group, timer, &config);

    timer_set_alarm_value(group, timer, timer_interval);
    timer_enable_intr(group, timer);

    // example_timer_info_t *timer_info = calloc(1, sizeof(example_timer_info_t));
    // timer_info->timer_group = group;
    // timer_info->timer_idx = timer;
    // timer_info->auto_reload = auto_reload;
    // timer_info->alarm_interval = timer_interval_sec;
    timer_isr_callback_add(group, timer, timer_group_isr_callback, NULL, 0);

    timer_start(group, timer);
}

void processRequest(unsigned long request, OpenThermResponseStatus status) {
    ESP_LOGD(TAG, "Request frame from thermostat: %lX", request); //TODO: Can ESP_LOGD  be used from an interrupt?
    char request_data[10];
    sprintf(request_data, "T%lX", request); 
    strcpy(dataBufferSlave[bufferSlaveCount++], request_data);
    unsigned long response = mOT.sendRequest(request);
    if (response) {
        ESP_LOGD(TAG, "Response frame from boiler: %lX", response); //TODO: Can ESP_LOGD  be used from an interrupt?
        char response_data[10];
        sprintf(response_data, "B%lX", response); 
        strcpy(dataBufferMaster[bufferMasterCount++], response_data);
    }
}

void processSendRequest(char data[9]) {
    ESP_LOGD(TAG, "Processing data: %c%c%c%c%c%c%c%c%c",
        data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
    // http://otgw.tclcode.com/firmware.html#operation
    // T = received from thermostat
    // B = received from boiler
    // R = sent to boiler
    // A = sent to thermostat
    // E = error

    // Only process messages received from thermostat or boiler, since we're in listening mode (not sending anything)
    // WARNING: one possible issue is BV which stands for Bad Value and also starts with B.

    // https://www.domoticaforum.eu/uploaded/Ard%20M/Opentherm%20Protocol%20v2-2.pdf
    // Messages are received in 4 bytes, converted to hex for a total of 8 characters

    // Bit 5, 6 and 7 of the the first byte (counted from LSB) contains the Message Type:
    // Master-to-Slave Messages      Slave-to-Master Messages
    // Value Message type            Value Message type
    // 000   READ-DATA               100 READ-ACK
    // 001   WRITE-DATA              101 WRITE-ACK
    // 010   INVALID-DATA            110 DATA-INVALID
    // 011   -reserved-              111 UNKNOWN-DATAID

    // Since we're only interested in gathering actual values, we filter on READ-ACK and WRITE-ACK.
    strncpy(hex, &data[1], 2);           // Copy 2 chars, starting from 1st character
    hex[2] = '\0';                       // null-terminated string
    int msgType = strtol(hex, NULL, 16); // Convert hex to dec

    // Now we need to take bit 5, 6 and 7. One way of doing that, is bit-shifting to the right by 4 places
    // so that ?xxx???? becomes 0000?xxx where xxx are the bits we're after.
    msgType = msgType >> 4;

    // And then bitwise AND compare it with 7, which leaves us with the first 3 bits:
    // Take the shifted msgType value   0000?xxx
    // Take the number 7                00000111
    // Bitwise AND compare              -------- &
    // Results in                       00000xxx
    msgType = msgType & 7;

    // Another option would be to bitwise AND compare it with 01110000 (112) without shifting,
    // and check the result for 01000000 (64 for READ-ACK) or 01010000 (80 for WRITE-ACK)

    // The second byte (char 3 and 4 if you don't take into account TBRAE) holds the Data-ID
    strncpy(hex, &data[3], 2);          // Copy 2 chars, starting from 3rd character
    hex[2] = '\0';                      // null-terminated string
    int dataId = strtol(hex, NULL, 16); // Convert hex to dec

    // Filter on specific Data ID's
    // 0 = STATUS (HB: thermostat; LB: boiler)
    // 14 = CAPACITY SETTING
    // 15 = MAX CAPACITY / MIN-MOD-LEVEL (HB/LB) [U8/U8]
    // 16 = ROOM SETPOINT [F8.8]
    // 24 = ROOM TEMPERATURE [F8.8]
    // 25 = BOILER WATER TEMP. [F8.8]
    // 28 = RETURN WATER TEMPERATURE [F8.8]
    // 54 = MAX CH WATER SETPOINT [F8.8]
    
    if (dataId == 0 and (msgType == 4 || msgType == 5)) {
        // Filter on READ-ACK (100 = 4) and WRITE-ACK (101 = 5)

        ESP_LOGD(TAG, "Processing Slave Status");
        // bit : description            [clear/0, set/1]
        // 0   : fault indication       [no fault, fault]
        // 1   : CH mode                [CH not active, CH active]
        // 2   : DHW mode               [DHW not active, DHW active]
        // 3   : Flame status           [flame off, flame on]
        // 4   : Cooling status         [cooling mode not active, cooling mode active]
        // 5   : CH2 mode               [CH2 not active, CH2 active]
        // 6   : diagnostic indication  [no diagnostics, diagnostic event]
        // 7   : reserved

        strncpy(hex, &data[7], 2);           // Copy 2 characters, starting from 7th char of the message (effectively take the last 2 hex chars: the low byte)
        hex[2] = '\0';                       // null-terminated string
        int lowbyte = strtol(hex, NULL, 16); // Convert hex to dec

        // Bitwise compare with 00000010 (2) to get bit number 2. The result is either 2 or 0.
        ch_mode = ((lowbyte & 2) == 2) ? 1 : 0;
        
        // Bitwise compare with 00000100 (4) to get bit number 3. The result is either 4 or 0.
        dhw_mode = ((lowbyte & 4) == 4) ? 1 : 0;

        // Bitwise compare with 00001000 (8) to get bit number 4. The result is either 8 or 0.
        flame = ((lowbyte & 8) == 8) ? 1 : 0;
        ESP_LOGD(TAG, "Processing ch_mode %d dhw_mode %d, flame %d", ch_mode, dhw_mode, flame);
    }
    else if (dataId == 14 || dataId == 16 || dataId == 17 || dataId == 24 || dataId == 25 || dataId == 28 || dataId == 57) {
        ESP_LOGD(TAG, "Processing regular data values");
        // These values are f8.8 signed fixed point value : 1 sign bit, 7 integer bit, 8 fractional bits
        // (two’s compliment ie. the LSB of the 16 bit binary number represents 1/256th of a unit).
        strncpy(hex, &data[5], 4); // Copy 4 characters, starting from 5th char of the message (or: take the last 4 chars of the message)

        hex[4] = '\0';                    // null-terminated string
        long lng = strtol(hex, NULL, 16); // Convert hex to dec

        if ((lng & 32768) == 32768) { // if the first bit (the sign bit) is a 1, then this a negative value (again, same trick as above)
            lng = (65536 - lng) * -1;
        }

        float tmp = (float)lng / 256.0; // As the last 8 bits are fractional, divide by 2^8 = 256

        if (msgType == 4 || msgType == 5) {
            // Filter on READ-ACK (100 = 4) and WRITE-ACK (101 = 5)
            // Register the property value as a floating point string with 2 decimals
            // or as an integer string for some properties
            switch (dataId) {
                case 17:
                    relModLvl = (int) round(tmp);
                    ESP_LOGD(TAG, "Processing relModLvl %i", relModLvl);
                    break;
                case 25:
                    sprintf(boilerSupplyTemp, "%.2f", tmp);
                    ESP_LOGD(TAG, "Processing boilerSupplyTemp %s", boilerSupplyTemp);
                    break;
                case 28:
                    sprintf(boilerReturnTemp, "%.2f", tmp);
                    ESP_LOGD(TAG, "Processing boilerReturnTemp %s", boilerReturnTemp);
                    break;
                case 57:
                    sprintf(boilerMaxSupplyTemp, "%.2f", tmp);
                    ESP_LOGD(TAG, "Processing boilerMaxSupplyTemp %s", boilerMaxSupplyTemp);
                    break;
                default:
                    break;
            }
        }
        else if (msgType == 0 || msgType == 1) { 
            // Filter on READ-DATA (000 = 0) and WRITE-DATA (001 = 1)
            // Register the property value as a floating point string with 2 decimals
            // or as an integer string for some properties
            switch (dataId) {
                case 14:
                    maxRelModLvl = (int) round(tmp);
                    ESP_LOGD(TAG, "Processing maxRelModLvl %i", maxRelModLvl);
                    break;
                case 16:
                    sprintf(roomSetpoint, "%.2f", tmp);
                    ESP_LOGD(TAG, "Processing roomSetpoint %s", roomSetpoint);
                    break;
                case 24:
                    sprintf(roomTemp, "%.2f", tmp);
                    ESP_LOGD(TAG, "Processing roomTemp %s", roomTemp);
                    break;
                default:
                    break;
            }
        }
    }
    else if (dataId == 15 and (msgType == 4 || msgType == 5)) {
        // Filter on READ-ACK (100 = 4) and WRITE-ACK (101 = 5)
        // HB : max. boiler capacity       u8     0..255kW
        // LB : min. modulation level      u8     0..100% expressed as a percentage of the maximum capacity

        strncpy(hex, &data[5], 2);            // Copy 2 characters, starting from 5th char of the message (high byte)
        hex[2] = '\0';                        // null-terminated string
        maxBoilerCap = strtol(hex, NULL, 16); // Straight forward unsigned 8 bit integer. Simply convert from hex to dec.

        strncpy(hex, &data[7], 2);         // Copy 2 characters, starting from 7th char of the message (low byte)
        hex[2] = '\0';                     // null-terminated string
        minModLvl = strtol(hex, NULL, 16); // Same here, 8 bit signed integer, so just convert from hex to dec.
        ESP_LOGD(TAG, "Processing maxBoilerCap %d and minModLvl %d", maxBoilerCap, minModLvl);
    }
}

//Function to initialise the buttons and LEDs, with interrupts on the buttons
void intialize_button_and_led_gpio() {
    ESP_LOGD(TAG, "starting intialize_button_and_led_gpio();");

    gpio_config_t io_conf;
    //CONFIGURE OUTPUTS:
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = BUTTON_AND_LED_OUTPUT_BITMASK;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    //CONFIGURE INPUTS:
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = BUTTON_AND_LED_INPUT_BITMASK;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    ESP_LOGD(TAG, "exiting intialize_button_and_led_gpio()");

}

void begin_opentherm() {

    ESP_LOGD(TAG, "calling start_opentherm_interrupts_handling()");
    start_opentherm_interrupt_handling();

    ESP_LOGD(TAG, "calling mOT.begin(mHandleInterrupt, processMasterRequest)");
    mOT.begin(mHandleInterrupt);
    ESP_LOGD(TAG, "sOT.begin(sHandleInterrupt, processSlaveRequest);");
    sOT.begin(sHandleInterrupt, processRequest);

    ESP_LOGD(TAG, "calling initialize_opentherm_timer()");
    initialize_opentherm_timer(TIMER_GROUP_0, TIMER_0, true, 1000000);

    ESP_LOGD(TAG, "starting main OpenTherm processing loop");

    
}

char *get_property_string_char(const char *prop_name, uint32_t time, char *value) {
    
    if (value == NULL or value[0]=='\0') {
         char *empty_str = (char *)malloc(1);
         empty_str[0] = '\0';
         return empty_str;
    } else {
        char property_string_char[] = "{\"property_name\": \"%s\","
            "\"measurements\": ["
            "{ \"timestamp\":\"%d\","
            "\"value\":\"%s\"}]}";

        int property_size = variable_sprintf_size(property_string_char, 3, prop_name, time, value);
        //Allocating enough memory so inputting the variables into the string doesn't overflow
        char *property_str = (char *)malloc(property_size);
        //Inputting variables into the plain json string from above(msgPlain).
        snprintf(property_str, property_size, property_string_char, prop_name, time, value);
        return property_str;
    }
}

char *get_property_string_integer(const char *prop_name, uint32_t time, int value) {
    if (value == -1) {
         char *empty_str = (char *)malloc(1);
         empty_str[0] = '\0';
         return empty_str;
    } else {
        char property_string_integer[] = "{\"property_name\": \"%s\","
            "\"measurements\": ["
            "{ \"timestamp\":\"%d\","
            "\"value\":\"%d\"}]}";

        int property_size = variable_sprintf_size(property_string_integer, 3, prop_name, time, value);
        //Allocating enough memory so inputting the variables into the string doesn't overflow
        char *property_str = (char *)malloc(property_size);
        //Inputting variables into the plain json string from above(msgPlain).
        snprintf(property_str, property_size, property_string_integer, prop_name, time, value);
        return property_str;
    }
}

char *get_boiler_measurements_data(uint32_t time) {
    //Get size of the message after inputting variables.
    std::string supply_temp_property(get_property_string_char(supply_temp_property_name, time, boilerSupplyTemp));
    std::string return_temp_property(get_property_string_char(return_temp_property_name, time, boilerReturnTemp));

    std::string combined_properties = "";
    if (!supply_temp_property.empty()) {
        combined_properties = combined_properties + supply_temp_property + ", ";
    }
    if (!return_temp_property.empty()) {
        combined_properties = combined_properties + return_temp_property + ", ";
    }
    if (!combined_properties.empty()) {
        combined_properties = combined_properties.substr(0,combined_properties.size()-2);
    }

    int msgSize = variable_sprintf_size(variable_upload_payload_template, 2, time, combined_properties.c_str());
    char *msg = (char *)malloc(msgSize);
    snprintf(msg, msgSize, variable_upload_payload_template, time, combined_properties.c_str());
    ESP_LOGD(TAG, "Data: %s", msg);

    // memcpy(&ch_mode_data[regular_measurement_count], &ch_mode, sizeof(ch_mode));                       // Central heating mode (ON/OFF)
    // memcpy(&dhw_mode_data[regular_measurement_count], &dhw_mode, sizeof(dhw_mode));                    // Domestic hot water mode (ON/OFF)
    // memcpy(&flame_data[regular_measurement_count], &flame, sizeof(flame));                             // Burner status (ON/OFF)
    // strcpy(roomSetpoint_data[regular_measurement_count], roomSetpoint);                                // Return water temperature
    // strcpy(maxRelModLvl_data[regular_measurement_count], maxRelModLvl);                                // Maximum relative modulation level
    // memcpy(&minModLvl_data[regular_measurement_count], &minModLvl, sizeof(minModLvl));                 // Minimum modulation level
    // strcpy(relModLvl_data[regular_measurement_count], relModLvl);                                      // Relative modulation level
    // memcpy(&maxBoilerCapStorage_data[regular_measurement_count], &maxBoilerCap, sizeof(maxBoilerCap));
    // free(boiler_temp_property);
    // free(return_temp_property);
    return msg;
}

char *get_5min_measurements_data(uint32_t time) {
    //Get size of the message after inputting variables.
    std::string room_setpoint_property(get_property_string_char(room_setpoint_property_name, time, roomSetpoint));
    std::string room_temp_property(get_property_string_char(room_temp_property_name, time, roomTemp));
    std::string max_supply_temp_property(get_property_string_char(max_supply_temp_property_name, time, boilerMaxSupplyTemp));


    std::string combined_properties = "";
    if (!room_setpoint_property.empty()) {
        combined_properties = combined_properties + room_setpoint_property + ", ";
    }
    if (!room_temp_property.empty()) {
        combined_properties = combined_properties + room_temp_property + ", ";
    }
    if (!max_supply_temp_property.empty()) {
        combined_properties = combined_properties + max_supply_temp_property + ", ";
    }
    if (!combined_properties.empty()) {
        combined_properties = combined_properties.substr(0,combined_properties.size()-2);
    }

    int msgSize = variable_sprintf_size(variable_upload_payload_template, 2, time, combined_properties.c_str());
    char *msg = (char *)malloc(msgSize);
    snprintf(msg, msgSize, variable_upload_payload_template, time, combined_properties.c_str());
    ESP_LOGD(TAG, "Data: %s", msg);

    // memcpy(&ch_mode_data[regular_measurement_count], &ch_mode, sizeof(ch_mode));                       // Central heating mode (ON/OFF)
    // memcpy(&dhw_mode_data[regular_measurement_count], &dhw_mode, sizeof(dhw_mode));                    // Domestic hot water mode (ON/OFF)
    // memcpy(&flame_data[regular_measurement_count], &flame, sizeof(flame));                             // Burner status (ON/OFF)
    // strcpy(roomSetpoint_data[regular_measurement_count], roomSetpoint);                                // Return water temperature
    // strcpy(maxRelModLvl_data[regular_measurement_count], maxRelModLvl);                                // Maximum relative modulation level
    // memcpy(&minModLvl_data[regular_measurement_count], &minModLvl, sizeof(minModLvl));                 // Minimum modulation level
    // strcpy(relModLvl_data[regular_measurement_count], relModLvl);                                      // Relative modulation level
    // memcpy(&maxBoilerCapStorage_data[regular_measurement_count], &maxBoilerCap, sizeof(maxBoilerCap));
    // free(boiler_temp_property);
    // free(return_temp_property);
    return msg;
}

char *get_regular_measurements_data(uint32_t time) {
    //Get size of the message after inputting variables.
    std::string ch_mode_property(get_property_string_integer(ch_mode_property_name, time, ch_mode));
    std::string dhw_mode_property(get_property_string_integer(dhw_mode_property_name, time, dhw_mode));
    std::string flame_property(get_property_string_integer(flame_property_name, time, flame));
    std::string max_rel_mod_lvl_property(get_property_string_integer(max_rel_mod_lvl_property_name, time, maxRelModLvl));
    std::string min_mod_lvl_property(get_property_string_integer(min_mod_lvl_property_name, time, minModLvl));
    std::string rel_mod_lvl_property(get_property_string_integer(rel_mod_lvl_property_name, time, relModLvl));
    std::string max_boiler_cap_property(get_property_string_integer(max_rel_mod_lvl_property_name, time, maxBoilerCap));

    std::string combined_properties = "";
    if (!ch_mode_property.empty()) {
        combined_properties = combined_properties + ch_mode_property + ", ";
    }
    if (!dhw_mode_property.empty()) {
        combined_properties = combined_properties + dhw_mode_property + ", ";
    }
    if (!flame_property.empty()) {
        combined_properties = combined_properties + flame_property + ", ";
    }
    if (!max_rel_mod_lvl_property.empty()) {
        combined_properties = combined_properties + max_rel_mod_lvl_property + ", ";
    }
    if (!min_mod_lvl_property.empty()) {
        combined_properties = combined_properties + min_mod_lvl_property + ", ";
    }
    if (!rel_mod_lvl_property.empty()) {
        combined_properties = combined_properties + rel_mod_lvl_property + ", ";
    }
    if (!max_boiler_cap_property.empty()) {
        combined_properties = combined_properties + max_boiler_cap_property + ", ";
    }
    if (!combined_properties.empty()) {
        combined_properties = combined_properties.substr(0,combined_properties.size()-2);
    }

    int msgSize = variable_sprintf_size(variable_upload_payload_template, 2, time, combined_properties.c_str());
    char *msg = (char *)malloc(msgSize);
    snprintf(msg, msgSize, variable_upload_payload_template, time, combined_properties.c_str());
    ESP_LOGD(TAG, "Data: %s", msg);


    // memcpy(&ch_mode_data[regular_measurement_count], &ch_mode, sizeof(ch_mode));                       // Central heating mode (ON/OFF)
    // memcpy(&dhw_mode_data[regular_measurement_count], &dhw_mode, sizeof(dhw_mode));                    // Domestic hot water mode (ON/OFF)
    // memcpy(&flame_data[regular_measurement_count], &flame, sizeof(flame));                             // Burner status (ON/OFF)
    // strcpy(roomSetpoint_data[regular_measurement_count], roomSetpoint);                                // Return water temperature
    // strcpy(maxRelModLvl_data[regular_measurement_count], maxRelModLvl);                                // Maximum relative modulation level
    // memcpy(&minModLvl_data[regular_measurement_count], &minModLvl, sizeof(minModLvl));                 // Minimum modulation level
    // strcpy(relModLvl_data[regular_measurement_count], relModLvl);                                      // Relative modulation level
    // memcpy(&maxBoilerCapStorage_data[regular_measurement_count], &maxBoilerCap, sizeof(maxBoilerCap));
    return msg;
}