/*
 * Practice: talking with ADC through I2C using dev/i2c.h
 * NOTE: In the schematic, it is said that the communication is through 4 bytes as follows:
 * 	1) Byte 1: Slave select, contains 7bit slave select address and 1bit for read/write flag
 * 	2) Byte 2: Pointer register byte (00 conversion, 01 config)
 * 	3) Byte 3 & 4: config register bytes
 * more info regarding ADS1015 ADC device can be found in: https://cdn-shop.adafruit.com/datasheets/ads1015.pdf specially 
 * from page 13 onwards
 *
 * But we do not need the first Byte as it is already covered by using write() and read() functions
 * */

#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <inttypes.h>
#include <math.h>

#define R1_10K 	10000 //10K resistance
#define R1_100K 100000 //20K resistance
#define R1_1M	1000000 //30K resistance
#define V_IN 3.29 //Raspberry PI voltage
#define ADDR 0x48 //This is the ADDR of ADS1015 according to i2cdetect, according to ADS1015 schematic page 14 Table 5, if
		  //the ADDR pin is connected to GND, SLAVE address is 1001000 which is 0x48



int main(){

	//Variables declarations
	int file; //file handler
	int adapter_nr = 1; //The i2c adapter we will talk to, in our case its i2c-1
	char filename[20]; //The string containing the device name (/dev/i2c-1)
	int16_t result; // store the 16bit representation of the data read
	float voltage, resistance = 0.0;
	int read_check, write_check = 0; //error checking variables
	uint8_t write_buf[3]; //The writing buffer used to write the 3 bytes to the device, byte1: pointer register
			      //byte2: bits[15-8] of the config register, byte3: bits[7-0] of the config register
	uint8_t read_buf[2];  //The reading buffer used to read from the ADS data register
	float bit_v = 6.144 / 2048; //Converter from binary to base10 decimal with respect to 
				    //the chosen gain amplifier (PGA = 6.144V)




	//Adding the adapter number to the device name, this step can be minimized by just writing a string "/dev/i2c-1/"
	//in the open function below replacing the filename array -- but I used it here for practice as it was used in
	//the Kernel.org dev-interface example https://www.kernel.org/doc/Documentation/i2c/dev-interface
	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
	file = open(filename, O_RDWR);//opening for reading and writing
	if(file < 0){
		printf("Error #1: error when opening file\n");
		exit(1);
	}

	
	

	//Using ioctl - control device function http://man7.org/linux/man-pages/man2/ioctl.2.html
	//to specify with which address in the device we are going to communicate to
	int ioc;
	ioc = ioctl(file, I2C_SLAVE, ADDR);
	if(ioc < 0){
		printf("Error #2: error when using ioctl\n");
		exit(1);
	}

	//To do a single-shot conversion we send the configuration bytes 11010001 00000011 to config register at address 00000001
	//Tables 6, 7, 8, and 9 explain these bits
	write_buf[0] = 1;    // 00000001 points to the config register
	write_buf[1] = 0xD1; // writing 11010001 in second byte (tried 000 PGA for 6.144V gain)
	write_buf[2] = 0x03;  // writing 00000011 in third byte

	read_buf[0] = 0; // initialize the reading buffer
	read_buf[1] = 0;

	//Starting the conversation:
	//Writing to ADS1015
	write_check = write(file, write_buf, 3); //write to file the write_buf content, and write 3 bytes
	if(write_check != 3){
		printf("Error #3: error when writing to device\n");
		exit(1);
	}

	//Now we keep checking the OS bit (bit 15) which tells us if the device is doing a conversion or not
	//In reading mode, 0 = device is performing conversion, 1 =  device not performing conversion
	while((read_buf[0] & 0x80) == 0){ // ANDing with 0x80 (10000000) to check bit 15
		read_check = read(file, read_buf, 2); //Keep checking until bit 15 changes to 1, meanining conversion stopped
		if(read_check != 2){
			printf("Error #4: error while reading data\n");
			exit(1);
		}
	}	

	//Now that the device finished conversion, we can start reading the data from ADS1015
	//First change the pointer register to 0 which is the address of the conversion register, to read from it
	write_buf[0] = 0;
	write_check = write(file, write_buf, 1);
	if(write_check != 1){
		printf("Error#5: error while writing to pointer register\n");
		exit(1);
	}
	//Now we read the data that was converted
	read_check = read(file, read_buf, 2); //read 2 bytes from file(conv register) into read_buf
	if(read_check != 2){
		printf("Error#6: error while reading the converted data\n");
		exit(1);
	}
	
	//Combine the 2bytes into a 16bit integer
	result = read_buf[0] << 8 | read_buf[1]; //shift first byte by 8 bits and OR the second byte with the rest
	
	result = result >> 4; //It was mentioned  in a driver that ADS1015 should be also shifted 4 bytes as it is 12bits
	if (result > 0x07FF) {
            // negative number - extend the sign to 16th bit
            result |= 0xF000;
        }
		
	//Printing Raw data for inspection
//	printf("The Raw Data in HEX is: %x\n\n", result);

	//Print the result to terminal
//	printf("Voltage Reading %f (V) \n", (float)result*bit_v); 


	///////////////////////////////////////////////////////////
	//saving the voltage in V format
	voltage = (float) result * bit_v;

	//Calculating R2 for 10K resistnce
	resistance = R1_10K * (1 / ((V_IN / voltage) - 1));

	printf("\n================================Analysis==========================================\n\n");
	printf("|\tVin\t|\tVout\t|\tR1\t\t|\tR2 Result\t|\n\n");
	printf("|\t3.3 V\t|\t%.2f V\t|\t%d ohms\t|\t%.2f ohms\t|\n",voltage, R1_10K, resistance);

	///////////////////////////////////////////////////////////



	//close the connection
	close(file);




	printf("\n\nEnd of Program\n");
	return 0;
}
