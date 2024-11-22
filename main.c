#include <stdio.h>
#include "weather.h"

int main() {
    double lat = 34.0522;  // Example latitude
    double lon = -118.2437; // Example longitude

    //Call RAW_Function
    RAW_data(lat, lon);


    //Call Process function
    Process_data(lat, lon);

    return 0;
}
