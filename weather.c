#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <time.h>
#include "weather.h"

#define API_KEY "5e9557cf29e168ccaf0e58117a17735b"
#define BASE_URL "http://api.openweathermap.org/data/2.5/weather?appid=" API_KEY "&units=metric&lang=ur"

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

// Function to fetch raw weather  and write to a file
void RAW_data(double lat, double lon) {
    CURL *curl;
    CURLcode res;
    char url[256];
    struct MemoryStruct chunk = { .memory = malloc(1), .size = 0 };

    snprintf(url, sizeof(url), "%s&lat=%.4f&lon=%.4f", BASE_URL, lat, lon);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            FILE *file = fopen("raw_data.txt", "a");
            if (file != NULL) {
                fprintf(file, "%s\n", chunk.memory);
                fprintf(file, "--------------------------------\n");
                fclose(file);
            } else {
                printf("Error opening file for writing\n");
            }
        }
        curl_easy_cleanup(curl);
    }
    free(chunk.memory);
    curl_global_cleanup();
}

//Function to process weather data and write it to a file
void Process_data(double lat, double lon) {
    CURL *curl;
    CURLcode res;
    char url[256];
    struct MemoryStruct chunk = { .memory = malloc(1), .size = 0 };

    snprintf(url, sizeof(url), "%s&lat=%.4f&lon=%.4f", BASE_URL, lat, lon);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            cJSON *json = cJSON_Parse(chunk.memory);
            if (json == NULL) {
                printf("Error parsing JSON\n");
            } else {
                FILE *file = fopen("process_data.txt", "a");
                if (file != NULL) {
                    time_t t = time(NULL);
                    struct tm *tm_info = localtime(&t);
                    char time_buffer[30];
                    strftime(time_buffer, 30, "%Y-%m-%d %H:%M:%S", tm_info);

                    fprintf(file, "Execution Time: %s\n", time_buffer);

                    cJSON *main = cJSON_GetObjectItem(json, "main");
                    cJSON *weather = cJSON_GetArrayItem(cJSON_GetObjectItem(json, "weather"), 0);

                    if (main && weather) {
                        double maxtemp = cJSON_GetObjectItem(main, "temp_max")->valuedouble;
                        double mintemp = cJSON_GetObjectItem(main, "temp_min")->valuedouble;
                        double temperature = cJSON_GetObjectItem(main, "temp")->valuedouble;
                        int humidity = cJSON_GetObjectItem(main, "humidity")->valueint;
                        const char *description = cJSON_GetObjectItem(weather, "description")->valuestring;
                    
                        fprintf(file, "Karachi, Pakistan \n");
                        fprintf(file, "Current Temperature: %.2f°C\n", temperature);
                        fprintf(file, "Average Temperature: %.2f°C\n", (maxtemp+mintemp)/2);
                        fprintf(file, "Humidity: %d%%\n", humidity);
                        fprintf(file, "Weather: %s\n", description);
                        if (temperature > 30.0) {
                            fprintf(file, "Alert: Temperature exceeded 30°C!\n");
                            printf("Alert: Temperature has exceeded 30°C! Current Temperature: %.2f°C\n", temperature);  
                            system("notify-send 'Weather Alert' 'Temperature exceeded 30°C'");

                        
                        }
                        else if (temperature < 10.0) {
                            fprintf(file, "Alert: Temperature lower than 10°C!\n");
                            printf("Alert: Temperaturw lower than 10°C! Current Temperature: %.2f°C\n", temperature);  
                            system("notify-send 'Weather Alert' 'Temperature lower than 10°C'");

                        }
                        fprintf(file, "-------------------------------------------\n");
                    }
                    fclose(file);
                }
                cJSON_Delete(json);
            }
        }
        curl_easy_cleanup(curl);
    }
    free(chunk.memory);
    curl_global_cleanup();
}
