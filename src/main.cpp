#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/adc.h"
#include "setup/wifi.h"
#include "camera/camera.h"

// -------- DEFAULT SKETCH PARAMETERS --------
const int SKETCH_VERSION = 2;

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

#define DEFAULT_SLEEP_TIME_MIN 5
#define GPIO_PIN_LED GPIO_NUM_13

ESPWiFi espwifi("ESP32-D0WDQ5");

void camera_init(){
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
	
	config.frame_size = FRAMESIZE_SVGA;
	config.jpeg_quality = 5;
	config.fb_count = 1;

	esp_err_t err = esp_camera_init(&config);

    if (err != ESP_OK)
	{
		Serial.println("Can't init camera.");
	}
}

void main_code()
{
	camera_init();

	AsyncWebServer *server = new AsyncWebServer(80);

	server->on("/stream", HTTP_GET, [] (AsyncWebServerRequest *request) {
        AsyncJpegStreamResponse *response = new AsyncJpegStreamResponse();
        if(!response){
            request->send(501);
            return;
        }
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

	server->on("/led", HTTP_GET, [](AsyncWebServerRequest *request){
        if (request->hasArg("on")){
            digitalWrite(GPIO_PIN_LED, HIGH);
        } else {
            digitalWrite(GPIO_PIN_LED, LOW);
        } 
        request->send(200, "text/html", "OK");
    });

	server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/html/index.html", "text/html");
    });

	server->begin();
	Serial.println("URL: http://" + WiFi.localIP().toString());
}

void sleep_state(){
	String result;
	int sleep_time = DEFAULT_SLEEP_TIME_MIN;

	if (espwifi.getHTTPData(espwifi.dataUrl + "/esp/camera", result)) {
		sleep_time = result.toInt();
	}

	if (sleep_time > 0){
		digitalWrite(GPIO_PIN_LED, LOW);

		gpio_deep_sleep_hold_en();
		gpio_hold_en(GPIO_PIN_LED);

		esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
		esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_ON);

		esp_sleep_enable_timer_wakeup(sleep_time*60*1000000);
		esp_deep_sleep_start();
	}
}

void setup(){
	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

	// gpio_hold_dis(GPIO_PIN_LED);
	pinMode(GPIO_PIN_LED, OUTPUT);

	Serial.begin(115200);

	espwifi.wifiConnect();

	WiFi.setSleep(WIFI_PS_NONE);

	if (WiFi.getMode() == WIFI_STA) {
		sleep_state();
		espwifi.updateSketch(SKETCH_VERSION);
		main_code();
	}
}

void loop(){
	if (WiFi.getMode() == WIFI_STA) {
		if (WiFi.status() != WL_CONNECTED) {
			WiFi.reconnect();
		}
		sleep_state();
		delay(5000);
	} 
}