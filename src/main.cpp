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
    Measurements::Measurement::AddFormatter("temp1__degC", "%.2f"); 
    Measurements::Measurement temp1("temp1__degC", tempReading1);
    auto secureUploadQueue1 = SecureUpload::Queue::GetInstance();
    secureUploadQueue1.AddMeasurement(temp1);

    ds18b20_init(DS_Pin2);
    tempReading2 = ds18b20_get_temp();
    ESP_LOGD("readTemp", "Temp2: %.6f", tempReading2);
    Measurements::Measurement::AddFormatter("temp2__degC", "%.2f"); 
    Measurements::Measurement temp2("temp2__degC", tempReading2);
    auto secureUploadQueue2 = SecureUpload::Queue::GetInstance();
    secureUploadQueue2.AddMeasurement(temp2);
    
    // Stable readings require a brief period before communication
    vTaskDelay(2000.0 / portTICK_PERIOD_MS);
}

unsigned int fiveMinuteCounter = 0;
extern "C" void app_main(void)
{
	// Add the generic tasks to the scheduler,
	// heartbeat, timesync and optionally presence detection and OTA updates.
	GenericTasks::AddTasksToScheduler();

	GenericESP32Firmware::Initialize(DEVICE_TYPE_NAME);
    GenericESP32Firmware::InitializeTimeSync(); //init timesync want de RTC heeft zonder power gezeten.

    Scheduler::AddTask(getTemperatureSensors, 
            "getTemperatureSensors", 
            4096, 
            nullptr, 
            2, 
            Scheduler::Interval::MINUTES_5);

    Scheduler::Start();

	ESP_LOGD("app_main", "Alle task gestart");

	gpio_install_isr_service(0);

    ESP_LOGD(TAG, "calling intialize_opentherm_gpio()");
    intialize_opentherm_gpio();

    ESP_LOGD(TAG, "calling begin_opentherm()");
    begin_opentherm();
    bool forceGetMeasurements = 0; //If 1 then skip the empty value checks and force the upload of empty data for testing.
    while (1) 
    {
        sOT.process();
        //mOT.process();
        if (bufferSlaveCount != 0) 
        {
            ESP_LOGD(TAG, "Slave Count: %d", bufferSlaveCount);
            processSendRequest(dataBufferSlave[--bufferSlaveCount]);
        }
        if (bufferMasterCount != 0) 
        {
            ESP_LOGD(TAG, "Master Count: %d", bufferMasterCount);
            processSendRequest(dataBufferMaster[--bufferMasterCount]);
        }
        if (boiler_timer_count >= BOILER_MEASUREMENT_INTERVAL_S) 
        {
            if ((boilerSupplyTemp[0] == '\0' and boilerReturnTemp[0] == '\0') && !forceGetMeasurements) 
            {
                ESP_LOGD(TAG, "No boilerSupplyTemp and no boilerReturnTemp measured");
            } 
            else 
            {
                ESP_LOGD(TAG, "Measuring boilerSupplyTemp and boileReturnTemp");
                //char *msg = get_boiler_measurements_data(time(NULL));
                /*std::string msg =*/ get_boiler_measurements_data(time(NULL));
                boilerSupplyTemp[0] = '\0';
                boilerReturnTemp[0] = '\0';
            }
            boiler_timer_count = 0;
        }
        if (room_temp_timer_count >= ROOM_TEMP_MEASUREMENT_INTERVAL_S) 
        {
            if ((roomTemp[0] == '\0' and  roomSetpoint[0] == '\0' and boilerMaxSupplyTemp[0] == '\0') && !forceGetMeasurements) 
            {
                ESP_LOGD(TAG, "No roomTemp no roomSetpoint and no boilerMaxSupplyTemp measured");
            } 
            else 
            {
                ESP_LOGD(TAG, "Measuring roomTemp, roomSetpoint and boilerMaxSupplyTemp");
                /*std::string msg =*/ get_5min_measurements_data(0/*time(NULL)*/);
                roomTemp[0] = '\0'; 
                roomSetpoint[0] = '\0';
                boilerMaxSupplyTemp[0] = '\0';
            }
            room_temp_timer_count = 0;
        }
        if (regular_timer_count >= REGULAR_MEASUREMENT_INTERVAL) 
        {
            if ((ch_mode == -1 and dhw_mode == -1 and flame == -1 and maxRelModLvl == -1 and minModLvl == -1 and relModLvl == -1 and maxBoilerCap == -1) && !forceGetMeasurements) 
            {
                ESP_LOGD(TAG, "No resular measurements");
            } 
            else 
            {
                ESP_LOGD(TAG, "Measuring regular measurements");
                /*std::string msg =*/ get_regular_measurements_data(0/*time(NULL)*/);
                ESP_LOGD(TAG, "data and download finshed");
                ch_mode = -1;
                dhw_mode = -1;
                flame = -1;
                maxRelModLvl = -1;
                minModLvl = -1;
                relModLvl = -1;
                maxBoilerCap = -1;
            }
            regular_timer_count = 0;
        }
        //vTaskDelay(10 / portTICK_PERIOD_MS);
        vTaskDelay(300000 / portTICK_PERIOD_MS); //Delaying for 10 minutes
        fiveMinuteCounter++;
        ESP_LOGD(TAG, "amount of times five minutes has past in the OpenTherm loop: %d", fiveMinuteCounter);
    }
}