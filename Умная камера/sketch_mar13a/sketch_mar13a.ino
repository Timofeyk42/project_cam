#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_camera.h"

// Настройки Wi-Fi
const char* ssid = "*******";
const char* password = "******";
const char* URL = "************/upload";

// Настройки GPIO для ESP32-CAM AI Thinker с камерой OV2640
#define PWDN_GPIO_NUM    32
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM     0
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27

#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      21
#define Y4_GPIO_NUM      19
#define Y3_GPIO_NUM      18
#define Y2_GPIO_NUM       5

#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

void setup() {
    Serial.begin(115200);

    // Инициализация камеры
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

    pinMode(12, INPUT_PULLDOWN); // Настройка пина 12 как входа с подтяжкой к земле

    // Настройка размера кадра и качества JPEG
    if (psramFound()) {
        Serial.println("PSRAM detected. Using larger frame size.");
        config.frame_size = FRAMESIZE_UXGA; // UXGA (1600x1200)
        config.jpeg_quality = 5;          // Качество JPEG
        config.fb_count = 2;
    } else {
        Serial.println("PSRAM not found. Using smaller frame size.");
        config.frame_size = FRAMESIZE_SVGA; // SVGA (800x600)
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    // Инициализация камеры
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        if (err == ESP_ERR_NOT_FOUND) {
            Serial.println("Camera not detected. Check hardware connections and pin configuration.");
        } else {
            Serial.println("Unknown error during camera initialization.");
        }
        return;
    }
    Serial.println("Camera initialized successfully");

    // Подключение к Wi-Fi
    WiFi.begin(ssid, password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Failed to connect to Wi-Fi");
        return;
    }
    Serial.println("Connected to WiFi");
}

void send() {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        return;
    }
    Serial.println("Image captured successfully");

    HTTPClient http;
    http.begin(URL); // URL сервера
    http.addHeader("Content-Type", "image/jpeg");

    int httpResponseCode = http.POST((uint8_t*)fb->buf, fb->len); // Отправляем массив байтов
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.printf("HTTP Response code: %d\n", httpResponseCode);
        Serial.println(response);
    } else {
        Serial.printf("Error in HTTP request: %d\n", httpResponseCode);
    }

    http.end();
    esp_camera_fb_return(fb); // Освобождаем буфер изображения
}

void loop() {
    int pinState = digitalRead(12); // Считываем состояние пина 12
    if (pinState == HIGH) {         // Проверяем, есть ли напряжение на пине
        send();                     // Отправляем изображение
        delay(1000);                // Добавляем задержку, чтобы избежать множественных отправок
    }
}