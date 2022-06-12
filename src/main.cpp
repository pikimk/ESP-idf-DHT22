#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "DHT22.hpp"

 void temperatureSensor(void* params){

         DHT22 sensor(GPIO_NUM_16); //esp32 gpio16

         while(1){
            if(sensor.readSensor() > 0){ // if it reads 1, is ok, -1 - sensor not read, -2 checksum error
                ESP_LOGW("SNS", "Temperature: %.1f Humidity: %.1f", sensor.getTemperature(), sensor.getHumidity());
            }else{ 
                ESP_LOGW("SNS","Sensor error!");
            }; 

             vTaskDelay(1000/ portTICK_PERIOD_MS); // delay for sensor to reset, at least 400ms
            
         }
        
    }

    void tempStatic(void* param){
        //use static method from class to read specific pin
        float temp;
        float hum;
        while(1){
             if(DHT22::getDataFromPin(GPIO_NUM_17,temp,hum) > 0){
            //read succsess
            ESP_LOGW("SNS","Temp: %.1f Hum: %.1f ",temp,hum);
        }else{
            ESP_LOGW("SNS","Static sensor error");
        };
        vTaskDelay(500/ portTICK_PERIOD_MS);
        }
       
    }


extern "C"{

    void app_main() {

        xTaskCreate(&temperatureSensor,"TEMP_TASK", 2048 ,NULL,5,NULL);
        xTaskCreate(&tempStatic,"STATIC-METHOD",2048,NULL,5,NULL);
        

    }

}
