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

#include <SPI.h>
#include <Ethernet.h>


// GLOBAL VAR
int timeOfRequest = 0;
int timeOfStart = 0;
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

void setup() 
{
	// Interrupt Setup
	attachInterrupt(0, sens, RISING);

	// Open serial communications and wait for port to open:
	Serial.begin(250000);
	while (!Serial) 
	{
		; // wait for serial port to connect. Needed for native USB port only
	}

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

// Interrupt function
void sens()
{
	Serial.println("NEW SIGNAL !!!");
	sensorData = sensorData + 1;
}

void loop() 
{
	

	timeOfStart = millis();
	static long long T = millis();
	
	if ((millis() - T) > 4000)	
	{
		// if you get a connection, report back via serial:
		if (client.connect(server, 53210))
		{
			Serial.println("connected");
			// Make a HTTP request:
			//client.print("GET /search?q=");
			//client.print(sensorData);
			//client.println(" HTTP/1.1");
			//client.println("Host: www.google.com");
			//client.println("Connection: close");
			//client.println();	
			client.print(sensorData);
			sensorData = 0;
		}
		else
		{
			// if you didn't get a connection to the server:
			Serial.println("connection failed");
		}
		Serial.print("TIME REQUEST: ");
		timeOfRequest = millis() - timeOfStart;
		Serial.println(timeOfRequest);
		Serial.print("SENSOR DATA: ");
		Serial.println(sensorData);

		T = millis();
	}

	// if there are incoming bytes available
	// from the server, read them and print them:
	delay(2000);
	while (client.available())
	{
		char c = client.read();
		Serial.print(c);
	}
	Serial.println();
	if (client.connected())
	{
		client.stop();
	}

	// if the server's disconnected, stop the client:
}

void discon()
{
	if (!client.connected())
	{
		Serial.println();
		Serial.println("disconnecting.");
		client.stop();

		Serial.print("TIME PRINT: ");
		int timeOfPrint = millis() - timeOfStart - timeOfRequest;
		Serial.println(timeOfPrint);
		delay(3000);
		Serial.println("END");
		// do nothing forevermore:
		while (true);
	}
}