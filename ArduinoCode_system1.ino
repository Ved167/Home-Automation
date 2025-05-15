#include <SoftwareSerial.h>

SoftwareSerial espSerial(10, 11); // RX, TX (Uno's 10 = RX from ESP, 11 = TX to ESP)

void setup() {
  Serial.begin(9600);        // For debugging on Serial Monitor
  espSerial.begin(9600);     // Communication with ESP8266

  //pinMode(2, INPUT);         // Flame sensor digital pin
}

void loop() {
  // Read and map moisture values
  int m3 = map(analogRead(A0), 1023, 0, 0, 100); // Moisture sensor 1
  int m4 = map(analogRead(A1), 1023, 0, 0, 100); // Moisture sensor 2

  // Send data to ESP in CSV format: m3,m4
  espSerial.print(m3); 
  espSerial.print(",");
  espSerial.println(m4); // Send new line for each data packet

  // Optional: Also print to Serial Monitor for debugging
  Serial.print("M3: "); Serial.print(m3); Serial.print("%, ");
  Serial.print("M4: "); Serial.print(m4); Serial.print("%, ");
  
  delay(2000); // Send data every 2 seconds
}
