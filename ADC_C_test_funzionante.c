/*
 * Compila con
 * gcc ADC_C_test.c ABElectronics_C_Libraries/ADCPi/ABE_ADCPi.c
 *     -I/home/pi/ABElectronics_C_Libraries/ADCPi/ -lmosquitto -lwiringPi
 */

#include <stdint.h>
#include <stdio.h>
#include "ABE_ADCPi.h"
#include <string.h>
#include <mosquitto.h>
#include <json-c/json.h>
#include <stdbool.h>
#include <wiringPi.h>

struct mosquitto *mosq = NULL;
char *topic = NULL;
const char* host = "localhost";

struct can_frame {
    unsigned short int id;
    unsigned int time;
    char data[8];
};

struct can_frame frame747;
struct can_frame frame748;
// struct can_frame frame749;

void send_Frame(struct can_frame frame)
{
	char json[100]; // da fare l'alloc della memoria ad hoc
        sprintf(json, "{\"id\":%u,\"time\":%u,\"data\":[", frame.id, frame.time);
        char tmp[10];
        int i = 0;
        for (i = 0; i < 8; i++) {
         	sprintf(tmp, "0x%02hhX,", frame.data[i]);
         	strcat(json, tmp);
        }

        strcat(json, "]}");
	// printf("SEND: %s\n", json); // PER DEBUG
        mosquitto_publish(mosq, NULL, "$SYS/raw", 100, &json, 0, false);
}

int main()
{
	mosquitto_lib_init();
	mosq = mosquitto_new(NULL, true, NULL);
	mosquitto_connect(mosq, host, 1883, 60);
	mosquitto_loop_start(mosq);

	while (1) {
		int acc1 = read_raw(0x69, 4, 14, 1, 1);//canale 8
		frame747.data[0] = acc1 >> 8;
		frame747.data[1] = acc1;
		int acc2 = read_raw(0x69, 3, 14, 1, 1);//canale7
		frame747.data[2] = acc2 >> 8;
		frame747.data[3] = acc2;
		int steering = read_raw(0x68, 2, 14, 1, 1);//canale2
		frame747.data[4] = steering >> 8;
		frame747.data[5] = steering;
		frame747.time = (unsigned int)millis();
		frame747.id = 747;
		send_Frame(frame747);

		int FLdamp = read_raw(0x69, 1, 14, 1, 1);//canale5
		frame748.data[0] = FLdamp >> 8;
		frame748.data[1] = FLdamp;
		int FRdamp = read_raw(0x69, 2, 14, 1, 1);//canale6
		frame748.data[2] = FRdamp >> 8;
		frame748.data[3] = FRdamp;
		int RLdamp = read_raw(0x68, 3, 14, 1, 1);//canale3
		frame748.data[4] = RLdamp >> 8;
		frame748.data[5] = RLdamp;
		int RRdamp = read_raw(0x68, 4, 14, 1, 1);//canale4
		frame748.data[6] = RRdamp >> 8;
		frame748.data[7] = RRdamp;
		frame748.time = (unsigned int)millis();
		frame748.id = 748;
		send_Frame(frame748);

		/*
		int current1 = read_raw(0x6A, 1, 14, 1, 1);
		int current2 = read_raw(0x6A, 2, 14, 1, 1);
		frame749.data[0] = current1 >> 8;
		frame749.data[1] = current1;
		frame749.data[2] = current2 >> 8;
		frame749.data[3] = current2;
		frame749.time = (uint32_t)millis();
		send_Frame(frame749);
		*/

		usleep(40000);
	}
}
