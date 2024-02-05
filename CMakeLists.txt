idf_component_register(
    SRCS "firestore.c" "rtdb.c"
    INCLUDE_DIRS .
    REQUIRES log freertos esp_http_client
)
