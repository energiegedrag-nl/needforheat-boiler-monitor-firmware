#ifndef OPENTHERMINIT_H
#define OPENTHERMINIT_H

#include "openTherm.h"
#include <stdio.h>
#include "string.h"

const int mInPin = MASTER_IN_PIN_GPIO;  
const int mOutPin = MASTER_OUT_PIN_GPIO; //not conected

const int sInPin = SLAVE_IN_PIN_GPIO;
const int sOutPin = SLAVE_OUT_PIN_GPIO; //not connected

extern OpenTherm mOT;
extern OpenTherm sOT;

#define BOOT_STARTUP_INTERVAL_MS (10 * 1000) // milliseconds ( 10 s * 1000 ms/s)
#define BOOT_STARTUP_INTERVAL_TXT "Wating 10 seconds before next measurement data series is started"

#define OPENTHERM_MONITOR_VERSION "V1.0.0"
static const char *TAG = "Twomes ESP32 OpenTherm Monitor main";

extern char variable_upload_payload_template[];

static const char supply_temp_property_name[]       = "boilerSupplyTemp";
static const char return_temp_property_name[]       = "boilerReturnTemp";
static const char ch_mode_property_name[]           = "isCentralHeatingModeOn";
static const char dhw_mode_property_name[]          = "isDomesticHotWaterModeOn";
static const char flame_property_name[]             = "isBoilerFlameOn";
static const char room_setpoint_property_name[]     = "roomSetpointTemp";
static const char room_temp_property_name[]         = "roomTemp";
static const char max_rel_mod_lvl_property_name[]   = "maxModulationLevel";
static const char min_mod_lvl_property_name[]       = "minModulationLevel";
static const char rel_mod_lvl_property_name[]       = "relativeModulationLevel";
static const char max_boiler_cap_property_name[]    = "maxBoilerCap";
static const char max_supply_temp_property_name[]   = "boilerMaxSupplyTemp";

#define BOILER_MEASUREMENT_INTERVAL_S       10    // seconds
#define ROOM_TEMP_MEASUREMENT_INTERVAL_S    300   // seconds (5 min * 60 s/min)
#define REGULAR_MEASUREMENT_INTERVAL        30    // seconds
// #define OPENTHERM_UPLOAD_INTERVAL           900   // seconds (15 min * 60 s/min )
#define FLOAT_STRING_LENGTH 7
#define BOILER_MEASUREMENT_MAX OPENTHERM_UPLOAD_INTERVAL / BOILER_MEASUREMENT_INTERVAL_S
extern int boiler_measurement_count;

#define ROOM_TEMPERATURE_MEASUREMENT_MAX OPENTHERM_UPLOAD_INTERVAL / ROOM_TEMP_MEASUREMENT_INTERVAL_S
extern int room_temp_count;

#define REGULAR_MEASUREMENT_MAX OPENTHERM_UPLOAD_INTERVAL / REGULAR_MEASUREMENT_INTERVAL
extern int regular_measurement_count;

#define TIMER_DIVIDER 80

// in-memory buffers for interpreted values
// int ch_mode_data[REGULAR_MEASUREMENT_MAX];                                 // Central heating mode (ON/OFF)
// int dhw_mode_data[REGULAR_MEASUREMENT_MAX];                                // Domestic hot water mode (ON/OFF)
// int flame_data[REGULAR_MEASUREMENT_MAX];                                   // Burner status (ON/OFF)
// char roomSetpoint_data[REGULAR_MEASUREMENT_MAX][FLOAT_STRING_LENGTH];      // Room temperature setpoint
// char roomTemp_data[ROOM_TEMPERATURE_MEASUREMENT_MAX][FLOAT_STRING_LENGTH]; // Current room temperature
// char boilerSupplyTemp_data[BOILER_MEASUREMENT_MAX][FLOAT_STRING_LENGTH];         // Boiler water temperature
// char boilerReturnTemp_data[BOILER_MEASUREMENT_MAX][FLOAT_STRING_LENGTH];         // Return water temperature
// char maxRelModLvl_data[REGULAR_MEASUREMENT_MAX][FLOAT_STRING_LENGTH];      // Maximum relative modulation level
// int minModLvl_data[REGULAR_MEASUREMENT_MAX];                               // Minimum modulation level
// char relModLvl_data[REGULAR_MEASUREMENT_MAX][FLOAT_STRING_LENGTH];         // Relative modulation level
// int maxBoilerCapStorage_data[REGULAR_MEASUREMENT_MAX];                     // Maximum boiler capacity



extern int ch_mode;                          // Central heating mode (ON/OFF)
extern int dhw_mode;                          // Domestic hot water mode (ON/OFF)
extern int flame;                             // Burner status (ON/OFF)
extern char roomSetpoint[FLOAT_STRING_LENGTH];     // Room temperature setpoint
extern char roomTemp[FLOAT_STRING_LENGTH];         // Current room temperature
extern char boilerSupplyTemp[FLOAT_STRING_LENGTH]; // Boiler CH supply water temperature
extern char boilerReturnTemp[FLOAT_STRING_LENGTH]; // Boiler CH return water temperature
extern int maxRelModLvl;                      // Maximum relative modulation level
extern int minModLvl;                         // Minimum modulation level
extern int relModLvl;                         // Relative modulation level
extern int maxBoilerCap;                      // Maximum boiler capacity
extern char boilerMaxSupplyTemp[FLOAT_STRING_LENGTH]; // Maximum water temperature

// const char *ch_mode_name = "chmode";
// const char *dhw_mode_name = "dhwmode";
// const char *flame_name = "flame";
// const char *room_setpoint_name = "roomSetpoint";
// const char *room_temp_name = "roomTemp";
// const char *boiler_temp_name = "boilerSupplyTemp";
// const char *return_temp_name = "boilerReturnTemp";
// const char *max_rel_mod_lvl_name = "maxRelModLvl";
// const char *min_mod_lvl_name = "minModLvl";
// const char *rel_mod_lvl_name = "relModLvl";
// const char *max_boiler_cap_name = "maxBoilerCap";

extern char hex[5];     // Max 4 hex chars (two bytes) + null-terminator
extern int pointer; // Used to keep track of position of received character

extern char dataBufferSlave[10][10];
extern int bufferSlaveCount;

extern char dataBufferMaster[10][10];
extern int bufferMasterCount;

extern int regular_timer_count;
extern int boiler_timer_count;
extern int room_temp_timer_count;
extern int upload_timer_count;

extern int buffered_cycles_count;

// char *storage_template = (char *)"OpenTherm-%d";

typedef struct {
    int timer_group;
    int timer_idx;
    int alarm_interval;
    bool auto_reload;
} seconds_timer_info_t;

void ICACHE_RAM_ATTR mHandleInterrupt(void *);
void ICACHE_RAM_ATTR sHandleInterrupt(void *);

void processRequest(unsigned long request, OpenThermResponseStatus status);

void begin_opentherm();
void processSendRequest(char data[9]);
// char *get_boiler_measurements_data(uint32_t time);
// char *get_5min_measurements_data(uint32_t time);
// char *get_regular_measurements_data(uint32_t time);

std::string get_boiler_measurements_data(uint32_t time);
std::string get_5min_measurements_data(uint32_t time);
std::string get_regular_measurements_data(uint32_t time);
std::string get_property_string_char(const char *prop_name, uint32_t time, char *value);
std::string get_property_string_integer(const char *prop_name, uint32_t time, int value);
std::string get_boiler_measurements_data(uint32_t time);

#endif // OpenThermInit_h