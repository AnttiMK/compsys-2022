# Project Tamagotchi
## Table of contents

- [Preview](#preview)
- [Communication and SensorTag](#communication-and-sensortag)
    + [Communication with course backend was two options:](#communication-with-course-backend-was-two-options-)
    + [SensorTag functionalities:](#sensortag-functionalities-)
- [Basic idea behind Tamagotchi](#basic-idea-behind-tamagotchi)
    + [Tamagotchi have 3 key values:](#tamagotchi-have-3-key-values-)
    + [SensorTag and Tamagotchi UI:](#sensortag-and-tamagotchi-ui-)
      - [Tamagotchi UI show user key values, measurement from SensorTag data and two text box.](#tamagotchi-ui-show-user-key-values--measurement-from-sensortag-data-and-two-text-box)
      - [SensorTag also instructions and encourages user by Buzzer and two Led.](#sensortag-also-instructions-and-encourages-user-by-buzzer-and-two-led)
- [Functionalities](#functionalities)
    + [Full list of implemented functions:](#full-list-of-implemented-functions-)
- [Motion measurement and functions](#motion-measurement-and-functions)
      - [MPU9250 sensor calculates six key values:](#mpu9250-sensor-calculates-six-key-values-)
      - [In final recognizer we use only Acceleration X Y Z values and specific functions around it:](#in-final-recognizer-we-use-only-acceleration-x-y-z-values-and-specific-functions-around-it-)
      - [Recognized movements and actions](#recognized-movements-and-actions)
- [State machine](#state-machine)
- [Summary](#summary)
    + [Schedule and division of labor](#schedule-and-division-of-labor)
      - [Schedule](#schedule)
      - [Division of labor](#division-of-labor)
      - [Roles:](#roles-)

## Preview 

Our project was to create own virtual pet, in other words Tamagotchi by using TIDC-CC2650STK SensorTag. Tamagotchi located inside course backend server and our job was to communicate with that wireless and create all needed functionalities by using SensorTag. 
&nbsp;

<img src="https://www.ti.com/diagrams/tidc-cc2650stk-sensortag_1_main.gif" width="350" />
<img src="https://cdn.coolstuff.com/pub_images/original/18474_01.jpg?method=crop&type=auto&width=1200&height=630" width="350" />

&nbsp;

## Communication and SensorTag

### Communication with course backend was two options:

* Wireless by using ready-made course library.

* UART by using Node gateway. 

We implement both ways, but Wireless couldn't handle as many functions as we created.. So we used UART in our  final version.



### SensorTag functionalities:

Tamagotchi functions based mainly in data which is collected by SensorTag sensors. In project we use four sensors:

* TMP007: Temperature 

* OPT3001: Brightness

* BMP280: Air pressure

* MPU9250: Motion sensor 

And Three other SensorTag own functionality: 

* Buzzer 

* Battery 

* Buttons


## Basic idea behind Tamagotchi


![Image 29 11 2022 at 17 58](https://user-images.githubusercontent.com/73113013/204579318-1bca0626-3c16-43c9-bbba-4f93c160c34f.jpg)


### Tamagotchi have 3 key values: 

* Food
* Exercise 
* Pet

Those values are downgraded by 1 unit in every 10 seconds. User job is to keep those values above 0, otherwise Tamagotchi will run away. 

User can upgrade each values by individually or all in once doing certain interactive tasks, which are listed in own section.  

### SensorTag and Tamagotchi UI:


#### Tamagotchi UI show user key values, measurement from SensorTag data and two text box.

* Box 1: Shows battery status
* Box 2: Gives user instructions, encourages and verify identified actions


#### SensorTag also instructions and encourages user by Buzzer and two Led.

* Blink Led and warning sound, if some key value drop level 2
* Buzzer response for accepted actions

 
## Functionalities
### Full list of implemented functions:

* Plays music and give audios signals by using SensorTag Buzzer

* Communicate with backend in both ways Wireless + UART

* Receive data from Tamagotchi backend server

     * Process data
     * Execute different Tasks based received data

* Menu controlled by auxButton and pwrButton

     * Initial state options are 
        * pwrButton long press --> shutdown
        * auxButton short press --> feed Tamagotchi
        * auxButton long press --> Menu state

     * Menu state options are 
        * pwrButton press --> mitä tekee
        * auxButton press --> mitä tekee

* State machine which controls all functions

* Measures temperature from Sensor Tag TMP007 sensor

* Measures brightness from Sensor Tag OPT3001 sensor. If there is too dark Tamagotchi going sleep

* Measures air pressure from Sensor Tag BMP280 sensor

* Reading battery data and keep user updated its state

* Informing user by Led, Buzzer and UI messages

* Sends bellow data to Tamagotchi backend server
    * SensorTag raw data from TMP007 OPT3001 BMP280 MPU9250 sensors
    * Key value increase commands which based user actions
    * Text based information about recognized user actions and other interactive information

* Measures movements from Sensor Tag MPU9250 sensor

* Recognises movements and made actions based that

# Motion measurement and functions

Many of the Tamagotchi actions is based user movements. To recognize those movements we use raw data, which is received form SensorTag MPU9250 sensor.

#### MPU9250 sensor calculates six key values: 
     Acceleration in X-axis
     Acceleration in Y-axis
     Acceleration in Z-axis
     Gyroscope in X-axis
     Gyroscope in Y-axis
     Gyroscope in Z-axis

&nbsp;

<img src="https://miro.medium.com/max/1322/1*t83diIzYpPD7ObIGdSwnrQ.png" width="350" />

&nbsp;

To create movement recognizer we tested several solutions and collect a lot of raw data with certain movements and situations. 

First we collected data analyze it by using plots 

<img width="350" alt="Screenshot 2022-11-29 at 21 59 39" src="https://user-images.githubusercontent.com/73113013/204635838-f4e8a25d-019c-4c58-a2e9-fb785370d261.png">


From that data, we was able to define error limits and test several data process techniques. We tested to calculate Standard deviation and Variance for six values, which we received form movement sensor and used different size of data windows. 

From those calculations, we created couple different type of functions to recognize moves and ended up final solution, which worked best.


#### In final recognizer we use only Acceleration X Y Z values and specific functions around it: 
   * Floating average for each X Y Z value
   * Limit values to recognize users faulty movements
   * Variance calculus to process raw data
   * Main recognizer, which receives processed data and create Tasks from recognized movements


#### Recognized movements and actions
   * Stairs --> Exercise 
   * Something --> Pet
   * Something --> play music

# State machine

Draft and final version


<img width="338" alt="Screenshot 2022-11-29 at 22 29 15" src="https://user-images.githubusercontent.com/73113013/204641424-4a519dae-145e-4d08-87ae-260015b5cf1a.png">

## Summary

Project was fun and we was able to create all wanted functionalities. We planned precisely work steps and schedule before we started coding a project, which helped a lot.

During the project, we faced some small problems but we was able to solve those quickly. We would also have liked to create more accurate movement recognizer, but course schedule, didn't offer enough time for that.

### Schedule and division of labor

#### Schedule
  * Week 1: Data collection, creating needed Algorithms and planning State machine
  * Week 2: Main functionalities
  * Week 3: Main and additional functionalities
  * Week 4: Combining functionalities, fixing bugs and fine tuning

#### Division of labor

Division of labor was quite clear from the beginning, Antti has strong knowledge and experience about programming, so he was our main wizard. Antti was responsible of main code and combining smaller functions together. Matias and Joonas helped Antti and created functionalities  for example Movement and Brightness detection, Menu structure etc.

Coding was mainly handled face-to-face work sessions, where we was able to share ideas, find needed information and test code. 

#### Roles:
   * Antti: Main Wizard i.e. Main coding + Connecting pieces together
   * Matias: Coding + Searching needed information
   * Joonas: Coding + Documentation
