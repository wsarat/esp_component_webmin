#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

#define WEBMIN_NVS_NAME "webmin_settings"

static const char *TAG = "WEBMIN_SETTINGS";

esp_err_t webmin_read_settings_str(const char *key, char *value) {
    esp_err_t err;
    // Open NVS handle
    ESP_LOGI(TAG, "\nOpening Non-Volatile Storage (NVS) handle...");
    nvs_handle_t my_handle;
    err = nvs_open(WEBMIN_NVS_NAME, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return ESP_FAIL;
    }

    size_t required_size = 0;
    ESP_LOGI(TAG, "\nReading string from NVS...");
    err = nvs_get_str(my_handle, "message", value, &required_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) nvs_get_str!", esp_err_to_name(err));
        return ESP_FAIL;   
    }
    ESP_LOGI(TAG, "Read string: %s", value);

    // Close
    nvs_close(my_handle);
    ESP_LOGI(TAG, "NVS handle closed.");

    return ESP_OK;
}

esp_err_t webmin_write_settings_str(const char *key, char *value) {
    esp_err_t err;

    // Open NVS handle
    ESP_LOGI(TAG, "\nOpening Non-Volatile Storage (NVS) handle...");
    nvs_handle_t my_handle;
    err = nvs_open(WEBMIN_NVS_NAME, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return ESP_FAIL;
    }

    // Store and read a string
    ESP_LOGI(TAG, "\nWriting string to NVS...");
    err = nvs_set_str(my_handle, key, value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write string!");
        nvs_close(my_handle);
        return ESP_FAIL;
    }
    // Commit changes
    // After setting any values, nvs_commit() must be called to ensure changes are written
    // to flash storage. Implementations may write to storage at other times,
    // but this is not guaranteed.
    ESP_LOGI(TAG, "\nCommitting updates in NVS...");
    err = nvs_commit(my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit NVS changes!");
        nvs_close(my_handle);
        return ESP_FAIL;
    }

    // Close
    nvs_close(my_handle);
    ESP_LOGI(TAG, "NVS handle closed.");

    return ESP_OK;
}