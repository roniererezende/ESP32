// 1 - Libraries includes and general definitions
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// FreeRTOS Libraries
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include <freertos/event_groups.h>

// General Espressif libraries
#include <esp_system.h>
#include <esp_spi_flash.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <nvs_flash.h>
#include <sdkconfig.h>

// Network libraries
#include <lwip/sockets.h>
#include <lwip/dns.h>
#include <lwip/netdb.h>

// MQTT ESP-IDF Libraries
#include <mqtt_client.h>

// JSON Libraries
#include <cJSON.h>

// Drivers (GPIO, PWM, LED, ADC)
#include <driver/spi_common.h>

// Soon -> DHT22
#include <dht.h>
#include <pcd8544.h>

/*************************************************/
// 2 - Defines and  Support Variables
static const gpio_num_t DHT_gpio = GPIO_NUM_33;
static const dht_sensor_type_t sensor_type = DHT_TYPE_AM2301;

// Support Element Initialization to application 
static const char *TAG_MQTT = "ESP-MGTT";
static const char *TAG_WIFI = "ESP-WIFI";
static const char *TAG_DHT  = "ESP_DHT";
static const char *TAG_SYS  = "ESP-SYS";

static const char *topic_mqtt_cmd = "unisal/iot/cmd";
static const char *topic_mqtt_data = "unisal/iot/data";

char mqtt_buffer[128]; // Used to manipulate data for transmission to MQTT broker

// References to know connection status
static EventGroupHandle_t wifi_event_group;
const static int CONNECTION_STATUS = BIT0;

// MQTT Client
esp_mqtt_client_handle_t mqtt_client; 

/*************************************************/
// 3 - System Event Treatment

/**
 * @brief Wifi connection event treatment
 * 
 * @param ctx Event context Data
 * @param event Event information
 * @return esp_err_t ESP_Ok - Successful return
 */
static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id)
    {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect(); // Starts wifi connection 
        break;

        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(wifi_event_group, CONNECTION_STATUS); // Sets connection status
        break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            esp_wifi_connect();
            xEventGroupClearBits(wifi_event_group, CONNECTION_STATUS); // Clear connection status 
        break;

        default: 
        break;
    }

    return ESP_OK; // anyway ok return
}

/**
 * @brief Wi-Fi Initialization Routine in our ESP32
 * 
 */
static void wifi_init(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid     = "Malware",
            .password = "SkyGato*474"
        }
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

    ESP_LOGI(TAG_WIFI, "Initializing connection with WI-FI connection...");
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG_WIFI, "Connecting....");
    xEventGroupWaitBits(wifi_event_group, CONNECTION_STATUS, false, true, portMAX_DELAY);
}

/**
 * @brief Routine of "callback" type for MQTT event treatment
 * 
 * @param event MQTT event reported by MQTT client
 * @return esp_err_t Callback treatment execution error
 */
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    // Recovery a local client reference
    // useful to responde message and to treat other events
    esp_mqtt_client_handle_t client = event->client;

    int msg_id;

    switch(event->event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG_MQTT, "Connection performed with MQTT broker");
            msg_id = esp_mqtt_client_subscribe(client, topic_mqtt_cmd, 0);
        break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG_MQTT, "Disconnection performed with MQTT broker");
        break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG_MQTT, "Subscribe performed with MQTT broker");
        break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG_MQTT, "Publish performed with MQTT Broker");
        break;

        // When it arrives data in 1 or more topic where 
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG_MQTT, "Data received via MQTT Broker");

            printf("TOPICO: %.*s\r\n", event->topic_len, event->topic);
            printf("Data: %.*s\r\n", event->data_len, event->data);

        break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG_MQTT, "MQTT_EVENT_ERROR");
        break;

        default:
        break;
    }

    return ESP_OK;
}

/**
 * @brief MQTT Initialization Routine
 * 
 */
static void mqtt_init(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://broker.hivemq.com:1883",
        .event_handle = mqtt_event_handler,
        .client_id = "ESP-IoT-Roniere_Rezende"  
    };

    // Initializes MQTT client
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(mqtt_client);
}

/***************************************************************/
// 4 - RTOS treats
void DHT_task(void *pvParameter)
{
    // Collects DHT11/22 sensor data
}

void LEDStatus_task(void *pvParameter)
{
    // Connecting/ disconnecting : led blinking 4 times/s 
    // Connected : led blinking 1 times/s
    // Problem   :
}

void RelayControl_task(void *pvParameter)
{
    // 2 GPIO control (supposedly connected to relay / LED for example)
    // Queues 
}

void DisplayControl_task(void *pvParameter)
{
    // Controls Nokia 5110 display
    // Queues
}

void MqttSendMsg_task(void *pvParameter)
{
    // Sends data from equipment to cloud
    // Queues
}

// 5 - Main application 

void app_main(void)
{
    ESP_LOGI(TAG_SYS, "Initializinf ESP32 IoT App");

    // Setup Application log modes
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE); // MQTT Library Log

    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        // If we are with memory problem to the start, clears and initializes the non-volatile memory
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    wifi_init();
    mqtt_init();

    float temperature = 0.0;
    float humidity    = 0.0;

    pcd8544_config_t config_pcd8544 = {
        .spi_host = HSPI_HOST,
        .is_backlight_common_anode = true,
    };

    pcd8544_init(&config_pcd8544); // Display initialization
    pcd8544_clear_display();       // Clears data on screen
    pcd8544_finalize_frame_buf();  // Clear memory buffer
    pcd8544_set_contrast(24);     // Sets contrast on screen
    pcd8544_set_pos(12, 0); // row = / col =
    pcd8544_puts("ESP32 IoT");     // Prints data on screen
    pcd8544_draw_line(1, 1 , 1, 10);

    while(true)
    {
        if(dht_read_float_data(sensor_type, DHT_gpio, &humidity, &temperature) == ESP_OK)
        {
            printf("Humidity: %.2f %% Temp: %.2f C\n", humidity, temperature);
        }
        else
        {
            printf("Could not read data from sensor\n");
        }

        pcd8544_set_pos(0, 2); // row = / col =
        pcd8544_printf("TMP: %2.1f%", temperature);
        pcd8544_set_pos(0, 3); // row = / col =
        pcd8544_printf("UMD: %2.1f%", humidity);
        pcd8544_sync_and_gc();

        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    // pcd8544_free();

    // Proximos passos (Atividade AVA IoT DHT11/22):
    // 1 - Criar e executar thread para controle de LED Hearbeat 1s / 1hz
    // 2 - Reservem um "LED" externo para ser conectado ao ESP32 - usaremos em instantes
    // 3 - Import e setup de Biblioteca pra DHT22 no IDF (feito)
    // 4 - Task pra leitura de DHT11/22 e enviar os dados usando JSON (cJSON)
    // para unisal/iot/data
    // {
    //   "device_id" : "nome ou id do seu esp32", 
    //   "temp"      : temperatura
    //   "umid"      : umidade
    //   "sample"    : nr da amostra
    //   "timestamp" : data e hora da coleta dos dados (desafio) (dica: lib de RTC (Real Time Clock) + NTP (Network Time Protocol))
    // }

}
