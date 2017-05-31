# Raspberry PI Ohmmeter #

An Ohmmeter device using **Raspberry PI 2B** with **ADS1015** as ADC

### Setup ###

+ Items needed:
    * PC or Laptop
    * Raspberry Pi (I used model 2B)
    * ADS1015 12bit ADC converter (if you use 1115 which is 16bit, there will be slight changes)
        * Breadbord, wires, resistors
         * Adafruit assembled Pi T-Cobbler Plus - GPIO Breakout **(Optional)**
+ Circuit setup
    * Refer to fritzing diagrams in schematics folder

* Configuration
    + SSH, VNC or anyother method of communication enabled between the Raspberry PI and the user PC
        + My case was 
            * Laptop with Ubuntu 16.04 LTS
            * Raspberry PI 2B with Raspbian Jessie release __2017-01-11-raspbian-jessie.img__
            * SSH communication through ethernet cable

* Deployment instructions
    + As per the schematics folder
        + Connect Raspberry with Adafruit T-Cobbler
        + Connect T-Cobbler onto the breadboard
        + Connect ADS1015 onto the breadboard
        + Do the wirings as described in the schematics folder
        + In the circuit diagram I made a note about the location of the *UNKNOWN* resistor
### Information Sources ###

* ADS1015 Schematic (Very useful !)
    + https://cdn-shop.adafruit.com/datasheets/ads1015.pdf 
* dev-interface driver at kernel.org
    + https://www.kernel.org/doc/Documentation/i2c/dev-interface
* Voltage divider info
    + https://learn.sparkfun.com/tutorials/voltage-dividers
* An excellent tutorial on basic usage of ADS1115 (basicly same as ADS1015), will clarify a lot !
    + http://openlabtools.eng.cam.ac.uk/Resources/Datalog/RPi_ADS1115/    

### Built With ###
* VIM text editor
* GCC compiler

### License ###
This project is licensed under the MIT License

### Acknowledgment ###

* David Purdie the author of openlab tutorial http://openlabtools.eng.cam.ac.uk/Resources/Datalog/RPi_ADS1115/
