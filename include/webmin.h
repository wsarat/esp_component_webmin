#define WEBMIN_KEY_WIFI_SSID "WEBMIN_KEY_WIFI_SSID"
#define WEBMIN_KEY_WIFI_PASS "WEBMIN_KEY_WIFI_PASS"
#define WEBMIN_KEY_WIFI_AUTH "WEBMIN_KEY_WIFI_AUTH"

void webmin_wifi_init(void);
void webmin_start(void);
esp_err_t webmin_read_settings_str(const char *key, char *value);
esp_err_t webmin_write_settings_str(const char *key, char *value);