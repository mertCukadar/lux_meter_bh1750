#include "data_handler.h"


static measurements_t current_measurements = {0};

measurements_t* data_handler_get(void) {
    return &current_measurements;
}