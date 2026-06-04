// #include "nvs_flash.h"
// #include "esp_wifi.h"
// #include "esp_event.h"
// #include "esp_log.h"

// static const char *TAG = "WIFI_TEST";

// // 事件處理器：處理連線成功、失敗與取得 IP
// void wifi_event_handler(void* arg, esp_event_base_t base, int32_t id, void* data) {
//     if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
//         esp_wifi_connect();
//     } else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
//         ESP_LOGI(TAG, "正在嘗試重新連線...");
//         esp_wifi_connect();
//     } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
//         ip_event_got_ip_t* event = (ip_event_got_ip_t*) data;
//         ESP_LOGI(TAG, "成功取得 IP: " IPSTR, IP2STR(&event->ip_info.ip));
//     }
// }

// void app_main(void) {
//     // 1. 初始化 NVS (必備)
//     esp_err_t ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//         ESP_ERROR_CHECK(nvs_flash_erase());
//         ret = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(ret);

//     // 2. 網路介面初始化
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());
//     esp_netif_create_default_wifi_sta();

//     // 3. Wi-Fi 預設配置
//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//     // 4. 註冊事件監聽
//     ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
//     ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

//     // 5. 設定 SSID 與 密碼
//     wifi_config_t wifi_config = {
//         .sta = {
//             .ssid = "阿欣",
//             .password = "903864328",
//         },
//     };
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
//     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
//     ESP_ERROR_CHECK(esp_wifi_start());
// }

// #include <stdio.h>
// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/event_groups.h"
// #include "esp_system.h"
// #include "esp_wifi.h"
// #include "esp_event.h"
// #include "esp_log.h"
// #include "nvs_flash.h"
// #include "esp_netif.h"
// #include "esp_tls.h"
// #include "esp_http_client.h"
// #include "esp_crt_bundle.h"
// #include "esp_sntp.h"
// #include <time.h>

// static const char *google_root_ca =
// "-----BEGIN CERTIFICATE-----\n"
// "MIIDdzCCAl+gAwIBAgIEbmh5ejANBgkqhkiG9w0BAQsFADBvMQswCQYDVQQGEwJV\n"
// "UzEVMBMGA1UEChMMRGlnaUNlcnQsIEluYzETMBEGA1UECxMKd3d3LmRpZ2ljZXJ0\n"
// "LmNvbTE0MDIGA1UEAxMrRGlnaUNlcnQgR2xvYmFsIFJvb3QgRzIgU0hBMjU2IFRp\n"
// "bWVTdGFtcGluZyBDQTAeFw0yMjAzMjMwMDAwMDBaFw0zNzAzMjIyMzU5NTlaMG8x\n"
// "CzAJBgNVBAYTAlVTMRUwEwYDVQQKEwxEaWdpQ2VydCwgSW5jLjETMBEGA1UECxMK\n"
// "d3d3LmRpZ2ljZXJ0LmNvbTE0MDIGA1UEAxMrRGlnaUNlcnQgR2xvYmFsIFJvb3Qg\n"
// "RzIgU0hBMjU2IFRpbWVTdGFtcGluZyBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEP\n"
// "ADCCAQoCggEBANM6xVnX3L7hIwrKyYVJZZzKzbwQ6vurIWBLL8GMDIS9ZhDJW60F\n"
// "7uO3cu6UytzszbmWzxubUoil58x2oyS9MhUlCT3VkOITkkpFmS6r30YIOCQM7DDO\n"
// "PAHDLcGRIDcN+xr3aMcMBXNIN1qfN3Wf9eKJeNEqXWiwxupCSvJzpuG1HsfrN7kD\n"
// "fNwCuWHa3gwxM/H2ymlk/wEYBLETymFcpnSUsctNk6heAQ7EzxKQEiC5EvhczHxV\n"
// "RJWb1x1o1t4bm/FYnGV8eK3opgVetwYqKqRR3YKHyCuXapnwXCfJOLLmObEWj1vQ\n"
// "teA94ppIqhzyapMI2vlA38nSxrdbidKfnUSsfx8CAwEAAaNCMEAwDgYDVR0PAQH/\n"
// "BAQDAgGGMBIGA1UdEwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFPS24kTx5cDIeEJD\n"
// "7BqXc9E+u6KDMA0GCSqGSIb3DQEBCwUAA4IBAQBM8mlFinallyWl7zBLmks5nZUD+\n"
// "dbCMryGZGsS2fy4VB/xWbfX3HBOH4DWz2oxEd4pqco3EQAOEjun9wioMxSsRKIYY\n"
// "iwSeNBY0d5HTdcNZa2m30IZHuZOKhHvJ3C0cFLbFhF38sZ4N2VNivg3XcX4Jt9dn\n"
// "H5PHeTtQKgHdwNwp0UrEGouGZWlznImPi0tLxe3LjVJN8dkUBaRdOy+nK8BPVKxH\n"
// "UefxWWEXUOYaL6VLdystD5nq2WEYLRh3SeDsICoZ6irMIXja+6JGZveHFkNjEcNW\n"
// "2tQeM+c/2yZfyPU/BK+f5pFwchAuC8W9jAq56k97X3CJidFG8sRP/6IDdnh3\n"
// "-----END CERTIFICATE-----\n";

// /* ─── 使用者設定區 ─────────────────────────────── */
// #define WIFI_SSID        "阿欣"
// #define WIFI_PASSWORD    "903864328"

// // Firebase Realtime Database
// // URL 格式: https://<project-id>-default-rtdb.firebaseio.com
// #define FIREBASE_HOST    "https://esp-to-web-72def-default-rtdb.asia-southeast1.firebasedatabase.app"
// #define FIREBASE_SECRET  "FEUMMSOMzF3ERd3ko8kKrUjeDfeXWgKaFJ0LYzgH"   // 舊版 legacy secret
// #define FIREBASE_PATH    "/sensors/esp32.json"     // 資料存放路徑

// // 上傳間隔 (毫秒)
// #define UPLOAD_INTERVAL_MS  5000
// /* ────────────────────────────────────────────── */

// static const char *TAG = "firebase";

// // Wi-Fi 連線事件群組
// static EventGroupHandle_t s_wifi_event_group;
// #define WIFI_CONNECTED_BIT  BIT0
// #define WIFI_FAIL_BIT       BIT1

// static int s_retry_count = 0;
// #define WIFI_MAX_RETRY  5

// /* ── 模擬感測器讀值（替換成真實 sensor API）────── */
// static float read_temperature(void) { return 25.0f + (esp_random() % 100) / 10.0f; }
// static float read_humidity(void)    { return 60.0f + (esp_random() % 100) / 10.0f; }


// static void obtain_time(void)
// {
//     ESP_LOGI("time", "開始 SNTP 校時");

//     sntp_setoperatingmode(SNTP_OPMODE_POLL);
//     sntp_setservername(0, "pool.ntp.org");
//     sntp_init();

//     time_t now = 0;
//     struct tm timeinfo = { 0 };

//     int retry = 0;
//     const int retry_count = 15;

//     while (timeinfo.tm_year < (2024 - 1900) &&
//            ++retry < retry_count)
//     {
//         ESP_LOGI("time", "等待系統時間更新... (%d/%d)",
//                  retry,
//                  retry_count);

//         vTaskDelay(pdMS_TO_TICKS(2000));

//         time(&now);
//         localtime_r(&now, &timeinfo);
//     }

//     ESP_LOGI("time",
//              "目前時間: %s",
//              asctime(&timeinfo));
// }


// /* ── Wi-Fi 事件處理 ──────────────────────────── */
// static void wifi_event_handler(void *arg, esp_event_base_t event_base,
//                                int32_t event_id, void *event_data)
// {
//     if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
//         esp_wifi_connect();
//     } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
//         if (s_retry_count < WIFI_MAX_RETRY) {
//             esp_wifi_connect();
//             s_retry_count++;
//             ESP_LOGW(TAG, "Wi-Fi 重連中... (%d/%d)", s_retry_count, WIFI_MAX_RETRY);
//         } else {
//             xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
//             ESP_LOGE(TAG, "Wi-Fi 連線失敗");
//         }
//     // } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
//     //     ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
//     //     ESP_LOGI(TAG, "取得 IP: " IPSTR, IP2STR(&event->ip_info.ip));
//     //     s_retry_count = 0;
//     //     xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
//     // }
//     }else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {

//         ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

//         ESP_LOGI(TAG, "取得 IP: " IPSTR,
//                 IP2STR(&event->ip_info.ip));

//         s_retry_count = 0;

//         xEventGroupSetBits(s_wifi_event_group,
//                         WIFI_CONNECTED_BIT);

//         ESP_LOGI(TAG, "開始 SNTP 校時");

//         obtain_time();

//         // 等待時間同步
//         vTaskDelay(pdMS_TO_TICKS(5000));

//         ESP_LOGI(TAG, "SNTP 校時完成");
//     }
// }

// /* ── Wi-Fi 初始化 ────────────────────────────── */
// static void wifi_init_sta(void)
// {
//     s_wifi_event_group = xEventGroupCreate();
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());
//     esp_netif_create_default_wifi_sta();

//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//     ESP_ERROR_CHECK(esp_event_handler_instance_register(
//         WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
//     ESP_ERROR_CHECK(esp_event_handler_instance_register(
//         IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

//     wifi_config_t wifi_config = {
//         .sta = {
//             .ssid     = WIFI_SSID,
//             .password = WIFI_PASSWORD,
//             .threshold.authmode = WIFI_AUTH_WPA2_PSK,
//         },
//     };
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
//     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
//     ESP_ERROR_CHECK(esp_wifi_start());

//     // 等待連線結果
//     EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
//         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

//     if (bits & WIFI_CONNECTED_BIT) {
//         ESP_LOGI(TAG, "Wi-Fi 連線成功 SSID: %s", WIFI_SSID);
//     } else {
//         ESP_LOGE(TAG, "Wi-Fi 連線失敗，請檢查帳密");
//     }
// }

// /* ── HTTP 事件 callback ──────────────────────── */
// static esp_err_t http_event_handler(esp_http_client_event_t *evt)
// {
//     switch (evt->event_id) {
//         case HTTP_EVENT_ERROR:
//             ESP_LOGW(TAG, "HTTP 錯誤");
//             break;
//         case HTTP_EVENT_ON_DATA:
//             // 印出 Firebase 回應（可選）
//             ESP_LOGI(TAG, "Firebase 回應: %.*s", evt->data_len, (char *)evt->data);
//             break;
//         default:
//             break;
//     }
//     return ESP_OK;
// }


// /* ── 上傳資料到 Firebase ─────────────────────── */
// static void firebase_send(float temperature, float humidity)
// {
//     // 建立 JSON payload
//     char payload[128];
//     snprintf(payload, sizeof(payload),
//              "{\"temperature\":%.2f,\"humidity\":%.2f}",
//              temperature, humidity);

//     // 組合完整 URL（含 auth secret）
//     char url[256];
//     snprintf(url, sizeof(url),
//              "%s%s?auth=%s",
//              FIREBASE_HOST, FIREBASE_PATH, FIREBASE_SECRET);

//     // esp_http_client_config_t config = {
//     //     .url            = url,
//     //     .method         = HTTP_METHOD_PUT,   // PUT = 覆寫；PATCH = 部分更新
//     //     .event_handler  = http_event_handler,
//     //     .transport_type = HTTP_TRANSPORT_OVER_SSL,
//     //     .skip_cert_common_name_check = true, // 生產環境建議關閉並驗證憑證
//     // };

//     // // 2. 在這裡更新你的配置
//     // esp_http_client_config_t config = {
//     //     .url            = url,
//     //     .method         = HTTP_METHOD_PUT,
//     //     .event_handler  = http_event_handler,
//     //     .crt_bundle_attach = esp_crt_bundle_attach, // 使用內建證書包進行安全驗證
//     //     // 移除原本的 skip_cert_common_name_check，因為有了 bundle 就不需要跳過檢查了
//     //     .keep_alive_enable = true, // 增加穩定性
//     // // 雖然 esp_http_client 預設會處理，但有些版本需確保網域名稱被帶入 TLS
//     // };
//     // esp_http_client_config_t config = {
//     //     .url = url,
//     //     .method = HTTP_METHOD_PUT,
//     //     .event_handler = http_event_handler,
//     //     .cert_pem = firebase_root_ca,         // 直接給它「身分證複印本」去核對
//     //     .transport_type = HTTP_TRANSPORT_OVER_SSL,
//     //     .skip_cert_common_name_check = false, // 既然有憑證了，就正式檢查網域
//     //     .keep_alive_enable = true,
//     // };

//     // esp_http_client_config_t config = {
//     // .url = url,
//     // .method = HTTP_METHOD_PUT,
//     // .cert_pem = google_root_ca,
//     // };

//     esp_http_client_config_t config = {
//     .url = url,
//     .method = HTTP_METHOD_PUT,
//     .transport_type = HTTP_TRANSPORT_OVER_SSL,
//     .crt_bundle_attach = esp_crt_bundle_attach,
//     };


//     esp_http_client_handle_t client = esp_http_client_init(&config);
//     esp_http_client_set_header(client, "Content-Type", "application/json");
//     esp_http_client_set_post_field(client, payload, strlen(payload));

//     esp_err_t err = esp_http_client_perform(client);
//     if (err == ESP_OK) {
//         int status = esp_http_client_get_status_code(client);
//         ESP_LOGI(TAG, "上傳成功 HTTP %d | temp=%.2f°C humi=%.2f%%",
//                  status, temperature, humidity);
//     } else {
//         ESP_LOGE(TAG, "上傳失敗: %s", esp_err_to_name(err));
//     }

//     esp_http_client_cleanup(client);
// }

// /* ── 主程式 ──────────────────────────────────── */
// void app_main(void)
// {
//     // 初始化 NVS（Wi-Fi 需要）
//     esp_err_t ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//         ESP_ERROR_CHECK(nvs_flash_erase());
//         ret = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(ret);

//     ESP_LOGI(TAG, "啟動 ESP32 Firebase 上傳範例");
//     wifi_init_sta();

//     // 主迴圈：讀感測器 → 上傳 Firebase
//     while (1) {
//         float temp = read_temperature();
//         float humi = read_humidity();
//         ESP_LOGI(TAG, "讀值 → 溫度: %.2f°C  濕度: %.2f%%", temp, humi);
//         firebase_send(temp, humi);
//         vTaskDelay(pdMS_TO_TICKS(UPLOAD_INTERVAL_MS));
//     }
// }


// #include <stdio.h>
// #include <string.h>
// #include <time.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/event_groups.h"
// #include "esp_system.h"
// #include "esp_wifi.h"
// #include "esp_event.h"
// #include "esp_log.h"
// #include "nvs_flash.h"
// #include "esp_netif.h"
// #include "esp_http_client.h"
// #include "esp_crt_bundle.h"
// #include "esp_sntp.h"
// #include "esp_adc/adc_oneshot.h"
// #include "ultrasonic.h"
// #include "hx711.h"

// /* ─── 使用者設定區 ─────────────────────────────── */
// #define WIFI_SSID           "阿欣"
// #define WIFI_PASSWORD       "903864328"

// #define FIREBASE_HOST       "https://esp-to-web-72def-default-rtdb.asia-southeast1.firebasedatabase.app"
// #define FIREBASE_SECRET     "FEUMMSOMzF3ERd3ko8kKrUjeDfeXWgKaFJ0LYzgH"
// #define FIREBASE_PATH       "/sensors/esp32.json"

// #define UPLOAD_INTERVAL_MS  1000

// /* ─── 腳位設定 ──────────────────────────────────── */
// #define MAX_DISTANCE_CM     100
// #define TRIGGER_GPIO        21
// #define ECHO_GPIO           22
// #define HX711_DOUT_GPIO     18
// #define HX711_SCK_GPIO      4
// #define LDR_ADC_CHAN        ADC_CHANNEL_6   // GPIO34

// /* ─── 全域變數 ──────────────────────────────────── */
// static const char *TAG = "FIREBASE_SENSOR";

// static EventGroupHandle_t   s_wifi_event_group;
// static bool                 s_wifi_connected = false;
// static bool                 s_time_synced    = false;

// #define WIFI_CONNECTED_BIT  BIT0
// #define WIFI_FAIL_BIT       BIT1
// #define WIFI_MAX_RETRY      5
// static int s_retry_count = 0;

// /* ─── 感測器 handle（全域，供 task 使用）─────────── */
// static ultrasonic_sensor_t      g_ultrasonic;
// static hx711_t                  g_hx711;
// static adc_oneshot_unit_handle_t g_adc1_handle;

// /* ── HX711 校正參數 ─────────────────────────────── */
// #define HX711_OFFSET    201360.3f
// #define HX711_SCALE     185945.3f

// /* ════════════════════════════════════════════════
//    SNTP 時間同步
//    ════════════════════════════════════════════════ */
// static void obtain_time(void)
// {
//     ESP_LOGI(TAG, "開始 SNTP 校時");
//     sntp_setoperatingmode(SNTP_OPMODE_POLL);
//     sntp_setservername(0, "pool.ntp.org");
//     sntp_init();

//     time_t now = 0;
//     struct tm timeinfo = {0};
//     int retry = 0;
//     const int retry_max = 20;

//     while (timeinfo.tm_year < (2024 - 1900) && ++retry < retry_max) {
//         ESP_LOGI(TAG, "等待時間同步... (%d/%d)", retry, retry_max);
//         vTaskDelay(pdMS_TO_TICKS(2000));
//         time(&now);
//         localtime_r(&now, &timeinfo);
//     }

//     if (timeinfo.tm_year >= (2024 - 1900)) {
//         s_time_synced = true;
//         // 設定台灣時區 UTC+8
//         setenv("TZ", "CST-8", 1);
//         tzset();
//         ESP_LOGI(TAG, "時間同步成功: %s", asctime(&timeinfo));
//     } else {
//         ESP_LOGW(TAG, "時間同步失敗，將使用 Unix timestamp=0");
//     }
// }

// /* ════════════════════════════════════════════════
//    Wi-Fi
//    ════════════════════════════════════════════════ */
// static void wifi_event_handler(void *arg, esp_event_base_t event_base,
//                                int32_t event_id, void *event_data)
// {
//     if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
//         esp_wifi_connect();

//     } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
//         s_wifi_connected = false;
//         if (s_retry_count < WIFI_MAX_RETRY) {
//             esp_wifi_connect();
//             s_retry_count++;
//             ESP_LOGW(TAG, "Wi-Fi 重連中... (%d/%d)", s_retry_count, WIFI_MAX_RETRY);
//         } else {
//             xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
//             ESP_LOGE(TAG, "Wi-Fi 連線失敗");
//         }

//     } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
//         ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
//         ESP_LOGI(TAG, "取得 IP: " IPSTR, IP2STR(&event->ip_info.ip));
//         s_retry_count   = 0;
//         s_wifi_connected = true;
//         xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);

//         // 拿到 IP 後立刻做 SNTP 校時
//         obtain_time();
//     }
// }

// static void wifi_init_sta(void)
// {
//     s_wifi_event_group = xEventGroupCreate();
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());
//     esp_netif_create_default_wifi_sta();

//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//     ESP_ERROR_CHECK(esp_event_handler_instance_register(
//         WIFI_EVENT, ESP_EVENT_ANY_ID,   &wifi_event_handler, NULL, NULL));
//     ESP_ERROR_CHECK(esp_event_handler_instance_register(
//         IP_EVENT,   IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

//     wifi_config_t wifi_config = {
//         .sta = {
//             .ssid      = WIFI_SSID,
//             .password  = WIFI_PASSWORD,
//             .threshold.authmode = WIFI_AUTH_WPA2_PSK,
//         },
//     };
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
//     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
//     ESP_ERROR_CHECK(esp_wifi_start());

//     EventBits_t bits = xEventGroupWaitBits(
//         s_wifi_event_group,
//         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
//         pdFALSE, pdFALSE, portMAX_DELAY);

//     if (bits & WIFI_CONNECTED_BIT) {
//         ESP_LOGI(TAG, "✅ Wi-Fi 連線成功 SSID: %s", WIFI_SSID);
//     } else {
//         ESP_LOGE(TAG, "❌ Wi-Fi 連線失敗，請檢查帳密");
//     }
// }

// /* ════════════════════════════════════════════════
//    感測器初始化
//    ════════════════════════════════════════════════ */
// static void sensors_init(void)
// {
//     // 超音波
//     g_ultrasonic.trigger_pin = TRIGGER_GPIO;
//     g_ultrasonic.echo_pin    = ECHO_GPIO;
//     ultrasonic_init(&g_ultrasonic);
//     ESP_LOGI(TAG, "超音波初始化完成");

//     // HX711
//     g_hx711.dout   = HX711_DOUT_GPIO;
//     g_hx711.pd_sck = HX711_SCK_GPIO;
//     g_hx711.gain   = HX711_GAIN_A_128;
//     ESP_ERROR_CHECK(hx711_init(&g_hx711));
//     ESP_LOGI(TAG, "HX711 初始化完成");

//     // ADC (光敏電阻)
//     adc_oneshot_unit_init_cfg_t init_config1 = {
//         .unit_id  = ADC_UNIT_1,
//         .ulp_mode = ADC_ULP_MODE_DISABLE,
//     };
//     ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &g_adc1_handle));

//     adc_oneshot_chan_cfg_t chan_cfg = {
//         .bitwidth = ADC_BITWIDTH_DEFAULT,
//         .atten    = ADC_ATTEN_DB_12,
//     };
//     ESP_ERROR_CHECK(adc_oneshot_config_channel(g_adc1_handle, LDR_ADC_CHAN, &chan_cfg));
//     ESP_LOGI(TAG, "ADC (LDR) 初始化完成");
// }

// /* ════════════════════════════════════════════════
//    讀取三組感測器
//    ════════════════════════════════════════════════ */
// static void sensors_read(float *distance_cm, float *weight_g, int *light_pct)
// {
//     // 距離
//     float dist_m = 0.0f;
//     if (ultrasonic_measure(&g_ultrasonic, MAX_DISTANCE_CM, &dist_m) == ESP_OK) {
//         *distance_cm = dist_m * 100.0f;
//     } else {
//         *distance_cm = -1.0f;   // 測量失敗標記
//     }

//     // 重量
//     int32_t raw = 0;
//     if (hx711_wait(&g_hx711, 500) == ESP_OK &&
//         hx711_read_data(&g_hx711, &raw) == ESP_OK) {
//         *weight_g = (float)(raw + HX711_OFFSET) * -100.0f / HX711_SCALE;
//     } else {
//         *weight_g = -1.0f;
//     }

//     // 亮度
//     int adc_raw = 0;
//     if (adc_oneshot_read(g_adc1_handle, LDR_ADC_CHAN, &adc_raw) == ESP_OK) {
//         *light_pct = (adc_raw * 100) / 4095;
//     } else {
//         *light_pct = -1;
//     }
// }

// /* ════════════════════════════════════════════════
//    HTTP 事件 callback
//    ════════════════════════════════════════════════ */
// static esp_err_t http_event_handler(esp_http_client_event_t *evt)
// {
//     if (evt->event_id == HTTP_EVENT_ON_DATA) {
//         ESP_LOGD(TAG, "Firebase 回應: %.*s", evt->data_len, (char *)evt->data);
//     }
//     return ESP_OK;
// }

// /* ════════════════════════════════════════════════
//    上傳到 Firebase
//    ════════════════════════════════════════════════ */
// static void firebase_send(float distance_cm, float weight_g, int light_pct,
//                           const char *timestamp_str, bool wifi_just_connected)
// {
//     // ── 組 JSON payload ──────────────────────────
//     char payload[256];

//     if (wifi_just_connected) {
//         // 第一筆：標記網路連線成功事件
//         snprintf(payload, sizeof(payload),
//                  "{"
//                  "\"timestamp\":\"%s\","
//                  "\"event\":\"wifi_connected\","
//                  "\"distance_cm\":%.2f,"
//                  "\"weight_g\":%.2f,"
//                  "\"light_pct\":%d"
//                  "}",
//                  timestamp_str, distance_cm, weight_g, light_pct);
//     } else {
//         snprintf(payload, sizeof(payload),
//                  "{"
//                  "\"timestamp\":\"%s\","
//                  "\"distance_cm\":%.2f,"
//                  "\"weight_g\":%.2f,"
//                  "\"light_pct\":%d"
//                  "}",
//                  timestamp_str, distance_cm, weight_g, light_pct);
//     }

//     // ── 組 URL（用 POST 到 /sensors/esp32/log.json 讓 Firebase 自動產生 key）
//     //    若要固定覆寫同一節點請改回 PUT + FIREBASE_PATH
//     char url[256];
//     snprintf(url, sizeof(url),
//              "%s/sensors/esp32/log.json?auth=%s",
//              FIREBASE_HOST, FIREBASE_SECRET);

//     esp_http_client_config_t config = {
//         .url               = url,
//         .method            = HTTP_METHOD_POST,   // POST = 每筆自動建 key（時序列）
//         .event_handler     = http_event_handler,
//         .transport_type    = HTTP_TRANSPORT_OVER_SSL,
//         .crt_bundle_attach = esp_crt_bundle_attach,
//     };

//     esp_http_client_handle_t client = esp_http_client_init(&config);
//     esp_http_client_set_header(client, "Content-Type", "application/json");
//     esp_http_client_set_post_field(client, payload, strlen(payload));

//     esp_err_t err = esp_http_client_perform(client);
//     if (err == ESP_OK) {
//         int status = esp_http_client_get_status_code(client);
//         ESP_LOGI(TAG, "✅ 上傳成功 HTTP %d | %s | dist=%.1fcm wt=%.1fg lux=%d%%",
//                  status, timestamp_str, distance_cm, weight_g, light_pct);
//     } else {
//         ESP_LOGE(TAG, "❌ 上傳失敗: %s", esp_err_to_name(err));
//     }

//     esp_http_client_cleanup(client);
// }

// /* ════════════════════════════════════════════════
//    取得時間字串 (ISO-8601)
//    ════════════════════════════════════════════════ */
// static void get_timestamp(char *buf, size_t len)
// {
//     time_t now;
//     struct tm timeinfo;
//     time(&now);
//     localtime_r(&now, &timeinfo);

//     if (s_time_synced && timeinfo.tm_year >= (2024 - 1900)) {
//         // 格式: 2025-05-20T14:30:00+08:00
//         strftime(buf, len, "%Y-%m-%dT%H:%M:%S+08:00", &timeinfo);
//     } else {
//         // 時間未同步，記錄啟動後的秒數
//         snprintf(buf, len, "uptime_%lus", (unsigned long)(now));
//     }
// }

// /* ════════════════════════════════════════════════
//    app_main
//    ════════════════════════════════════════════════ */
// void app_main(void)
// {
//     // NVS 初始化
//     esp_err_t ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//         ESP_ERROR_CHECK(nvs_flash_erase());
//         ret = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(ret);

//     ESP_LOGI(TAG, "🚀 系統啟動");

//     // 感測器優先初始化（不需要網路）
//     sensors_init();

//     // Wi-Fi 連線（含 SNTP 校時）
//     wifi_init_sta();

//     // 連線成功後，第一筆資料標記 wifi_connected
//     bool first_upload = true;

//     // ── 主迴圈 ─────────────────────────────────
//     while (1) {
//         if (!s_wifi_connected) {
//             ESP_LOGW(TAG, "Wi-Fi 未連線，等待重連...");
//             vTaskDelay(pdMS_TO_TICKS(2000));
//             continue;
//         }

//         // 讀感測器
//         float distance_cm, weight_g;
//         int   light_pct;
//         sensors_read(&distance_cm, &weight_g, &light_pct);

//         // 取時間戳
//         char timestamp[32];
//         get_timestamp(timestamp, sizeof(timestamp));

//         // 上傳
//         firebase_send(distance_cm, weight_g, light_pct, timestamp, first_upload);
//         first_upload = false;

//         vTaskDelay(pdMS_TO_TICKS(UPLOAD_INTERVAL_MS));
//     }
// }

// #include <stdio.h>
// #include <string.h>
// #include <time.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/event_groups.h"
// #include "esp_system.h"
// #include "esp_wifi.h"
// #include "esp_event.h"
// #include "nvs_flash.h"
// #include "esp_netif.h"
// #include "esp_tls.h"
// #include "esp_http_client.h"
// #include "esp_crt_bundle.h"
// #include "esp_sntp.h"
// #include "ultrasonic.h"
// #include "hx711.h"
// #include "esp_adc/adc_oneshot.h"
// #include "esp_task_wdt.h"

// // ─── 使用者設定區 ───────────────────────────────
// #define WIFI_SSID         "阿欣"
// #define WIFI_PASSWORD     "903864328"
// // #define WIFI_SSID         "zongfu_iphone"
// // #define WIFI_PASSWORD     "Shamballaaaa"
// #define FIREBASE_HOST     "https://esp-to-web-72def-default-rtdb.asia-southeast1.firebasedatabase.app"
// #define FIREBASE_SECRET   "FEUMMSOMzF3ERd3ko8kKrUjeDfeXWgKaFJ0LYzgH"

// // ─── 感測器腳位設定 ─────────────────────────────
// #define MAX_DISTANCE_CM   400
// #define TRIGGER_GPIO      21            // 已避開 Flash/PSRAM 衝突腳位
// #define ECHO_GPIO         22            // 已避開 Flash/PSRAM 衝突腳位
// #define HX711_DOUT_GPIO   18
// #define HX711_SCK_GPIO    4
// #define LDR_ADC_CHAN      ADC_CHANNEL_6 // GPIO34 屬於 ADC1，與 Wi-Fi 完全不衝突

// #define HX711_OFFSET      198255.3f
// #define HX711_SCALE       185945.3f

// // ─── 修改：上傳間隔改為 10 秒 (10000 毫秒) ────────
// #define UPLOAD_INTERVAL_MS 10000
// // ───────────────────────────────────────────────

// static EventGroupHandle_t s_wifi_event_group;
// #define WIFI_CONNECTED_BIT  BIT0

// static bool s_wifi_connected = false;
// static bool s_time_synced    = false;

// static ultrasonic_sensor_t g_ultrasonic;
// static hx711_t             g_hx711;
// static adc_oneshot_unit_handle_t g_adc1_handle;

// /* ════════════════════════════════════════════════
//    SNTP 時間同步 (已移除 Log 與 printf)
//    ════════════════════════════════════════════════ */
// // static void obtain_time(void)
// // {
// //     sntp_setoperatingmode(SNTP_OPMODE_POLL);
// //     sntp_setservername(0, "pool.ntp.org");
// //     sntp_init();

// //     time_t now = 0;
// //     struct tm timeinfo = {0};
// //     int retry = 0;
// //     const int retry_max = 20;

// //     while (timeinfo.tm_year < (2024 - 1900) && ++retry < retry_max) {
// //         vTaskDelay(pdMS_TO_TICKS(2000));
// //         time(&now);
// //         localtime_r(&now, &timeinfo);
// //     }

// //     if (timeinfo.tm_year >= (2024 - 1900)) {
// //         s_time_synced = true;
// //         setenv("TZ", "CST-8", 1); // 設定台灣時區 UTC+8
// //         tzset();
// //     }
// // }

// static void obtain_time(void)
// {
//     // 改用 esp_sntp_xxx 函數
//     esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
//     esp_sntp_setservername(0, "pool.ntp.org");
//     esp_sntp_init();

//     time_t now = 0;
//     struct tm timeinfo = {0};
//     int retry = 0;
//     const int retry_max = 20;

//     while (timeinfo.tm_year < (2024 - 1900) && ++retry < retry_max) {
//         vTaskDelay(pdMS_TO_TICKS(2000));
//         time(&now);
//         localtime_r(&now, &timeinfo);
//     }

//     if (timeinfo.tm_year >= (2024 - 1900)) {
//         s_time_synced = true;
//         setenv("TZ", "CST-8", 1); // 設定台灣時區
//         tzset();
//     }
// }

// /* ════════════════════════════════════════════════
//    Wi-Fi 事件處理 (升級版：移除次數限制，斷線背景無限重連)
//    ════════════════════════════════════════════════ */
// static void wifi_event_handler(void *arg, esp_event_base_t event_base,
//                                int32_t event_id, void *event_data)
// {
//     if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
//         esp_wifi_connect();

//     } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
//         s_wifi_connected = false;
//         // 核心修正：只要斷線，不論失敗幾次，永遠在背景嘗試重新連線，不向主程式拋出 FAIL 狀態
//         esp_wifi_connect();

//     } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
//         s_wifi_connected = true;
//         xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);

//         // 拿到 IP 後立刻做 SNTP 校時
//         obtain_time();
//     }
// }

// static void wifi_init_sta(void)
// {
//     s_wifi_event_group = xEventGroupCreate();
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());
//     esp_netif_create_default_wifi_sta();

//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//     ESP_ERROR_CHECK(esp_event_handler_instance_register(
//         WIFI_EVENT, ESP_EVENT_ANY_ID,   &wifi_event_handler, NULL, NULL));
//     ESP_ERROR_CHECK(esp_event_handler_instance_register(
//         IP_EVENT,   IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

//     wifi_config_t wifi_config = {
//         .sta = {
//             .ssid      = WIFI_SSID,
//             .password  = WIFI_PASSWORD,
//             .threshold.authmode = WIFI_AUTH_WPA2_PSK,
//         },
//     };
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
//     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
//     ESP_ERROR_CHECK(esp_wifi_start());

//     // 核心修正：開機最多等 10 秒，連不上也不卡死，讓感測器主迴圈先跑，Wi-Fi 會在背景繼續重連
//     xEventGroupWaitBits(
//         s_wifi_event_group,
//         WIFI_CONNECTED_BIT,
//         pdFALSE, pdFALSE, portMAX_DELAY);
// }

// /* ════════════════════════════════════════════════
//    感測器初始化 (已移除 Log)
//    ════════════════════════════════════════════════ */
// static void sensors_init(void)
// {
//     // 超音波
//     g_ultrasonic.trigger_pin = TRIGGER_GPIO;
//     g_ultrasonic.echo_pin    = ECHO_GPIO;
//     ultrasonic_init(&g_ultrasonic);

//     // HX711
//     g_hx711.dout   = HX711_DOUT_GPIO;
//     g_hx711.pd_sck = HX711_SCK_GPIO;
//     g_hx711.gain   = HX711_GAIN_A_128;
//     ESP_ERROR_CHECK(hx711_init(&g_hx711));

//     // ADC (光敏電阻)
//     adc_oneshot_unit_init_cfg_t init_config1 = {
//         .unit_id  = ADC_UNIT_1,
//         .ulp_mode = ADC_ULP_MODE_DISABLE,
//     };
//     ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &g_adc1_handle));

//     adc_oneshot_chan_cfg_t chan_cfg = {
//         .bitwidth = ADC_BITWIDTH_DEFAULT,
//         .atten    = ADC_ATTEN_DB_12,
//     };
//     ESP_ERROR_CHECK(adc_oneshot_config_channel(g_adc1_handle, LDR_ADC_CHAN, &chan_cfg));
// }

// // /* ════════════════════════════════════════════════
// //    讀取三組感測器
// //    ════════════════════════════════════════════════ */
// // static void sensors_read(float *distance_cm, float *weight_g, int *light_pct)
// // {
// //     // 距離
// //     float dist_m = 0.0f;
// //     if (ultrasonic_measure(&g_ultrasonic, MAX_DISTANCE_CM, &dist_m) == ESP_OK) {
// //         *distance_cm = dist_m * 100.0f;
// //     } else {
// //         *distance_cm = -1.0f;   
// //     }

// //     // 重量
// //     int32_t raw = 0;
// //     if (hx711_wait(&g_hx711, 500) == ESP_OK &&
// //         hx711_read_data(&g_hx711, &raw) == ESP_OK) {
// //         *weight_g = (float)(raw + HX711_OFFSET) * -100.0f / HX711_SCALE;
// //     } else {
// //         *weight_g = -1.0f;
// //     }

// //     // 亮度
// //     int adc_raw = 0;
// //     if (adc_oneshot_read(g_adc1_handle, LDR_ADC_CHAN, &adc_raw) == ESP_OK) {
// //         *light_pct = (adc_raw * 100) / 4095;
// //     } else {
// //         *light_pct = -1;
// //     }
// // }


// /* ════════════════════════════════════════════════
//    讀取三組感測器 (優化版：縮短 HX711 等待時間)
//    ════════════════════════════════════════════════ */
// static void sensors_read(float *distance_cm, float *weight_g, int *light_pct)
// {
//     // 距離
//     float dist_m = 0.0f;
//     if (ultrasonic_measure(&g_ultrasonic, MAX_DISTANCE_CM, &dist_m) == ESP_OK) {
//         *distance_cm = dist_m * 100.0f;
//     } else {
//         *distance_cm = -1.0f;   
//     }

//     // 重量 (核心修正：將原本的 500ms 阻塞縮短到 100ms，大幅降低與 Wi-Fi 中斷撞車的死鎖機率)
//     int32_t raw = 0;
//     if (hx711_wait(&g_hx711, 100) == ESP_OK &&
//         hx711_read_data(&g_hx711, &raw) == ESP_OK) {
//         *weight_g = (float)(raw + HX711_OFFSET) * -100.0f / HX711_SCALE;
//     } else {
//         *weight_g = -1.0f;
//     }

//     // 亮度
//     int adc_raw = 0;
//     if (adc_oneshot_read(g_adc1_handle, LDR_ADC_CHAN, &adc_raw) == ESP_OK) {
//         *light_pct = (adc_raw * 100) / 4095;
//     } else {
//         *light_pct = -1;
//     }
// }

// static esp_err_t http_event_handler(esp_http_client_event_t *evt)
// {
//     return ESP_OK;
// }

// /* ════════════════════════════════════════════════
//    上傳到 Firebase (已移除 Log 與安全略過設定)
//    ════════════════════════════════════════════════ */
// static void firebase_send(float distance_cm, float weight_g, int light_pct,
//                           const char *timestamp_str, bool wifi_just_connected)
// {
//     char payload[256];

//     if (wifi_just_connected) {
//         snprintf(payload, sizeof(payload),
//                  "{"
//                  "\"timestamp\":\"%s\","
//                  "\"event\":\"wifi_connected\","
//                  "\"distance_cm\":%.2f,"
//                  "\"weight_g\":%.2f,"
//                  "\"light_pct\":%d"
//                  "}",
//                  timestamp_str, distance_cm, weight_g, light_pct);
//     } else {
//         snprintf(payload, sizeof(payload),
//                  "{"
//                  "\"timestamp\":\"%s\","
//                  "\"distance_cm\":%.2f,"
//                  "\"weight_g\":%.2f,"
//                  "\"light_pct\":%d"
//                  "}",
//                  timestamp_str, distance_cm, weight_g, light_pct);
//     }

//     char url[256];
//     // 已對齊網頁端路徑 /sensors/esp32/log.json
//     snprintf(url, sizeof(url),
//              "%s/sensors/esp32/log.json?auth=%s",
//              FIREBASE_HOST, FIREBASE_SECRET);

//     esp_http_client_config_t config = {
//         .url               = url,
//         .method            = HTTP_METHOD_POST,   
//         .event_handler     = http_event_handler,
//         .transport_type    = HTTP_TRANSPORT_OVER_SSL,
//         .crt_bundle_attach = esp_crt_bundle_attach,
//         .keep_alive_enable = true,
//     };

//     esp_http_client_handle_t client = esp_http_client_init(&config);
//     esp_http_client_set_header(client, "Content-Type", "application/json");
//     esp_http_client_set_post_field(client, payload, strlen(payload));

//     // 默默執行，不論成功失敗皆不阻塞或輸出 Log
//     esp_http_client_perform(client);
//     esp_http_client_cleanup(client);
// }

// /* ════════════════════════════════════════════════
//    取得時間字串 (ISO-8601)
//    ════════════════════════════════════════════════ */
// static void get_timestamp(char *buf, size_t len)
// {
//     time_t now;
//     struct tm timeinfo;
//     time(&now);
//     localtime_r(&now, &timeinfo);

//     if (s_time_synced && timeinfo.tm_year >= (2024 - 1900)) {
//         strftime(buf, len, "%Y-%m-%dT%H:%M:%S+08:00", &timeinfo);
//     } else {
//         snprintf(buf, len, "uptime_%lus", (unsigned long)(now));
//     }
// }

// /* ════════════════════════════════════════════════
//    app_main
//    ════════════════════════════════════════════════ */
// // void app_main(void)
// // {
// //     esp_err_t ret = nvs_flash_init();
// //     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
// //         ESP_ERROR_CHECK(nvs_flash_erase());
// //         ret = nvs_flash_init();
// //     }
// //     ESP_ERROR_CHECK(ret);

// //     // 感測器優先初始化（不依賴網路）
// //     sensors_init();

// //     // 啟動 Wi-Fi 背景任務
// //     wifi_init_sta();

// //     bool first_upload = true;

// //     while (1) {
// //         // 如果網路沒連上，直接跳過本輪，等待 2 秒後再確認，避免盲目調用 HTTPS 造成死鎖
// //         if (!s_wifi_connected) {
// //             vTaskDelay(pdMS_TO_TICKS(2000));
// //             continue;
// //         }

// //         // 讀感測器
// //         float distance_cm, weight_g;
// //         int   light_pct;
// //         sensors_read(&distance_cm, &weight_g, &light_pct);

// //         // 取時間戳
// //         char timestamp[32];
// //         get_timestamp(timestamp, sizeof(timestamp));

// //         // 上傳資料
// //         firebase_send(distance_cm, weight_g, light_pct, timestamp, first_upload);
// //         first_upload = false;

// //         // 延遲 10 秒後再進行下一輪偵測與上傳
// //         vTaskDelay(pdMS_TO_TICKS(UPLOAD_INTERVAL_MS));
// //     }
// // }

// void app_main(void)
// {
//     esp_err_t ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//         ESP_ERROR_CHECK(nvs_flash_erase());
//         ret = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(ret);

//     // ─── 🛠️ ESP-IDF v6.0 標準看門狗配置 ───
//     esp_task_wdt_config_t twdt_config = {
//         .timeout_ms = 15000,                  // 15 秒超時
//         .idle_core_mask = 3,                  // 二進制 011 代表同時監控 Core 0 與 Core 1
//         .trigger_panic = true                 // 超時直接重啟
//     };
//     // 在 v6.0 中，必須使用 esp_task_wdt_reconfigure
//     esp_task_wdt_reconfigure(&twdt_config);
//     esp_task_wdt_add(NULL);                   // 將主任務加入看門狗監控
//     // ─────────────────────────────────────

//     // 感測器優先初始化（不依賴網路）
//     sensors_init();

//     // 啟動 Wi-Fi 背景任務
//     wifi_init_sta();

//     bool first_upload = true;

//     while (1) {
//         // 正常跑完一圈就餵狗
//         esp_task_wdt_reset(); 

//         if (!s_wifi_connected) {
//             vTaskDelay(pdMS_TO_TICKS(2000));
//             continue;
//         }

//         // 讀感測器
//         float distance_cm, weight_g;
//         int   light_pct;
//         sensors_read(&distance_cm, &weight_g, &light_pct);

//         // 取時間戳
//         char timestamp[32];
//         get_timestamp(timestamp, sizeof(timestamp));

//         // 上傳資料
//         firebase_send(distance_cm, weight_g, light_pct, timestamp, first_upload);
//         first_upload = false;

//         // 延遲 10 秒
//         vTaskDelay(pdMS_TO_TICKS(UPLOAD_INTERVAL_MS));
//     }
// }

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_sntp.h"
#include "driver/gpio.h"
#include "ultrasonic.h"
#include "hx711.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_task_wdt.h"

// ─── 使用者設定區 ───────────────────────────────
#define WIFI_SSID         "阿欣"
#define WIFI_PASSWORD     "903864328"
#define FIREBASE_HOST     "https://esp-to-web-72def-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_SECRET   "FEUMMSOMzF3ERd3ko8kKrUjeDfeXWgKaFJ0LYzgH"

// ─── 感測器腳位設定 ─────────────────────────────
#define MAX_DISTANCE_CM   400
#define TRIGGER_GPIO      21            // 已避開 Flash/PSRAM 衝突腳位
#define ECHO_GPIO         22            // 已避開 Flash/PSRAM 衝突腳位
#define HX711_DOUT_GPIO   18
#define HX711_SCK_GPIO    4
#define LDR_ADC_CHAN      ADC_CHANNEL_6 // GPIO34 屬於 ADC1，與 Wi-Fi 完全不衝突

// ─── 新增：Nano 33 BLE 硬體訊號輸入腳位 ──────────
#define INPUT_HAPPY_GPIO    25          // 接收 Happy 訊號
#define INPUT_SOTIRED_GPIO  26          // 接收 Sotired 訊號

#define HX711_OFFSET      198255.3f
#define HX711_SCALE       185945.3f
#define UPLOAD_INTERVAL_MS 10000        // 感測器上傳間隔 10 秒
// ───────────────────────────────────────────────

static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT  BIT0

static bool s_wifi_connected = false;
static bool s_time_synced    = false;

static ultrasonic_sensor_t g_ultrasonic;
static hx711_t             g_hx711;
static adc_oneshot_unit_handle_t g_adc1_handle;

// ─── 函數宣告 ───────────────────────────────────
static void get_timestamp_key(char *buf, size_t len);
static void get_timestamp_iso(char *buf, size_t len);
static esp_err_t http_event_handler(esp_http_client_event_t *evt) { return ESP_OK; }
static void sensors_read(float *distance_cm, float *weight_g, int *light_pct);

/* ════════════════════════════════════════════════
   SNTP 時間同步
   ════════════════════════════════════════════════ */
static void obtain_time(void)
{
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_init();

    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_max = 20;

    while (timeinfo.tm_year < (2024 - 1900) && ++retry < retry_max) {
        vTaskDelay(pdMS_TO_TICKS(2000));
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    if (timeinfo.tm_year >= (2024 - 1900)) {
        s_time_synced = true;
        setenv("TZ", "CST-8", 1); // 設定台灣時區 UTC+8
        tzset();
    }
}

/* ════════════════════════════════════════════════
   Wi-Fi 事件處理 (斷線背景無限重連)
   ════════════════════════════════════════════════ */
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        s_wifi_connected = false;
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        s_wifi_connected = true;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        obtain_time();
    }
}

static void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid      = WIFI_SSID,
            .password  = WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
}

/* ════════════════════════════════════════════════
   硬體腳位初始化 (包含感測器與新增的輸入 GPIO)
   ════════════════════════════════════════════════ */
static void hardware_init(void)
{
    // 1. 超音波
    g_ultrasonic.trigger_pin = TRIGGER_GPIO;
    g_ultrasonic.echo_pin    = ECHO_GPIO;
    ultrasonic_init(&g_ultrasonic);

    // 2. HX711
    g_hx711.dout   = HX711_DOUT_GPIO;
    g_hx711.pd_sck = HX711_SCK_GPIO;
    g_hx711.gain   = HX711_GAIN_A_128;
    ESP_ERROR_CHECK(hx711_init(&g_hx711));

    // 3. ADC (光敏電阻)
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id  = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &g_adc1_handle));
    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten    = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(g_adc1_handle, LDR_ADC_CHAN, &chan_cfg));

    // 4. 新增：初始化 GPIO 25, 26 作為純數位輸入，下拉電阻防止浮空
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << INPUT_HAPPY_GPIO) | (1ULL << INPUT_SOTIRED_GPIO),
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE
    };
    gpio_config(&io_conf);
}

/* ════════════════════════════════════════════════
   優化版 Firebase POST ➔ 改為 PUT 達成客製化 Key 排序
   ════════════════════════════════════════════════ */
static void firebase_send_sensors(float distance_cm, float weight_g, int light_pct, const char *ts_key, const char *ts_iso)
{
    char payload[256];
    snprintf(payload, sizeof(payload),
             "{"
             "\"timestamp\":\"%s\","
             "\"distance_cm\":%.2f,"
             "\"weight_g\":%.2f,"
             "\"light_pct\":%d"
             "}",
             ts_iso, distance_cm, weight_g, light_pct);

    char url[320];
    // 💡 關鍵修改：路徑改為 /sensors/log/時間戳記.json，並使用 PUT 請求，完美取代亂數 Push ID
    snprintf(url, sizeof(url),
             "%s/sensors/log/%s.json?auth=%s",
             FIREBASE_HOST, ts_key, FIREBASE_SECRET);

    esp_http_client_config_t config = {
        .url               = url,
        .method            = HTTP_METHOD_PUT, // 使用 PUT 直接寫入指定的 Key 節點
        .event_handler     = http_event_handler,
        .transport_type    = HTTP_TRANSPORT_OVER_SSL,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .keep_alive_enable = true,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, payload, strlen(payload));
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

static void firebase_send_voice(const char *voice_label, const char *ts_key, const char *ts_iso)
{
    char payload[128];
    snprintf(payload, sizeof(payload),
             "{"
             "\"timestamp\":\"%s\","
             "\"voice_command\":\"%s\""
             "}",
             ts_iso, voice_label);

    char url[320];
    // 💡 語音結果同樣儲存在 /voice/log/時間戳記.json 中，後端極好撈取
    snprintf(url, sizeof(url),
             "%s/voice/log/%s.json?auth=%s",
             FIREBASE_HOST, ts_key, FIREBASE_SECRET);

    esp_http_client_config_t config = {
        .url               = url,
        .method            = HTTP_METHOD_PUT,
        .event_handler     = http_event_handler,
        .transport_type    = HTTP_TRANSPORT_OVER_SSL,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .keep_alive_enable = true,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, payload, strlen(payload));
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

/* ════════════════════════════════════════════════
   核心任務一：原生 3 組感測器定時上傳 (配置於 Core 0)
   ════════════════════════════════════════════════ */
void sensor_upload_task(void *pvParameters)
{
    esp_task_wdt_add(NULL); // 納入看門狗監控

    while (1) {
        esp_task_wdt_reset(); // 正常餵狗

        if (!s_wifi_connected) {
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        float distance_cm, weight_g;
        int   light_pct;
        sensors_read(&distance_cm, &weight_g, &light_pct);

        char ts_key[32];
        char ts_iso[32];
        get_timestamp_key(ts_key, sizeof(ts_key));
        get_timestamp_iso(ts_iso, sizeof(ts_iso));

        // 發送感測器資料
        firebase_send_sensors(distance_cm, weight_g, light_pct, ts_key, ts_iso);

        vTaskDelay(pdMS_TO_TICKS(UPLOAD_INTERVAL_MS));
    }
}

/* ════════════════════════════════════════════════
   核心任務二：高效讀取 Nano 33 實體腳位 (配置於 Core 1)
   ════════════════════════════════════════════════ */
void nano_gpio_scan_task(void *pvParameters)
{
    while (1) {
        // 如果網路沒通，先不處理硬體接收
        if (!s_wifi_connected) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        // 讀取實體 GPIO 電平狀態
        int happy_state   = gpio_get_level(INPUT_HAPPY_GPIO);
        int sotired_state = gpio_get_level(INPUT_SOTIRED_GPIO);

        if (happy_state == 1) {
            char ts_key[32]; char ts_iso[32];
            get_timestamp_key(ts_key, sizeof(ts_key));
            get_timestamp_iso(ts_iso, sizeof(ts_iso));

            firebase_send_voice("happy", ts_key, ts_iso);
            
            // 延遲 600ms 跳過 Nano 剩餘的脈衝寬度，精準防止重複觸發
            vTaskDelay(pdMS_TO_TICKS(600)); 
        } 
        else if (sotired_state == 1) {
            char ts_key[32]; char ts_iso[32];
            get_timestamp_key(ts_key, sizeof(ts_key));
            get_timestamp_iso(ts_iso, sizeof(ts_iso));

            firebase_send_voice("sotired", ts_key, ts_iso);

            vTaskDelay(pdMS_TO_TICKS(600));
        }

        // 輪詢頻率設定為 20ms，反應速度極快且不佔用 CPU 資源
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

/* ════════════════════════════════════════════════
   時間字串處理
   ════════════════════════════════════════════════ */
static void get_timestamp_key(char *buf, size_t len)
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    if (s_time_synced && timeinfo.tm_year >= (2024 - 1900)) {
        // 格式：20260604_011230 (用於 Firebase 鍵值，避開禁止字元 '.'、'#'、'$'、'['、']')
        strftime(buf, len, "%Y%m%d_%H%M%S", &timeinfo);
    } else {
        snprintf(buf, len, "uptime_%lus", (unsigned long)(now));
    }
}

static void get_timestamp_iso(char *buf, size_t len)
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    if (s_time_synced && timeinfo.tm_year >= (2024 - 1900)) {
        strftime(buf, len, "%Y-%m-%dT%H:%M:%S+08:00", &timeinfo);
    } else {
        snprintf(buf, len, "uptime_%lus", (unsigned long)(now));
    }
}

static void sensors_read(float *distance_cm, float *weight_g, int *light_pct)
{
    float dist_m = 0.0f;
    if (ultrasonic_measure(&g_ultrasonic, MAX_DISTANCE_CM, &dist_m) == ESP_OK) {
        *distance_cm = dist_m * 100.0f;
    } else {
        *distance_cm = -1.0f;   
    }

    int32_t raw = 0;
    if (hx711_wait(&g_hx711, 100) == ESP_OK && hx711_read_data(&g_hx711, &raw) == ESP_OK) {
        *weight_g = (float)(raw + HX711_OFFSET) * -100.0f / HX711_SCALE;
    } else {
        *weight_g = -1.0f;
    }

    int adc_raw = 0;
    if (adc_oneshot_read(g_adc1_handle, LDR_ADC_CHAN, &adc_raw) == ESP_OK) {
        *light_pct = (adc_raw * 100) / 4095;
    } else {
        *light_pct = -1;
    }
}

/* ════════════════════════════════════════════════
   app_main 主入口
   ════════════════════════════════════════════════ */
void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 🛠️ ESP-IDF v6.0 標準看門狗配置
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = 15000,
        .idle_core_mask = 3, // 同時看守 Core 0 與 1
        .trigger_panic = true 
    };
    esp_task_wdt_reconfigure(&twdt_config);

    // 硬體與網路初始化
    hardware_init();
    wifi_init_sta();

    // 🚀 分流雙核並行任務
    // 任務一：扔給 Core 0，負責感測器定時收集 (與看門狗綁定)
    xTaskCreatePinnedToCore(sensor_upload_task, "sensor_task", 4096, NULL, 5, NULL, 0);
    
    // 任務二：扔給 Core 1，高靈敏度即時捕捉 Nano 腳位狀態 (不鎖定看門狗，保證上傳網路遲延時不當機)
    xTaskCreatePinnedToCore(nano_gpio_scan_task, "nano_gpio_task", 4096, NULL, 6, NULL, 1);
}