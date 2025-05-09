#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "eecs388_lib.h"


void auto_brake(int devid)
{
    // Task-1: 
    // Your code here (Use Lab 02 - Lab 04 for reference)
    // Use the directions given in the project document
    uint16_t dist;
    // read an input from lidar sensor and turn on corresponding led
        if ('Y' == ser_read(0) && 'Y' == ser_read(0)) {
            
            uint8_t dist_L = ser_read(0);
            uint8_t dist_H = ser_read(0);

            dist = (dist_H << 8) | dist_L;
 
            if (dist > 200) {
                gpio_write(RED_LED, OFF);
                gpio_write(GREEN_LED, ON);
            }

            else if (100 < dist && dist <= 200) {
                gpio_write(RED_LED, ON);
                gpio_write(GREEN_LED, ON);
            }
            else if (60 < dist && dist <= 100) {
                gpio_write(GREEN_LED, OFF);
                gpio_write(RED_LED, ON);
            }
            else {
                gpio_write(GREEN_LED, OFF);
                gpio_write(RED_LED, ON);
                delay(50);
                gpio_write(RED_LED, OFF);
                delay(50);
            }
    }
}
int read_from_pi(int devid)
{
    // Task-2: 
    // You code goes here (Use Lab 09 for reference)
    // After performing Task-2 at dnn.py code, modify this part to read angle values from Raspberry Pi.

    int recievedAngle;
    char inputString[10];

    if (ser_isready(devid)) {
        ser_readline(devid, 10, inputString);
        sscanf(inputString, "%d", &recievedAngle);
        return recievedAngle;
    }

    return 0;
}
void steering(int gpio, int pos)
{
    // Task-3: 
    // Your code goes here (Use Lab 05 for reference)
    // Check the project document to understand the task

    #define SERVO_PULSE_MAX 2400
    #define SERVO_PULSE_MIN 544
    #define SERVO_PERIOD 20000

    float pulseRate = (SERVO_PULSE_MAX - SERVO_PULSE_MIN)/180;
    float pulsePeriod = (pulseRate * pos) + SERVO_PULSE_MIN;
    
    gpio_write(gpio, ON);
    delay_usec(pulsePeriod);
    gpio_write(gpio, OFF);
    delay_usec(SERVO_PERIOD - pulsePeriod);
}


int main()
{
    // initialize UART channels
    ser_setup(0); // uart0
    ser_setup(1); // uart1
    int pi_to_hifive = 1; //The connection with Pi uses uart 1
    int lidar_to_hifive = 0; //the lidar uses uart 0
    
    printf("\nUsing UART %d for Pi -> HiFive", pi_to_hifive);
    printf("\nUsing UART %d for Lidar -> HiFive", lidar_to_hifive);
    
    //Initializing PINs
    gpio_mode(PIN_19, OUTPUT);
    gpio_mode(RED_LED, OUTPUT);
    gpio_mode(BLUE_LED, OUTPUT);
    gpio_mode(GREEN_LED, OUTPUT);

    printf("Setup completed.\n");
    printf("Begin the main loop.\n");

    while (1) {

        auto_brake(lidar_to_hifive); // measuring distance using lidar and braking
        int angle = read_from_pi(pi_to_hifive); //getting turn direction from pi
        //printf("\nangle=%d", angle) 

        int gpio = PIN_19; 
        for (int i = 0; i < 5; i++){
            // Here, we set the angle to 180 if the prediction from the DNN is a positive angle
            // and 0 if the prediction is a negative angle.
            // This is so that it is easier to see the movement of the servo.
            // You are welcome to pass the angle values directly to the steering function.
            // If the servo function is written correctly, it should still work,
            // only the movements of the servo will be more subtle
            if(angle>0){
                steering(gpio, angle);
            }
            else {
                steering(gpio,0);
            }
            
            // Uncomment the line below to see the actual angles on the servo.
            // Remember to comment out the if-else statement above!
            //steering(gpio, angle);
        }

    }
    return 0;
}
