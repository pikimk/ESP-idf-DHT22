#ifndef DHT_22
#define DHT_22

#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

class DHT22 {

    private:
        gpio_num_t pin;
        float temperature;
        float humidity;
        uint8_t data[5];
    
    public:
        DHT22(gpio_num_t pin){
            this->pin = pin;
            data[0] = data[1] = data[2] = data[3] = data[4] = 0; // set all bits zero

            gpio_intr_disable(pin);
            gpio_pulldown_dis(pin);
            gpio_pullup_dis(pin);
        };

        int8_t readSensor(){
            
            gpio_pad_select_gpio(pin);
            gpio_set_direction(pin,GPIO_MODE_OUTPUT);
            gpio_set_level(pin, 1);

           
            esp_rom_delay_us(10000);

            //first pull low for 5ms
            gpio_set_level(pin,0);
            esp_rom_delay_us(3000);

            gpio_set_level(pin,1);
            esp_rom_delay_us(30);

            //set input mode
            gpio_set_direction(pin, GPIO_MODE_INPUT);
            esp_rom_delay_us(40);

            bool bitLow = gpio_get_level(pin);
            esp_rom_delay_us(80);

            bool bitHigh = gpio_get_level(pin);
            esp_rom_delay_us(40);
            
            int time50us = 0;
            int timeLowHigh = 0;
            
            if(bitHigh == true && bitLow == false){
                //sensor is read
                 for(int i = 0; i < 5; i++){

                    //bit number loop
                    for(int j = 7; j >= 0; j--){

                        // Count 50us
                        while (gpio_get_level(pin) == false){ 
                             time50us++;
                         }

                        // check timing for on/off state after the period of 50us
                        while (gpio_get_level(pin)  == true){
                            timeLowHigh++;
                        }

                         //if it is more then 50us, set bit high, else set bit low

                         if(time50us < timeLowHigh){
                             data[i] |= (1 << j);
                         }else{
                            data[i] &= ~(1 << j);
                         }
        
                        //reset timing
                         time50us = timeLowHigh = 0;

                    }

                }

                     uint8_t checksum = data[0] + data[1]+ data[2] + data[3];
                    if( checksum == data[4]){

                        int16_t tempData = 0; // signed 16 bit cause we expect negative temp
                        uint16_t humData = 0;
 

                        //Shifting 2 bytes into one ex. 01100100 and 00010011 into 0110010000010011 - humidity 
                        humData |= ((data[0] << 8) | data[1]);
                        //Shifting 2 bytes into one ex. 01100100 and 00010011 into 0110010000010011 - humidity 
                        tempData |= ((data[2] << 8) | data[3]);

                        //handle negative temp, the 15t bit is 1 if it is negative, other 14 show the values
                        if (tempData < 0){
                         // the 15th bit is 1 means negative value

                            tempData &= ~(1 << 15);
                             //set it like positive, and multiply with -1
                             tempData*= -1;
      
                        }

                         //we need to get decimal values since we have high and low byte ex. 239 / 10 = 23.9
                        float temp = (float)tempData / 10;
                        float hum = (float)humData / 10;

                        temperature = temp;
                        humidity = hum;

                    }else{
                        //checksumm error
                        temperature = NULL;
                        humidity = NULL;
                        return -2;
                    }
            }else{
                //sensor not read
                temperature = NULL;
                humidity = NULL;
                return -1;
            };
            
                return 1;
        };

        float getHumidity(){
            return humidity;
        };

        float getTemperature(){
            return temperature;
        };
   

};

    

#endif