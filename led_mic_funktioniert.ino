#include <driver/i2s.h>
#define I2S_WS 15
#define I2S_SD 32
#define I2S_SCK 14
#define I2S_PORT I2S_NUM_0
#define bufferLen 128

#include <Adafruit_NeoPixel.h>  // https://github.com/adafruit/Adafruit_NeoPixel

#define STRIP_LEDS 90
#define STRIP_OUT 16

#define MAX_INPUT 400

Adafruit_NeoPixel strip(STRIP_LEDS, STRIP_OUT, NEO_GRB + NEO_KHZ800);

uint8_t reds[STRIP_LEDS] = {0, 20, 25, 30, 35, 40, 50, 60, 70, 70, 80, 80, 90, 110, 130, 150, 170, 190, 200, 220, 240, 250, 255, 255, 255, 255 };

int16_t sBuffer[bufferLen];
void setup() {
  Serial.begin(115200);
  Serial.println("Setup I2S ...");
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);
  delay(500);

  strip.begin();
  strip.setBrightness(50);
  for (int i = 0; i < STRIP_LEDS; i++) {
    strip.setPixelColor(i, 0xFFFFFF);
    strip.show();
    delay(20);
  }
  delay(100);
}

void loop() {

  size_t bytesIn = 0;
  esp_err_t result = i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);
  if (result == ESP_OK) {
    int samples_read = bytesIn / 8;
    if (samples_read > 0) {
      float mean = 0;
      for (int i = 0; i < samples_read; ++i) {
        mean += (sBuffer[i]);
      }
      mean /= samples_read;
      int maxLed = map((long)abs(mean), 10, MAX_INPUT, 0, STRIP_LEDS);
      //  Serial.println(maxLed);
      for (int i = 0; i < maxLed; i++) {
        uint8_t red = constrain(15*i, 0, 255);
        uint8_t green = constrain(255 - 12*i, 0, 255);
        uint32_t color = (red << 16) | (green << 8);
        strip.setPixelColor(i, color);
        strip.show();
      }
    }
  }

  strip.fill(0x000000, 0, STRIP_LEDS);
  strip.show();
}

void i2s_install() {
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 5120,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = 0,  // default interrupt priority
    .dma_buf_count = 8,
    .dma_buf_len = bufferLen,
    .use_apll = false
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin() {
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}
