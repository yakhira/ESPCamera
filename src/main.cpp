#include "setup/wifi.h"
#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

extern int gpLed = 4; // Light
extern String WiFiAddr ="";

unsigned long last_time = 0;
unsigned long http_errors_count = 0;

// -------- DEFAULT SKETCH PARAMETERS --------
const int SKETCH_VERSION = 1; 

ESPWiFi espwifi("ESP32-D0WDQ5");

void startCameraServer();

void checkSleepState(unsigned int interval){
	if (last_time == 0) {
		last_time = millis()/1000;
	}

	if ((last_time + interval) <= (millis()/1000)) {
		JSONVar sleepState;
		if (!espwifi.getHTTPJsonData(
			espwifi.dataUrl + "/esp/sleep?mac=" + WiFi.macAddress(),
			sleepState
		)) {
			http_errors_count +=1;
		} else {
			http_errors_count = 0;
		}

		if ((bool)sleepState["state"] || http_errors_count > 10){
			digitalWrite(gpLed, LOW);
			gpio_hold_en((gpio_num_t) gpLed);

			gpio_deep_sleep_hold_en();

			esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
			esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_ON);
			esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);

			esp_sleep_enable_timer_wakeup((int)sleepState["interval"]*1000000);	
			esp_deep_sleep_start();
		}
		last_time = millis()/1000;
	}
}

void main_code()
{
	camera_config_t config;
	config.ledc_channel = LEDC_CHANNEL_0;
	config.ledc_timer = LEDC_TIMER_0;
	config.pin_d0 = Y2_GPIO_NUM;
	config.pin_d1 = Y3_GPIO_NUM;
	config.pin_d2 = Y4_GPIO_NUM;
	config.pin_d3 = Y5_GPIO_NUM;
	config.pin_d4 = Y6_GPIO_NUM;
	config.pin_d5 = Y7_GPIO_NUM;
	config.pin_d6 = Y8_GPIO_NUM;
	config.pin_d7 = Y9_GPIO_NUM;
	config.pin_xclk = XCLK_GPIO_NUM;
	config.pin_pclk = PCLK_GPIO_NUM;
	config.pin_vsync = VSYNC_GPIO_NUM;
	config.pin_href = HREF_GPIO_NUM;
	config.pin_sscb_sda = SIOD_GPIO_NUM;
	config.pin_sscb_scl = SIOC_GPIO_NUM;
	config.pin_pwdn = PWDN_GPIO_NUM;
	config.pin_reset = RESET_GPIO_NUM;
	config.xclk_freq_hz = 20000000;
	config.pixel_format = PIXFORMAT_JPEG;
	
	config.frame_size = FRAMESIZE_VGA;
	config.jpeg_quality = 10;
	config.fb_count = 1;

	esp_err_t err = esp_camera_init(&config);
	if (err != ESP_OK){
		Serial.println("Couldn't init camera" + err);
		return;
	}

 	startCameraServer();
	WiFiAddr = WiFi.localIP().toString();
	Serial.println("URL: http://" + WiFiAddr);
}

void setup()
{
	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

	Serial.begin(115200);

	pinMode(gpLed, OUTPUT);
	digitalWrite(gpLed, LOW);
	gpio_hold_dis((gpio_num_t)gpLed);

	espwifi.wifiConnect();

	WiFi.setSleep(WIFI_PS_NONE);

	if (WiFi.getMode() == WIFI_STA) {
		checkSleepState(0);
		espwifi.updateSketch(SKETCH_VERSION);

		main_code();
	}
}

void loop() {
	if (WiFi.getMode() == WIFI_STA) {
		if (WiFi.status() != WL_CONNECTED) {
			WiFi.reconnect();
		}
		checkSleepState(30);
	}

	espwifi.stateCheck();
}