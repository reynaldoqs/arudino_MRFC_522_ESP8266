#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Configuracion firebase
#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""
// red y contrasenia wifi
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

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
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  //card reader start
  SPI.begin();
  mfrc522.PCD_Init(); 
  Serial.println("Control de acceso:");
}

 String firebasePath = "/users/";
 int passCost = 20;

void loop() {
  if ( mfrc522.PICC_IsNewCardPresent()){
      if ( mfrc522.PICC_ReadCardSerial()){
        Serial.println(""); 
        Serial.print("tag UID");
        String cardId = "";
        for (byte i = 0; i < mfrc522.uid.size; i++) {
               Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
               Serial.print(mfrc522.uid.uidByte[i], HEX);  
               cardId += mfrc522.uid.uidByte[i];
               //para parsear a hexa el id
               //cardId.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "_0" : "_"));
               //cardId.concat(String(mfrc522.uid.uidByte[i], HEX));
            }
                  //cardId.toUpperCase();
                  Serial.println(""); 
                  Serial.print("firebase id: ");
                  Serial.println(cardId);
                  String path = firebasePath+cardId;

                  String userName = "";
                  String userNameReaded = Firebase.getString(path+"/nombre");
                  if(Firebase.success()){
                    userName = userNameReaded;
                    }

                  int userBid = -1;
                  int userBidReaded = Firebase.getInt(path+"/credito");
                  if(Firebase.success()){
                    userBid = userBidReaded;
                   }
                  
                   Serial.println(userName);
                  //mejorar esta validacion
                  if(userName != "" && userBid != -1){          
                      
                    if((userBid - passCost) < 0){
           
                            Serial.print("Credito insuficiente: ");
                            Serial.print(userBid);
                            Serial.println(" Bs");
                            return;
                          }else{
                          Serial.println("success firebase get");
                          
                          Firebase.setInt(path+"/credito", (userBid-passCost));// descontando el costo del pase 
                          if(Firebase.failed()){
                            Serial.println("Error al descontar el monto");
                            return;
                           }
                          
                          Serial.print("Credito restante: ");
                          Serial.print(userBid-passCost);//actualizando solo para mostrar
                          Serial.println(" Bs");    
                          }
               
                    }else{
                      Serial.println("Tarjeta no registrada o error en la conexion");
                      }
                  
                  mfrc522.PICC_HaltA(); 
                  
        }
    }

}
