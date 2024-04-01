#include "led_matrix.h"

static uint8_t led_strip_pixels[EXAMPLE_LED_NUMBERS * 3];
static const char *TAG = "LED_MATRIX";

uint8_t pattern_zero[5][3] = { { 1, 1, 1 }, { 1, 0, 1 }, { 1, 0, 1 },
		{ 1, 0, 1 }, { 1, 1, 1 } };
uint8_t pattern_one[5][3] = { { 0, 1, 0 }, { 1, 1, 0 }, { 0, 1, 0 },
		{ 0, 1, 0 }, { 1, 1, 1 } };
uint8_t pattern_two[5][3] = { { 1, 1, 1 }, { 0, 0, 1 }, { 1, 1, 1 },
		{ 1, 0, 0 }, { 1, 1, 1 } };
uint8_t pattern_three[5][3] = { { 1, 1, 1 }, { 0, 0, 1 }, { 1, 1, 1 },
		{ 0, 0, 1 }, { 1, 1, 1 } };
uint8_t pattern_four[5][3] = { { 1, 0, 1 }, { 1, 0, 1 }, { 1, 1, 1 },
		{ 0, 0, 1 }, { 0, 0, 1 } };
uint8_t pattern_five[5][3] = { { 1, 1, 1 }, { 1, 0, 0 }, { 1, 1, 1 },
		{ 0, 0, 1 }, { 1, 1, 1 } };
uint8_t pattern_six[5][3] = { { 1, 1, 1 }, { 1, 0, 0 }, { 1, 1, 1 },
		{ 1, 0, 1 }, { 1, 1, 1 } };
uint8_t pattern_seven[5][3] = { { 1, 1, 1 }, { 0, 0, 1 }, { 1, 1, 1 },
		{ 0, 0, 1 }, { 0, 0, 1 } };
uint8_t pattern_eight[5][3] = { { 1, 1, 1 }, { 1, 0, 1 }, { 1, 1, 1 },
		{ 1, 0, 1 }, { 1, 1, 1 } };
uint8_t pattern_nine[5][3] = { { 1, 1, 1 }, { 1, 0, 1 }, { 1, 1, 1 },
		{ 0, 0, 1 }, { 1, 1, 1 } };

const uint8_t (*patterns[10])[3] = { pattern_zero, pattern_one, pattern_two,
		pattern_three, pattern_four, pattern_five, pattern_six, pattern_seven,
		pattern_eight, pattern_nine };

uint8_t start_end_fourth[3][2] = { { 86, 0 }, { 73, 1 }, { 70, 0 } };
uint8_t start_end_third[3][2] = { { 118, 0 }, { 105, 1 }, { 102, 0 } };
uint8_t start_end_second[3][2] = { { 166, 0 }, { 153, 1 }, { 150, 0 } };
uint8_t start_end_first[3][2] = { { 198, 0 }, { 185, 1 }, { 182, 0 } };

const uint8_t (*numbers[4])[2] = { start_end_first, start_end_second,
		start_end_third, start_end_fourth };

void set_pixel(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b) {
	led_strip_pixels[pixel * 3 + 0] = g;
	led_strip_pixels[pixel * 3 + 1] = r;
	led_strip_pixels[pixel * 3 + 2] = b;
}

void transmit_pixels(led_strip_control_t *control) {
	ESP_ERROR_CHECK(
			rmt_transmit(control->led_chan, control->led_encoder,
					led_strip_pixels, sizeof(led_strip_pixels),
					&(control->tx_config)));
	ESP_ERROR_CHECK(rmt_tx_wait_all_done(control->led_chan, portMAX_DELAY));
}

void set_pixel_number(uint8_t position,
		uint8_t new_number) {

	for (int i = 0; i < 3; i++) {
		if (numbers[position][i][1] == 0) {
			int x = 0;
			for (int j = numbers[position][i][0];
					j > numbers[position][i][0] - 5; j--) {
				if (patterns[new_number][x][i] == 1) {
					set_pixel(j, 100, 100, 100);
				} else {
					set_pixel(j, 0, 0, 0);
				}
				x++;
			}
		} else {
			int x = 0;
			for (int j = numbers[position][i][0];
					j < numbers[position][i][0] + 5; j++) {
				if (patterns[new_number][x][i] == 1) {
					set_pixel(j, 100, 100, 100);
				} else {
					set_pixel(j, 0, 0, 0);
				}
				x++;
			}
		}
	}

	set_pixel(133, 100, 100, 100);
	set_pixel(131, 100, 100, 100);
}

void init_pixel_numbers(led_strip_control_t *control) {
	for (uint8_t i = 0; i < 4; i++) {
		set_pixel_number(i, 0);
	}
	transmit_pixels(control);
}

void init_led_matrix(led_strip_control_t *control){

		ESP_LOGI(TAG, "Create RMT TX channel");
		rmt_channel_handle_t led_chan = NULL;
		rmt_tx_channel_config_t tx_chan_config =
				{
						.clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
						.gpio_num = RMT_LED_STRIP_GPIO_NUM,
						.mem_block_symbols = 64, // increase the block size can make the LED less flickering
						.resolution_hz = RMT_LED_STRIP_RESOLUTION_HZ,
						.trans_queue_depth = 4, // set the number of transactions that can be pending in the background
				};
		ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &led_chan));

		ESP_LOGI(TAG, "Install led strip encoder");
		rmt_encoder_handle_t led_encoder = NULL;
		led_strip_encoder_config_t encoder_config = { .resolution =
		RMT_LED_STRIP_RESOLUTION_HZ, };
		ESP_ERROR_CHECK(rmt_new_led_strip_encoder(&encoder_config, &led_encoder));

		ESP_LOGI(TAG, "Enable RMT TX channel");
		ESP_ERROR_CHECK(rmt_enable(led_chan));

		ESP_LOGI(TAG, "Start LED rainbow chase");
		rmt_transmit_config_t tx_config = { .loop_count = 0, // no transfer loop
				};

//		control = { .pixels = led_strip_pixels, .num_pixels =
//		EXAMPLE_LED_NUMBERS, .led_chan = led_chan, .led_encoder = led_encoder,
//				.tx_config = tx_config };
		control->pixels = led_strip_pixels;
		control->num_pixels = EXAMPLE_LED_NUMBERS;
		control->led_chan = led_chan;
		control->led_encoder = led_encoder;
		control->tx_config = tx_config;

//		init_pixel_numbers(&control);
}
