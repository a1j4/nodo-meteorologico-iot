#ifndef CONFIG_H
#define CONFIG_H
//CONFIGURACION GENERAL VA ANTES
 #define SENSOR_READ_INTERVAL 2000 //milidegundos
 //definir el numero de pines del ESP32 
 #define DHT_PIN 8 //cambiar este número
 //tipo de sensor 
 #define SENSOR_TYPE_DHT 1
#define WAKE_INTERVAL 60//SEGUNDOS

 //configuracón del muestreo al momento de estabilizarse
 #define SENSOR_STABILIZATION_TIME 2000 // define la estabilisacion del sensor en 2 segundos
 #define SAMPLE_COUNT 5 // define el número de muestra durarte la estabilizacion del sensor
 #define SAMPLE_INTERVAL 500 //define el mili segundos entre muestras tomadas
 #endif

