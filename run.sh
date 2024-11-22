
#!/bin/bash

while true; do
    echo "Compiling weather_app..."
    gcc main.c weather.c -o weather_app -lcurl -lcjson

    if [ $? -eq 0 ]; then
        echo "Compilation successful! Running weather_app..."
        ./weather_app
    else
        echo "Compilation failed! Check your code. :("
        exit 1
    fi
    echo "Your data has been saved. :)"
    sleep 600  
done

