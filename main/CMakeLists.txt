# idf_component_register(
#     SRCS
#         "main.c"
#     INCLUDE_DIRS
#         "."
#     REQUIRES
#         nvs_flash        # NVS 儲存（Wi-Fi 設定需要）
#         esp_wifi         # Wi-Fi 驅動
#         esp_event        # 事件迴圈
#         esp_netif        # 網路介面抽象層
#         esp_http_client  # HTTP / HTTPS client
#         esp-tls          # TLS（HTTPS 底層）
#         freertos         # FreeRTOS task / event group
#         log              # ESP_LOGI / ESP_LOGE
#         mbedtls   # 給licence
# )

#整合程式
idf_component_register(
    SRCS
        "main.c"
    INCLUDE_DIRS
        "."
    REQUIRES
        nvs_flash
        esp_wifi
        esp_event
        esp_netif
        esp_http_client
        esp-tls
        freertos
        log
        mbedtls
        esp_adc
        ultrasonic
        hx711
        driver
)
