idf_component_register(SRCS "webmin.c" "wifi_sta.c" "settings.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES esp_http_server esp-tls esp_wifi nvs_flash
                    EMBED_TXTFILES index.html)
