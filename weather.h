#ifndef WEATHER_H
#define WEATHER_H

#include <stdlib.h>
#include <curl/curl.h>

// Structure to hold the response from the API
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Function declarations
void RAW_data(double lat, double lon);
void Process_data(double lat, double lon);

#endif // WEATHER_H
