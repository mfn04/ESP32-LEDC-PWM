
#include "driver/ledc.h"
#include "esp_task.h"
#include "esp_log.h"
#include "math.h"
#include "driver/spi_master.h"

#define RESOLUTION LEDC_TIMER_16_BIT
#define CHANNEL LEDC_CHANNEL_0
#define SPEED_MODE LEDC_HIGH_SPEED_MODE
#define PIN GPIO_NUM_26
#define MAX_DUTY pow(2,RESOLUTION)

uint32_t get_duty(float percent_in_dec) {
    return percent_in_dec*MAX_DUTY;
}

void app_main() {

    bool backward = false;
    float curr_duty = 0.05;

    ledc_timer_config_t timer_conf = {
        .freq_hz = 120,
        .duty_resolution = RESOLUTION,
        .clk_cfg = LEDC_APB_CLK,
        .speed_mode = SPEED_MODE,
        .timer_num = LEDC_TIMER_1
    };

    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {
        .channel = CHANNEL,
        .gpio_num = PIN,
        .duty = get_duty(0.1),
        .speed_mode = SPEED_MODE,
        .timer_sel = LEDC_TIMER_1,
        .flags.output_invert = false 
    };

    ledc_channel_config(&channel_conf);

    ledc_fade_func_install(0);

    bool manual = false;

    while(1){
        if(manual){
            if(!backward){
                if(curr_duty < 1){
                    curr_duty += 0.005;
                } else {
                    curr_duty = 1;
                    backward = true;
                }
            } else {
                if(curr_duty > 0){
                    curr_duty -= 0.005;
                } else {
                    curr_duty = 0;
                    backward = false;
                }
            }
            uint32_t d = get_duty(curr_duty);
            ledc_set_duty_and_update(SPEED_MODE, CHANNEL, d, 0);
            ESP_LOGI("INFO", "CURRENT DUTY: %ld", d);
            vTaskDelay(pdMS_TO_TICKS(5));
        } else {
            ESP_LOGI("INFO","RUNNING FADE!");
            ledc_set_fade_time_and_start(SPEED_MODE,CHANNEL,MAX_DUTY,2000,LEDC_FADE_WAIT_DONE);
            //vTaskDelay(pdMS_TO_TICKS(2000));
            ledc_set_fade_time_and_start(SPEED_MODE,CHANNEL,0,2000,LEDC_FADE_WAIT_DONE);
            ESP_LOGI("INFO","FINISHED FADE!");
            //vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }

}