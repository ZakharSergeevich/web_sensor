/*
 Name:		web_sensor.ino
 Created:	13.07.2017 13:16:09
 Author:	zakha
*/

// Connect to Microsoft Azure IOT



#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {
	0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
const char *server = "zakhar.servicebus.windows.net";
const char *sas = "SharedAccessSignature sr=https%3a%2f%2fzakhar.servicebus.windows.net%2fevhub1%2fpublishers%2f1%2fmessages&sig=8trW18kJ4S8sK5NLa%2fbmna7yHRMCYZEChO1%2bkPQcoZU%3d&se=1536144312&skn=send";
const char *serviceNamespace = "zakhar";
const char *hubName = "evhub1";
const char *deviceName = "1";
EthernetClient client;
char buffer[64];

void send_request(int value)
{
	Serial.println("\nconnecting...");
	if (client.connect(server, 443)) 
	{

		Serial.print("sending");
		Serial.println(value);

		// POST URI
		sprintf(buffer, "POST /eventhubtest1/publishers/%s/messages HTTP/1.1", deviceName);
		client.println(buffer);

		// Host header
		sprintf(buffer, "Host: %s", server);
		client.println(buffer);

		// Application key
		sprintf(buffer, "Authorization: %s", sas);
		client.println(buffer);

		// content type
		client.println("Content-Type: application/atom+xml;type=entry;charset=utf-8");

		// POST body
		sprintf(buffer, "{\"value\": %s}", "Hello World");

		// Content length
		client.print("Content-Length");
		client.println(strlen(buffer));

		// End of headers
		client.println();

		// Request body
		client.println(buffer);
	}
	else 
	{
		Serial.println("connection failed");
	}
}
/*
** Wait for response
*/
void wait_response()
{
	while (!client.available()) 
	{
		if (!client.connected()) 
		{
			return;
		}
	}
}


/*
** Read the response and dump to serial
*/
void read_response()
{
	bool print = true;
	while (client.available()) {
		char c = client.read();
		// Print only until the first carriage return
		if (c == '\n') 
		{
			print = false;
		}
		if (print)
			Serial.print(c);
	}
	if (!client.available()) 
	{
		char c = client.read();
		Serial.print(c);
	}
}


/*
** Close the connection
*/
void end_request()
{
	client.stop();
}
/*
** Arduino Setup
*/
void setup()
{
	Serial.begin(250000);
	while (!Serial) {
		; // wait for serial port to connect.
	}
	Serial.println("ethernet");
	if (Ethernet.begin(mac) == 0) {
		Serial.println("ethernet failed");
		for (;;);
	}
	// give the Ethernet shield a second to initialize:
	delay(1000);
}


/*
** Arduino Loop
*/
void loop()
{
	int val = analogRead(A0);
	send_request(val);
	wait_response();
	read_response();
	end_request();
	delay(1000);
}