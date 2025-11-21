idf_component_register(SRCS "webmin.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES esp_http_server esp-tls
                    EMBED_TXTFILES index.html)
