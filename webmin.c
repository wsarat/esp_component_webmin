#include <stdio.h>
#include <esp_http_server.h>
#include "esp_tls_crypto.h"
#include "esp_log.h"
#include "webmin.h"

static httpd_handle_t webmin_server = NULL;

static const char *TAG = "webmin";

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

#define HTTPD_401      "401 UNAUTHORIZED"           /*!< HTTP Response 401 */

typedef struct {
    char    *username;
    char    *password;
} basic_auth_info_t;

static char *http_auth_basic(const char *username, const char *password)
{
    int out;
    char *user_info = NULL;
    char *digest = NULL;
    size_t n = 0;
    asprintf(&user_info, "%s:%s", username, password);
    esp_crypto_base64_encode(NULL, 0, &n, (const unsigned char *)user_info, strlen(user_info));
    digest = calloc(1, 6 + n + 1);
    strcpy(digest, "Basic ");
    esp_crypto_base64_encode((unsigned char *)digest + 6, n, (size_t *)&out, (const unsigned char *)user_info, strlen(user_info));
    free(user_info);
    return digest;
}

/* An HTTP GET handler */
static esp_err_t basic_auth_get_handler(httpd_req_t *req)
{
    char *buf = NULL;
    size_t buf_len = 0;
    basic_auth_info_t *basic_auth_info = req->user_ctx;

    buf_len = httpd_req_get_hdr_value_len(req, "Authorization") + 1;
    if (buf_len > 1) {
        buf = calloc(1, buf_len);
        if (httpd_req_get_hdr_value_str(req, "Authorization", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Authorization: %s", buf);
        } else {
            ESP_LOGE(TAG, "No auth value received");
        }

        char *auth_credentials = http_auth_basic(basic_auth_info->username, basic_auth_info->password);
        if (strncmp(auth_credentials, buf, buf_len)) {
            ESP_LOGE(TAG, "Not authenticated");
            httpd_resp_set_status(req, HTTPD_401);
            httpd_resp_set_type(req, "application/json");
            httpd_resp_set_hdr(req, "Connection", "keep-alive");
            httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Hello\"");
            httpd_resp_send(req, NULL, 0);
        } else {
            ESP_LOGI(TAG, "Authenticated!");
            char *basic_auth_resp = NULL;
            httpd_resp_set_status(req, HTTPD_200);
            //httpd_resp_set_type(req, "application/json");
            httpd_resp_set_hdr(req, "Connection", "keep-alive");
            //asprintf(&basic_auth_resp, "{\"authenticated\": true,\"user\": \"%s\"}", basic_auth_info->username);
            size_t content_length = index_html_end - index_html_start;
            httpd_resp_send(req, (char*)index_html_start, content_length);
            free(basic_auth_resp);
        }
        free(auth_credentials);
        free(buf);
    } else {
        ESP_LOGE(TAG, "No auth header received");
        httpd_resp_set_status(req, HTTPD_401);
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_hdr(req, "Connection", "keep-alive");
        httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Hello\"");
        httpd_resp_send(req, NULL, 0);
    }

    return ESP_OK;
}

static httpd_uri_t webmin_url_root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = basic_auth_get_handler,
    .user_ctx  = NULL
};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = (uint16_t)CONFIG_WEBMIN_HTTP_PORT; 
    config.ctrl_port = (uint16_t)CONFIG_WEBMIN_HTTP_PORT;
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        // httpd_register_uri_handler(server, &webmin_url_root);
        // httpd_register_uri_handler(server, &echo);
        // httpd_register_uri_handler(server, &ctrl);
        // httpd_register_uri_handler(server, &any);

        basic_auth_info_t *basic_auth_info = calloc(1, sizeof(basic_auth_info_t));
        basic_auth_info->username = CONFIG_WEBMIN_BASIC_USERNAME;
        basic_auth_info->password = CONFIG_WEBMIN_BASIC_PASSWORD;
        webmin_url_root.user_ctx = basic_auth_info;

        httpd_register_uri_handler(server, &webmin_url_root);

        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void webmin_start(void)
{
    webmin_server = start_webserver();
}
