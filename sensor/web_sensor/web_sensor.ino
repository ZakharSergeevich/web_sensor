/*
 Name:		web_sensor.ino
 Created:	13.07.2017 13:16:09
 Author:	zakha
*/

// Connect to Microsoft Azure IOT



#include <SPI.h>
#include <Ethernet.h>

char hostname[] = "zakhar.azure-devices.net"; // имя узла Azure IoT Hub
char authSAS[] = "SharedAccessSignature sr=zakhar.azure-devices.net%2Fdevices%2Fsensor1&sig=92wNmX5ZzlEBMLHJgsrtWulnkJD3O6v3fkuJmMN5c2I%3D&se=1531236776";
String deviceName = "sensor1"; // ID нашего девайса
String uri = "/devices/sensor1/messages/events?api-version=2016-02-03";

// GLOBAL VAR
int timeOfRequest = 0;
int timeOfStart = 0;
int sensorData = 0;

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
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

void httpPost(String content)
{
	client.stop(); // закрываем подключение, если вдруг оно открыто
	if (client.connect(hostname, 443)) 
	{
		Serial.println("connect!!");
		client.print("POST ");
		client.print(uri);
		client.println(" HTTP/1.1");
		client.print("Host: ");
		client.println(hostname);
		client.print("Authorization: ");
		client.println(authSAS);
		client.println("Connection: close");
		client.print("Content-Type: ");
		client.println("text/plain");
		client.print("Content-Length: ");
		client.println(content.length());
		client.println();
		client.println(content);
		delay(200);
	}
	else 
	{
		Serial.println("HTTP POST отправка неудачна");
	}
}

void loop() 
{
	delay(1000);
	httpPost("Some message from Arduino");
	String response = "";
	char c;
	while (client.available()) 
	{
		c = client.read();
		response.concat(c);
	}
	if (!response.equals(""))
	{
		if (response.startsWith("HTTP/1.1 204")) 
		{
			Serial.println("Строка была отправлена в Azure");
		}
		else 
		{
			Serial.println("Ошибка");
			Serial.println(response);
		}
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