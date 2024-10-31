#include <HCSR04.h>

#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <deprecated.h>
#include <require_cpp11.h>

#define RST_PIN         9          // Pinos de conexão do leitor de cartão
#define SS_PIN          10

MFRC522 mfrc522(SS_PIN, RST_PIN); // Leitor de cartão

String idTagPrincipal = "3AA5987";     // id da tag (cartão) que consegue parar o alarme
String idTagLida = "";                  // id da tag sendo lida pelo leitor

void setup() {
  // put your setup code here, to run once:
  SPI.begin();            // Inicialização do leitor de cartão
  mfrc522.PCD_Init();

  Serial.begin(9600);
  Serial.println("Leitor pronto...");
}

void loop() {
  // put your main code here, to run repeatedly:
  while(getID()){
    if(idTagLida == idTagPrincipal){
      Serial.println("ID identificado, desligando alarme");
    }else{
      Serial.println("ID não identificado");
    }
    delay(100);
  }
}

boolean getID(){
  if(!mfrc522.PICC_IsNewCardPresent()){
    return false;
  }

  if(!mfrc522.PICC_ReadCardSerial()){
    return false;
  }

  idTagLida = "";
  for(uint8_t i = 0; i < 4; i++){
    idTagLida.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  idTagLida.toUpperCase();
  Serial.println("-----------------------------------");
  Serial.println("ID capturado: "+idTagLida);
  mfrc522.PICC_HaltA();
  return true;
}