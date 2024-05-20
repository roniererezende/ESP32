/*
 * ESP32 COURSE: REALLY LEARN
 * WR Kits: https://wrkits.com.br | https://space.hotmart.com/wrkits
 * 
 * Smart Timer with binary display
 * 
 * Actives a buzzer by a time and keeps a relay activated in counting final
 * User adjust value of 1 to 1 or 10 to 10
 * 
 * ESP-WROOM-32
 * Board: DevKitV1
 * Compiler: ESP-IDF
 * 
 * Author: Roniere Rezende
 * Date: May, 2024
 *
 */

// =================================================================================================
// --- Auxiliar Libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOSConfig.h"
#include "esp_task_wdt.h"
//#include "freeRTOS.h"

// =================================================================================================
// --- Hardware mapping ---
#define     LED_6       27          // Led 6 of display
#define     LED_5       26          // Led 5 of display
#define     LED_4       18          // Led 4 of display
#define     LED_3       19          // Led 3 of display
#define     LED_2       21          // Led 2 of display
#define     LED_1       22          // Led 1 of display
#define     LED_0       23          // Led 0 of display
#define     BT_INC      15          // Button for adjust 1 to 1
#define     BT_INC10     4          // Button for adjust 10 to 10
#define     BT_START     5          // Buttton to start timing 
#define     RELAY       33          // Relay activated after the end of time
#define     BUZZER      25          // Buzzer for sound indication
#define     LED_OB       2          // LED onboard

// =================================================================================================
// --- Functions Prototype ---
void read_bts();                    // Button reading
void ios_init();                    // Initialize IOs
void set_display(uint8_t en);       // Binary display control
void delay_ms(const uint32_t ms);   // delay in milliseconds

// =================================================================================================
// --- Local Global ---
int     timer_val     = 0;            // Binary counting value
bool    start_flag    = false,        // Flag to indicate
        st_ledob      = false,
        Perform_count = false;

const esp_task_wdt_config_t esp_task_wdt = { 
    .timeout_ms = 10000,
    .idle_core_mask = 1,
    .trigger_panic = false
};

// =================================================================================================
// --- Task Declaration ---
void Smart_Timer_Task(void *pvParameter);
void Blink_Led_Task(void *pvParameter);

// =================================================================================================
// --- Main Functions ---
void app_main(void)
{
    //esp_task_wdt_init(&esp_task_wdt);
    xTaskCreate(Smart_Timer_Task,
                "Smart_Timer_Task",
                10000,
                NULL,
                5,
                NULL);

    xTaskCreate(Blink_Led_Task,
                "Blink_LED_Task",
                10000,
                NULL,
                5,
                NULL);
    
    ios_init();                     // Initialize IOs
    set_display(0x00);              // display starts turned off
}


// =================================================================================================
// --- Smart Timer Task ---
void Smart_Timer_Task(void *pvParameter)
{
    while(true)                     // Infinity loop
    {      
        read_bts();                 // Reads keyboard
        set_display(timer_val);     // Updates display

        if(start_flag)              // Starts set flag
        {
            timer_val--;            // Decrements timer_val

            if((timer_val < 0) && (Perform_count == true))             // timer_val lower than zero?
            {                             // yes
               printf("COUNTNG has finished!!!\n"); 
               timer_val = 0;            // reset timer_val in zero

                gpio_set_level(RELAY, true);    // Turns relay on
                gpio_set_level(BUZZER, true);   // Turns buzzer on
                delay_ms(555);                  // Waits
                gpio_set_level(BUZZER, false);  // Turns buzzer off
                start_flag = false;                 // Cleans start_flag
                Perform_count = false;
            }
            if(Perform_count == true)
            {
                delay_ms(1000);
            }
        } // ende if !timer_val

        if(Perform_count == false)
        {
            delay_ms(10);               // each second
        }
    }     // end if start_flag
}

// --- Blink Led Task ---
void Blink_Led_Task(void *pvParameter)
{
    while(true)
    {
        st_ledob = !st_ledob;   // Toggle status of onboard LED
        gpio_set_level(LED_OB, st_ledob); // Toggle LED_onboard
        delay_ms(100);               // each second
    }
        
}
// =================================================================================================
// --- Functions Development ---

// =================================================================================================
// --- read_bts ---
// -> Buttons readins(hold and click logic)
void read_bts()
{
    static bool flag1 = false, flag2 = false, flag3 = false; // Local variables for button status

    if(!gpio_get_level(BT_INC))     flag1 = true;            // If increment button pressed, set flag1
    if(!gpio_get_level(BT_INC10))   flag2 = true;            // If increment of 10 button pressed, set flag2
    if(!gpio_get_level(BT_START))   flag3 = true;            // If start button pressed, set flag1

    if(gpio_get_level(BT_INC) && flag1)                      // inc button releases and flag1 set?
    {                                                        // Yes
        printf("INC BUTTON pressed\n");                                       // Delay anti-boucing
        
        flag1 = false;                                       // cleans flag1
        timer_val += 1;                                      // increments timer_val   
        if(timer_val > 127) timer_val = 0;                   // If higher than 127, it goes to zero
        //delay_ms(150);
    }   // End if BT_INC

    if(gpio_get_level(BT_INC10) && flag2)                // inc 10 button releases and flag2 set?
    {
        printf("INC 10 BUTTON pressed\n");   
                                                                // Yes
        flag2 = false;                                       // cleans flag2
        timer_val += 10;                                     // increments timer_val 
        if(timer_val > 127) timer_val = 0;                   // If higher than 127, it goes to zero
        //delay_ms(150);                                       // Delay anti-boucing
    }   // End if BT_INC

    if(gpio_get_level(BT_START) && flag3)
    {
        printf("START BUTTON pressed\n");   
        flag3 = false;                          // Clear flag3
        start_flag = true;                      // set start_flag
        Perform_count = true;
        if(timer_val > 127) timer_val = 0;      // turn relay off
        //delay_ms(150);                          // delay anti-bouncing
    }   // End if BT_INC10
}       // End read_bts

// =================================================================================================
// --- ios_init ---
// -> Initializes inputs and outputs used in timer
void ios_init()
{
    gpio_set_direction(LED_6, GPIO_MODE_OUTPUT);  // Output for LED_6
    gpio_set_direction(LED_5, GPIO_MODE_OUTPUT);  // Output for LED_5
    gpio_set_direction(LED_4, GPIO_MODE_OUTPUT);  // Output for LED_4
    gpio_set_direction(LED_3, GPIO_MODE_OUTPUT);  // Output for LED_3
    gpio_set_direction(LED_2, GPIO_MODE_OUTPUT);  // Output for LED_2
    gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);  // Output for LED_1
    gpio_set_direction(LED_0, GPIO_MODE_OUTPUT);  // Output for LED_0
    gpio_set_direction(RELAY, GPIO_MODE_OUTPUT);  // Output for relay
    gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT); // Output for buzzer
    gpio_set_direction(LED_OB, GPIO_MODE_OUTPUT); // Output for onboard led

    gpio_set_level(LED_6,  false);                // LED_6 starts off
    gpio_set_level(LED_5,  false);                // LED_5 starts off
    gpio_set_level(LED_4,  false);                // LED_4 starts off
    gpio_set_level(LED_3,  false);                // LED_3 starts off
    gpio_set_level(LED_2,  false);                // LED_2 starts off
    gpio_set_level(LED_1,  false);                // LED_1 starts off
    gpio_set_level(LED_1,  false);                // LED_0 starts off
    gpio_set_level(RELAY,  false);                // RELAY starts off
    gpio_set_level(BUZZER, false);                // BUZZER starts off
    gpio_set_level(LED_OB, false);                // LED_onboard starts off

    gpio_set_direction(BT_INC,   GPIO_MODE_INPUT);// INPUT for 
    gpio_set_direction(BT_INC10, GPIO_MODE_INPUT);// INPUT for 
    gpio_set_direction(BT_START, GPIO_MODE_INPUT);// INPUT for 
} // end ios_init

// =================================================================================================
// --- set_display ---
// -> Initializes inputs and outputs used in timer
void set_display(uint8_t en)
{
    gpio_set_level(LED_6, (en>>6)&0x01);    // LED_6 state,
    gpio_set_level(LED_5, (en>>5)&0x01);    // LED_5 state,
    gpio_set_level(LED_4, (en>>4)&0x01);    // LED_4 state,
    gpio_set_level(LED_3, (en>>3)&0x01);    // LED_3 state,
    gpio_set_level(LED_2, (en>>2)&0x01);    // LED_2 state,
    gpio_set_level(LED_1, (en>>1)&0x01);    // LED_1 state,
    gpio_set_level(LED_0, (en>>0)&0x01);    // LED_0 state,
} // end set_display

// =================================================================================================
// --- delay_ms ---
// -> Intuitive function for delay in millisseconds
void delay_ms(const uint32_t ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}   // end delay_ms