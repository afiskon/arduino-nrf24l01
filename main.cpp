#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9, 10);

const int role_pin = A4;
const int led_pin = 2;
const int button_pin = led_pin;
const uint64_t addresses[] = { 0xAF1510009001LL, 0xAF1510009002LL };

bool isMaster = false;
uint8_t last_btn_state = HIGH;

void setup(void)
{
  Serial.begin(9600);

  pinMode(role_pin, INPUT);
  // enable internal pull-up resistor. keep this!
  digitalWrite(role_pin, HIGH);
  isMaster = (bool)digitalRead(role_pin);

  radio.begin();
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);

  if (isMaster)
  {
    radio.openWritingPipe(addresses[1]);
    // radio.openReadingPipe(1, addresses[0]); // not used yet

    pinMode(button_pin,INPUT);
  }
  else
  {
    // radio.openWritingPipe(addresses[0]); // not used yet
    radio.openReadingPipe(1, addresses[1]);

    radio.startListening();
    pinMode(led_pin,OUTPUT);
    digitalWrite(led_pin, HIGH);
  }
}

void loop(void)
{
  uint8_t state;

  delay(10);

  if (isMaster)
  {
    state = digitalRead(button_pin);

    if(state != last_btn_state)
    {
      Serial.println("Sending state: " + String(state));
      bool ok = radio.write( &state, sizeof(uint8_t) );
      if(ok) Serial.println("OK");
      else Serial.println("Failed!");
      last_btn_state = state;
    }
  }
  else
  {
    if(radio.available())
    {
      Serial.println("Data available, reading...");
      radio.read( &state, sizeof(state) );

      Serial.println("Received: " + String(state));
      digitalWrite(led_pin, state);
    }
  }
}
