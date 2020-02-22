#include <SoftwareSerial.h>

#define DEBUG
#define PHONE_NUMBER "12345678"

static const char ON_MESSAGE[] = "Power On!";
static const char OFF_MESSAGE[] = "Power Off!";

// GSM module software serial
SoftwareSerial mySerial(2, 3); // RX, TX

// Power supply status
bool state = false;
bool prevState = state;

void setup()
{
    // Open serial communications to computer
    Serial.begin(9600);

    // Open serial communication to the GSM module
    mySerial.begin(115200);

#ifdef DEBUG
    // Get current baud rate
    mySerial.println("AT+IPR?");
    delay(100); // Let the command run
    while (mySerial.available()) {
        mySerial.read();
    }
#endif

    // Set baud rate
    mySerial.println("AT+IPR=115200");
    delay(100); // Let the command run
    while (mySerial.available()) {
        mySerial.read();
    }

    // Check if GSM module is ready
    mySerial.println("AT");
    while (mySerial.available()) {
        Serial.write(mySerial.read());
    }

    // The power supply PWR cable
    pinMode(A0, INPUT);

    // LED on Arduino to show whether the power is on or off
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
}

void loop()
{
#ifdef DEBUG
    while (Serial.available()) {
        mySerial.write(Serial.read());
    }

    while (mySerial.available()) {
        Serial.write(mySerial.read());
    }
#endif

    // Check power supply status
    prevState = state;
    state = analogRead(A0) > 0;

    // Send SMS when power supply status goes online or offline
    // It also turns on / off the LED on Arduino
    if (state != prevState) {
        if (state) {
            sendSMS(ON_MESSAGE);
            digitalWrite(13, HIGH);
        } else {
            sendSMS(OFF_MESSAGE);
            digitalWrite(13, LOW);
        }
    }
}

// sendSMS sends a text message to a phone number
static void sendSMS(const char *msg)
{
    // Set mode to text mode
    mySerial.println("AT+CMGF=1");
    delay(1000);

#ifdef DEBUG
    while (mySerial.available()) {
        Serial.write(mySerial.read());
    }
#endif

    // Query for carrier address
    mySerial.println("AT+CSCA?");
    delay(1000);

#ifdef DEBUG
    while (mySerial.available()) {
        Serial.write(mySerial.read());
    }
#endif

    // Set message validity to one day
    mySerial.println("AT+CSMP=17,167,0,16");
    delay(1000);

#ifdef DEBUG
    while (mySerial.available()) {
        Serial.write(mySerial.read());
    }
#endif

    // Set phone number
    mySerial.println("AT+CMGS=\"" PHONE_NUMBER "\"");
    delay(1000);

#ifdef DEBUG
    while (mySerial.available()) {
        Serial.write(mySerial.read());
    }
#endif

    // Set text message
    mySerial.print(msg);
    mySerial.write(26); // End of message
    mySerial.flush();
    delay(1000);

#ifdef DEBUG
    while (mySerial.available()) {
        Serial.write(mySerial.read());
    }
#endif
}
