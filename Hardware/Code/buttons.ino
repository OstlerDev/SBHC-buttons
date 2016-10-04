/***************************************************
  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Written by Skylar Ostler, based on original code by Tony DiCola 
  for Adafruit Industries and Richard Albritton.
 ****************************************************/
#include <ESP8266WiFi.h>
 
/****************************** Pins ******************************************/
 
#define Buttons       A0  // analog 0
#define Blue            5  // LED Color
#define Green            4  // LED Color
#define Red            2  // LED Color
 
/************************* WiFi Access Point *********************************/
 
#define WLAN_SSID       "ssid"
#define WLAN_PASS       "pass"
 
/************************** Website Setup ************************************/
 
#define HOST      "skylarostler.com"
#define PORT      80
#define UPDATE    "/sbhc/button_update.php"

/************************** Button Name **************************************/
 
#define NAME  "ticket_taker"
 
/************ Global State (you don't need to change this!) ******************/
 
// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
 
/*************************** Sketch Code ************************************/
 
// remote-buttons state
int ButtonRead = 0;
int current = 0;
int last = -1;

void setup() {

  pinMode(0, OUTPUT);
  pinMode(Blue, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Red, OUTPUT);
  // Turn off the light on Huzzah
  digitalWrite(0, LOW);

  digitalWrite(Red, HIGH);
  digitalWrite(Blue, HIGH);
  digitalWrite(Green, HIGH);
  
  Serial.print(F("Starting"));
  Serial.begin(115200);
 
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  delay(10);
  Serial.print(F("Connecting to "));
  Serial.println(WLAN_SSID);
 
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
 
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());
  digitalWrite(0, HIGH);
}
 
void loop() {  
    if (analogRead(Buttons) > 10){
      Serial.print("\n");
      Serial.print(analogRead(Buttons));
    }
    ButtonRead = analogRead(Buttons);
    delay(1);
    // grab the current state of the remote-buttons
    if (ButtonRead > 500 && ButtonRead < 700) {
      current = 3;
    }
    if (ButtonRead > 700 && ButtonRead < 1000) {
      current = 2;
    }
    if (ButtonRead > 1000) {
      current = 1;
    }
 
  // return if the value hasn't changed
  if(current == last)
    return;
    
  int32_t value = current;
 
  // Now we can publish stuff!
  Serial.print(F("\nSending RemoteButtons value: "));
  Serial.print(value);
  Serial.print("... ");

  // Turn on the light while processing
  digitalWrite(0, LOW);
  
  bool published = false;
  // Loop until it goes thru
  while (!published)
    if (publish(value)){
      published = true;
      // Turn off the light
      digitalWrite(0, HIGH);
    }
 
  // save the RemoteButtons state
  last = current;
 
}
 
// connect to adafruit io via MQTT
bool publish(int value) {
 
  Serial.print(F("Sending value to website... "));

  if (!client.connect(HOST, PORT)) {
    Serial.println("connection failed");
    return false;
  }

  Serial.println("Connected to Website");

  // Create URL
  String url = (String) UPDATE + "?name=" + (String) NAME + "&value=" + value;
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.0\r\n" +
               "Host: " + HOST + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");

  switch(value) {
    case 0: // Green
      digitalWrite(Red, HIGH);
      digitalWrite(Blue, HIGH);
      digitalWrite(Green, HIGH);
      break;
    case 1: // Green
      digitalWrite(Red, HIGH);
      digitalWrite(Blue, HIGH);
      digitalWrite(Green, LOW);
      break;
     case 2: // Yellow
      digitalWrite(Red, LOW);
      digitalWrite(Blue, HIGH);
      digitalWrite(Green, LOW);
      break;
     case 3: // Red
      digitalWrite(Red, LOW);
      digitalWrite(Blue, HIGH);
      digitalWrite(Green, HIGH);
      break;
  }

  return true;
}

