#include <stdio.h>
#include <stdbool.h>
#include "esp_log.h"

static const char *TAG = "Program Logging";

void app_main(void)
{
    ESP_LOGI(TAG, "Starts program\n");

    printf("\n LOG Level for Default = INFO\n");
    ESP_LOGE(TAG, "Error...");       // It will be exhibit
    ESP_LOGW(TAG, "Warning...");     // It will be exhibit
    ESP_LOGI(TAG, "Information..."); // It will be exhibit
    ESP_LOGD(TAG, "Debub...");       // It won't be exhibit
    ESP_LOGV(TAG, "Verbose...");     // It won't be exhibit

    printf("\n LOG Level for Error\n");
    esp_log_level_set(TAG, ESP_LOG_ERROR);

    ESP_LOGE(TAG, "Error...");       // It will be exhibit
    ESP_LOGW(TAG, "Warning...");     // It won't be exhibit
    ESP_LOGI(TAG, "Information..."); // It won'l be exhibit
    ESP_LOGD(TAG, "Debub...");       // It won't be exhibit
    ESP_LOGV(TAG, "Verbose...");     // It won't be exhibit

    printf("\n LOG Level for Warning\n");
    esp_log_level_set(TAG, ESP_LOG_WARN);
    
    ESP_LOGE(TAG, "Error...");       // It will be exhibit
    ESP_LOGW(TAG, "Warning...");     // It will be exhibit
    ESP_LOGI(TAG, "Information..."); // It won't be exhibit
    ESP_LOGD(TAG, "Debub...");       // It won't be exhibit
    ESP_LOGV(TAG, "Verbose...");     // It won't be exhibit

    printf("\n LOG Level for Info\n");
    esp_log_level_set(TAG, ESP_LOG_INFO);

    ESP_LOGE(TAG, "Error...");       // It will be exhibit
    ESP_LOGW(TAG, "Warning...");     // It will be exhibit
    ESP_LOGI(TAG, "Information..."); // It will be exhibit
    ESP_LOGD(TAG, "Debub...");       // It won't be exhibit
    ESP_LOGV(TAG, "Verbose...");     // It won't be exhibit

    printf("\n LOG Level for Debug\n");
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
    
    ESP_LOGE(TAG, "Error...");       // It will be exhibit
    ESP_LOGW(TAG, "Warning...");     // It will be exhibit
    ESP_LOGI(TAG, "Information..."); // It will be exhibit
    ESP_LOGD(TAG, "Debub...");       // It will be exhibit
    ESP_LOGV(TAG, "Verbose...");     // It won't be exhibit

        printf("\n LOG Level for Verbose\n");
    esp_log_level_set(TAG, ESP_LOG_VERBOSE);
    
    ESP_LOGE(TAG, "Error...");       // It will be exhibit
    ESP_LOGW(TAG, "Warning...");     // It will be exhibit
    ESP_LOGI(TAG, "Information..."); // It will be exhibit
    ESP_LOGD(TAG, "Debub...");       // It will be exhibit
    ESP_LOGV(TAG, "Verbose...");     // It will be exhibit

    printf("\n LOG Level for None\n");
    esp_log_level_set(TAG, ESP_LOG_NONE);
    
    ESP_LOGE(TAG, "Error...");       // It won't be exhibit
    ESP_LOGW(TAG, "Warning...");     // It won't be exhibit
    ESP_LOGI(TAG, "Information..."); // It won't be exhibit
    ESP_LOGD(TAG, "Debub...");       // It won't be exhibit
    ESP_LOGV(TAG, "Verbose...");     // It won't be exhibit
/*    while(true)
    {

    }
*/
}
