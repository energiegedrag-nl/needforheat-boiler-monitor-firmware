#include <generic_esp_32.hpp>
#include <generic_tasks.hpp>
#include <scheduler.hpp>
#include <esp_log.h>
#include "driver/gpio.h" //for gpio

//For measurement upload to database
#include <measurements.hpp>
#include <secure_upload.hpp> 

constexpr const char *DEVICE_TYPE_NAME = "twomes-boiler-base-firmware";



#include <stdio.h>
#include "string.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "ds18b20-single.h" //Include library
#include "openTherm.h"
#include "openThermInit.h"
//#include "../lib/generic_esp_32.h"
#include "generic_esp_32.h"

const int DS_Pin1 = GPIO_NUM_25; //Temp sensor pins
const int DS_Pin2 = GPIO_NUM_26;

void getTemperatureSensors(void *taskInfo ) 
{ 
	auto taskName = Scheduler::GetName(taskInfo);	
	ESP_LOGI(taskName.c_str(), "getTemperatureSensors");
    float tempReading1 = 0;
    float tempReading2 = 0;

    ds18b20_init(DS_Pin1);
    tempReading1 = ds18b20_get_temp();
    ESP_LOGD("readTemp", "Temp1: %.6f", tempReading1);
    Measurements::Measurement::AddFormatter("boilerTemp1", "%.2f"); 
    Measurements::Measurement temp1("boilerTemp1", tempReading1);
    auto secureUploadQueue1 = SecureUpload::Queue::GetInstance();
    secureUploadQueue1.AddMeasurement(temp1);

    ds18b20_init(DS_Pin2);
    tempReading2 = ds18b20_get_temp();
    ESP_LOGD("readTemp", "Temp2: %.6f", tempReading2);
    Measurements::Measurement::AddFormatter("boilerTemp2", "%.2f"); 
    Measurements::Measurement temp2("boilerTemp2", tempReading2);
    auto secureUploadQueue2 = SecureUpload::Queue::GetInstance();
    secureUploadQueue2.AddMeasurement(temp2);
    
    // Stable readings require a brief period before communication
    vTaskDelay(2000.0 / portTICK_PERIOD_MS);
}

void getTemperatureSensorsTest(void *taskInfo ) 
{ 
	auto taskName = Scheduler::GetName(taskInfo);	
	ESP_LOGI(taskName.c_str(), "getTemperatureSensors");
    float tempReading1 = 0;
    float tempReading2 = 0;
    for(int i = 0; i < 10000; i++)
    {
        ds18b20_init(DS_Pin1);
        tempReading1 = ds18b20_get_temp();
        //ESP_LOGD("readTemp", "Temp1: %.6f", tempReading1);

        ds18b20_init(DS_Pin2);
        tempReading2 = ds18b20_get_temp();
        //ESP_LOGD("readTemp", "Temp2: %.6f", tempReading2);
        ESP_LOGI("temp", "%d, %.2f, %.2f", i, tempReading1, tempReading2);
        vTaskDelay(100.0 / portTICK_PERIOD_MS);
    }
    // Stable readings require a brief period before communication
    vTaskDelay(2000.0 / portTICK_PERIOD_MS);
}



void openThermTest(void *taskInfo)
{
    auto taskName = Scheduler::GetName(taskInfo);	
	ESP_LOGI(taskName.c_str(), "OpenThermTest");
}

extern "C" void app_main(void)
{
	// Add the generic tasks to the scheduler,
	// heartbeat, timesync and optionally presence detection and OTA updates.
	GenericTasks::AddTasksToScheduler();

	GenericESP32Firmware::Initialize(DEVICE_TYPE_NAME);
    GenericESP32Firmware::InitializeTimeSync(); //init timesync want de RTC heeft zonder power gezeten.

// Scheduler::AddTask(getTemperatureSensors, 
//         "getTemperatureSensors", 
//         4096, 
//         nullptr, 
//         2, 
//         Scheduler::Interval::MINUTES_1);

// Scheduler::AddTask(getTemperatureSensorsTest, 
//     "getTemperatureSensorsTest", 
//     4096, 
//     nullptr, 
//     30, 
//     Scheduler::Interval::MINUTES_1);

    Scheduler::AddTask(openThermTest, 
            "OpenThermTest", 
            4096, 
            nullptr, 
            1, 
            Scheduler::Interval::MINUTES_1);
	Scheduler::Start();

	ESP_LOGD("app_main", "Alle task gestart");

	gpio_install_isr_service(0);

    // ESP_LOGD(TAG, "calling initialize_intialize_button_and_led_gpio_gpio()");
    // intialize_button_and_led_gpio();

    ESP_LOGD(TAG, "calling intialize_opentherm_gpio()");
    intialize_opentherm_gpio();

    // twomes_device_provisioning(DEVICE_TYPE_NAME);

    // ESP_LOGD(TAG, "Starting heartbeat task");
    // xTaskCreatePinnedToCore(&heartbeat_task, "heartbeat_task", 4096, NULL, 1, NULL, 1);

    // ESP_LOGD(TAG, BOOT_STARTUP_INTERVAL_TXT);
    // vTaskDelay(BOOT_STARTUP_INTERVAL_MS / portTICK_PERIOD_MS);
    
    // ESP_LOGD(TAG, "Starting timesync task");
    // xTaskCreatePinnedToCore(&timesync_task, "timesync_task", 4096, NULL, 1, NULL, 1);

    // ESP_LOGD(TAG, BOOT_STARTUP_INTERVAL_TXT);
    // vTaskDelay(BOOT_STARTUP_INTERVAL_MS / portTICK_PERIOD_MS);

    // #ifdef CONFIG_TWOMES_PRESENCE_DETECTION
    // ESP_LOGD(TAG, "Starting presence detection");
    // start_presence_detection();

    // ESP_LOGD(TAG, BOOT_STARTUP_INTERVAL_TXT);
    // vTaskDelay(BOOT_STARTUP_INTERVAL_MS / portTICK_PERIOD_MS);

    // // additional startup delay to make sure the first rount of presence detection before starting OpenTherm
    // ESP_LOGD(TAG, BOOT_STARTUP_INTERVAL_TXT);
    // vTaskDelay(BOOT_STARTUP_INTERVAL_MS / portTICK_PERIOD_MS);

    // #endif
    // ESP_LOGD(TAG, "Generic Setup complete");

    ESP_LOGD(TAG, "calling begin_opentherm()");
    begin_opentherm();

    while (1) {
        sOT.process();
        //mOT.process();
        if (bufferSlaveCount != 0) {
            ESP_LOGD(TAG, "Slave Count: %d", bufferSlaveCount);
            processSendRequest(dataBufferSlave[--bufferSlaveCount]);
            // ESP_LOGD(TAG, "upload_timer_count: %d", upload_timer_count);
            // ESP_LOGD(TAG, "boiler_timer_count: %d", boiler_timer_count );
            // ESP_LOGD(TAG, "room_temp_timer_count: %d", room_temp_timer_count );
            // ESP_LOGD(TAG, "regular_timer_count: %d", regular_timer_count );
        }
        if (bufferMasterCount != 0) {
            ESP_LOGD(TAG, "Master Count: %d", bufferMasterCount);
            processSendRequest(dataBufferMaster[--bufferMasterCount]);
            // ESP_LOGD(TAG, "upload_timer_count: %d", upload_timer_count);
            // ESP_LOGD(TAG, "boiler_timer_count: %d", boiler_timer_count );
            // ESP_LOGD(TAG, "room_temp_timer_count: %d", room_temp_timer_count );
            // ESP_LOGD(TAG, "regular_timer_count: %d", regular_timer_count );
        }
        // if (upload_timer_count >= OPENTHERM_UPLOAD_INTERVAL) {
        //     // ESP_LOGE(TAG, "Uploading all buffered data!");
        //     // regular_measurement_count = 0;
        //     // boiler_measurement_count = 0;
        //     // room_temp_count = 0;
        //     upload_timer_count = 0;
        // }
        //boilerSupplyTemp & boilerboilerReturnTemp timer
        if (boiler_timer_count >= BOILER_MEASUREMENT_INTERVAL_S) {
            if (boilerSupplyTemp[0] == '\0' and boilerReturnTemp[0] == '\0') {
                ESP_LOGD(TAG, "No boilerSupplyTemp and no boilerReturnTemp measured; nothing to upload");
            } else {
                ESP_LOGD(TAG, "Measuring and uploading boilerSupplyTemp and boileReturnTemp");
                char *msg = get_boiler_measurements_data(time(NULL));
                boilerSupplyTemp[0] = '\0';
                boilerReturnTemp[0] = '\0';
                //Huub kan uit denk ik: upload_data_to_server(VARIABLE_UPLOAD_ENDPOINT, POST_WITH_BEARER, msg, NULL, 0);
                ESP_LOGD(TAG, "Free Heap before free(msg): %d", esp_get_free_heap_size());
                free(msg);
                ESP_LOGD(TAG, "Free Heap after free(msg): %d", esp_get_free_heap_size());
                // strcpy(boilerSupplyTemp_data[boiler_measurement_count], boilerSupplyTemp);
                // strcpy(boilerReturnTemp_data[boiler_measurement_count], boilerReturnTemp);
                // boiler_measurement_count++;
            }
            boiler_timer_count = 0;
        }
        if (room_temp_timer_count >= ROOM_TEMP_MEASUREMENT_INTERVAL_S) {
            if (roomTemp[0] == '\0' and  roomSetpoint[0] == '\0' and boilerMaxSupplyTemp[0] == '\0') {
                ESP_LOGD(TAG, "No roomTemp no roomSetpoint and no boilerMaxSupplyTemp measured; nothing to upload");
            } else {
                ESP_LOGD(TAG, "Measuring and uploading roomTemp, roomSetpoint and boilerMaxSupplyTemp");
                char *msg = get_5min_measurements_data(time(NULL));
                roomTemp[0] = '\0'; 
                roomSetpoint[0] = '\0';
                boilerMaxSupplyTemp[0] = '\0';
                //Huub: kan uit denk ik upload_data_to_server(VARIABLE_UPLOAD_ENDPOINT, POST_WITH_BEARER, msg, NULL, 0);
                ESP_LOGD(TAG, "Free Heap before free(msg): %d", esp_get_free_heap_size());
                free(msg);
                ESP_LOGD(TAG, "Free Heap after free(msg): %d", esp_get_free_heap_size());
            }
            room_temp_timer_count = 0;
        }
        if (regular_timer_count >= REGULAR_MEASUREMENT_INTERVAL) {
            if (ch_mode == -1 and dhw_mode == -1 and flame == -1 and maxRelModLvl == -1 and minModLvl == -1 and relModLvl == -1 and maxBoilerCap == -1) {
                ESP_LOGD(TAG, "No resular measurements; nothing to upload");
            } else {
                ESP_LOGD(TAG, "Measuring and uploading regular measurements");
                char *msg = get_regular_measurements_data(time(NULL));
                ch_mode = -1;
                dhw_mode = -1;
                flame = -1;
                maxRelModLvl = -1;
                minModLvl = -1;
                relModLvl = -1;
                maxBoilerCap = -1;
                //Huub: kan uit denk ik upload_data_to_server(VARIABLE_UPLOAD_ENDPOINT, POST_WITH_BEARER, msg, NULL, 0);
                ESP_LOGD(TAG, "Free Heap before free(msg): %d", esp_get_free_heap_size());
                free(msg);
                ESP_LOGD(TAG, "Free Heap after free(msg): %d", esp_get_free_heap_size());
                // memcpy(&ch_mode_data[regular_measurement_count], &ch_mode, sizeof(ch_mode));                       // Central heating mode (ON/OFF)
                // memcpy(&dhw_mode_data[regular_measurement_count], &dhw_mode, sizeof(dhw_mode));                    // Domestic hot water mode (ON/OFF)
                // memcpy(&flame_data[regular_measurement_count], &flame, sizeof(flame));                             // Burner status (ON/OFF)
                // strcpy(roomSetpoint_data[regular_measurement_count], roomSetpoint);                                // Return water temperature
                // strcpy(maxRelModLvl_data[regular_measurement_count], maxRelModLvl);                                // Maximum relative modulation level
                // memcpy(&minModLvl_data[regular_measurement_count], &minModLvl, sizeof(minModLvl));                 // Minimum modulation level
                // strcpy(relModLvl_data[regular_measurement_count], relModLvl);                                      // Relative modulation level
                // memcpy(&maxBoilerCapStorage_data[regular_measurement_count], &maxBoilerCap, sizeof(maxBoilerCap)); // Maximum boiler capacity
                // regular_measurement_count++;

            }
            regular_timer_count = 0;
        }
        // ESP_LOGD(TAG, "Upload Timer Count: %d", upload_timer_count);
        // ESP_LOGD(TAG, "Boiler Timer Count: %d", boiler_timer_count);
        // ESP_LOGD(TAG, "Room Temp Timer Count: %d", room_temp_timer_count);
        // ESP_LOGD(TAG, "Regular Timer Count: %d", regular_timer_count);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        // ESP_LOGD(TAG, "I'm looping in the opentherm loop!");
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}