#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// red y contrasenia wifi
#define WIFI_SSID "AMD2"
#define WIFI_PASSWORD "1989REY"

//pins MRF22
#define SS_PIN D4
#define RST_PIN D2

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  //card reader start
  SPI.begin();
  mfrc522.PCD_Init(); 
  Serial.println("Control de acceso:");
  
}

 String service_url = "http://192.168.0.2:3000/socket_desc"; //ip host, it would change on every instance


void loop() {

    if ( mfrc522.PICC_IsNewCardPresent()){
        if ( mfrc522.PICC_ReadCardSerial()){
          Serial.println("-------------------------------------------------"); 
          String cardId = "";
          for (byte i = 0; i < mfrc522.uid.size; i++) {
                 Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                 Serial.print(mfrc522.uid.uidByte[i], HEX);  
                 cardId += mfrc522.uid.uidByte[i];
              }
                    Serial.println(""); 
                    Serial.print("idTarjeta: ");
                    Serial.println(cardId);
                    
                    StaticJsonDocument<100> testDocument;
                    testDocument["terminalMac"] = "fr:fr:54:r4";
                    testDocument["idTarjeta"] = cardId;
                    
                    char buffer[100];
                    serializeJson(testDocument, buffer);
                  
                    HTTPClient http; 
                    http.begin(service_url); 
                    http.addHeader("Content-Type", "application/json");  //Specify content-type header
   
                    int httpCode = http.POST(buffer);
                    String payload = http.getString();   
  
                    if(httpCode >= 200 && httpCode < 300){
                        Serial.println("Everithing is ok");
                        Serial.println(payload);
                      }else{
                        Serial.println("Something has been fallen");
                        Serial.println(payload);
                        }
                     
                    
                    http.end();
                    
                    mfrc522.PICC_HaltA(); 
                    
          }
      }
 

}
