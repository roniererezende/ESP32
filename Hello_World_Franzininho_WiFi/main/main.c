#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    while(true)
    {
        printf("Hello World!\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
