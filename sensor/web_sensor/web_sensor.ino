/*
 Name:		web_sensor.ino
 Created:	13.07.2017 13:16:09
 Author:	zakha
*/

// Send sensors data to electro control center
// Use your own token from admin interface
// In admin interface add 2 sensors
//   external_id = 2_1 (temperature)
//   external_id = 2_2 (humidity)

/*
Web client

This sketch connects to a website (http://www.google.com)
using an Arduino Wiznet Ethernet shield.

Circuit:
* Ethernet shield attached to pins 10, 11, 12, 13

created 18 Dec 2009
by David A. Mellis
modified 9 Apr 2012
by Tom Igoe, based on work by Adrian McEwen

*/

#include <Ultrasonic.h>
#include <SPI.h>
#include <Ethernet.h>
#include <string.h>


// GLOBAL VAR
long long timeOfStart = 0;
static long long t_ultrasonic = 0;
static long long t_http_request = 0;
static int counter = 0;

int sensorData = 0;

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
IPAddress server(192,168,1,200);  // numeric IP for Google (no DNS)
//char server[] = "www.google.com";    // name address for Google (using DNS)

									 // Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

// Init Ultrasonic
Ultrasonic ultrasonic(8, 9);

void setup() 
{
	// Open serial communications and wait for port to open:
	Serial.begin(250000);

	// start the Ethernet connection:
	//if (Ethernet.begin(mac) == 0) 
	{
		Serial.println("Failed to configure Ethernet using DHCP");
		// try to congifure using IP address instead of DHCP:
		Ethernet.begin(mac, ip);
	}
	// give the Ethernet shield a second to initialize:
	delay(1000);
	Serial.println("connecting...");
}


void loop()
{
	timeOfStart = millis();	

	if ((millis() - t_ultrasonic) > 50)
	{
		t_ultrasonic = millis();

		sensorData = ultrasonic.distanceRead();

		static bool flag = 0;

		if (sensorData < 50)
		{
			if (flag == 0)
			{
				counter = counter + 1;
				sound();
				flag = 1;
				Serial.println(counter);
			}

		}
		else
		{
			flag = 0;
		}
	}

	// to server JSON data
	String dataString = "{\"sens\":\"" + String(counter) + "\"}";
	//Serial.print(dataString.length());
	//Serial.println(dataString);
	

	if ((millis() - t_http_request) > 10000)	
	{
		t_http_request = millis();
		
		// POST to SERVER
		postRequest(dataString);
		
		

		if (client.connected())
		{
			client.stop();
		}
	}

	// if there are incoming bytes available
	// from the server, read them and print them:
	while (client.available())
	{
		char c = client.read();
		Serial.print(c);
	}
}


void sound()
{
	tone(7, 1000, 50);
}

void postRequest(String dataString)
{
	client.setTimeout(100);

	if (client.connect(server, 2000))
	{
		Serial.println("connected");
		// Make a HTTP POST request:
		client.print("POST /sensor/");
		client.println(" HTTP/1.1");
		client.println("Host: 192.168.1.200");
		client.println("Content-Type: application/json");
		//client.println("User-Agent: arduino-ethernet");
		client.println("Connection: close");
		client.print("Content-Length: ");
		client.println(String(dataString.length()));	// number of bytes in the payload													
		client.println();	// important need an empty line here 
		client.println(dataString);// the payload

		counter = 0;
	}
	else
	{
		// if you didn't get a connection to the server:
		Serial.println("connection failed");
	}
}