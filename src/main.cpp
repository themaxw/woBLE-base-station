#include <Arduino.h>
#include <Message.h>

#include <ESP8266WiFi.h>
#include <espnow.h>

#include <Adafruit_DS3502.h>
#include <Wire.h>


Adafruit_DS3502 ds3502 = Adafruit_DS3502();

Message message;
TwoWire wire;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
    if(len != sizeof(message)){
        Serial.println("invalid message size");
        return;
    }

	memcpy(&message, incomingData, sizeof(message));

    uint wiper =  message.rot_x / M_PI_2 * 127;
    if (message.rot_x < 0) wiper = 0;
    else if (wiper > 127) wiper = 127;
    ds3502.setWiper(wiper);

    Serial.print("Bytes received: ");
	Serial.println(len);
	Serial.print(">x:");
	Serial.println(message.rot_x);
	Serial.print(">y:");
	Serial.println(message.rot_y);
	Serial.print(">z:");
	Serial.println(message.rot_z);
	Serial.println();
}

void setup() {
	

	Serial.begin(115200);
	Serial.println("Starting Init");

	// Set device as a Wi-Fi Station
	WiFi.mode(WIFI_STA);


	// Init ESP-NOW
	if (esp_now_init() != 0) {
		Serial.println("Error initializing ESP-NOW");
		return;
	}
	
    // SDA: PIN D2, SCL: PIN D1
	wire.begin(PIN_WIRE_SDA, PIN_WIRE_SCL);
	ds3502.begin((uint8_t) 0x28, &wire);
    if (!ds3502.begin()) {
		Serial.println("Couldn't find DS3502 chip");
		while (1);
	}
	Serial.println("Initialized");
    ds3502.setWiper(63);

	// Once ESPNow is successfully Init, we will register for recv CB to
	// get recv packer info
	esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
	esp_now_register_recv_cb(OnDataRecv);

}

void loop() {
}