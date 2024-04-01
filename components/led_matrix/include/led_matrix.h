#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"
#include "led_strip_encoder.h"

#define RMT_LED_STRIP_RESOLUTION_HZ 10000000 // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
#define RMT_LED_STRIP_GPIO_NUM      0
#define TOUCH_RIGHT_GPIO_NUM 		0

#define EXAMPLE_LED_NUMBERS         256
#define EXAMPLE_CHASE_SPEED_MS      1000

typedef struct {
	uint8_t *pixels;
	size_t num_pixels;
	rmt_channel_handle_t led_chan;
	rmt_encoder_handle_t led_encoder;
	rmt_transmit_config_t tx_config;

} led_strip_control_t;

void init_led_matrix(led_strip_control_t *control);
void transmit_pixels(led_strip_control_t *control);
void set_pixel_number(uint8_t position,
		uint8_t new_number);
