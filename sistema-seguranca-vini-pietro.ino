#include <Ultrasonic.h>

#include <Servo.h>

#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <deprecated.h>
#include <require_cpp11.h>

#define RST_PIN         9          // Pinos de conexão do leitor de cartão
#define SS_PIN          10

MFRC522 mfrc522(SS_PIN, RST_PIN); // Leitor de cartão

Ultrasonic sensor(3, 2);

int ledAlarme = 6;
int ledLibera = 7;

int buzzer = 8;
int tomAlarme = 261;
int tomLibera = 330;

Servo porta;
int anguloPortaFechada = 0;
int anguloPortaAberta = 90;

String idTagPrincipal = "3AA5987";     // id da tag (cartão) que consegue parar o alarme
String idTagLida = "";                  // id da tag sendo lida pelo leitor

unsigned long tempoLimiteParaAlarme = 10000;

bool dispararAlarme = true;

void setup() {
  // put your setup code here, to run once:
  SPI.begin();            // Inicialização do leitor de cartão
  mfrc522.PCD_Init();

  Serial.begin(9600);

  pinMode(ledAlarme, OUTPUT);
  pinMode(ledLibera, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  porta.attach(4);
  porta.write(anguloPortaFechada);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long tempoInicial = millis();
  if(sensor.read() <= 10){
    while(dispararAlarme){
      if(getID()){
        if(idTagLida == idTagPrincipal){
          Serial.println("ID identificado");
          dispararAlarme = false;
          break;
        }else{
          dispararAlarme = true;
          Serial.println("ID não identificado");
        }
      }
      if(millis() - tempoInicial >= tempoLimiteParaAlarme){
        tone(buzzer, tomAlarme);
        digitalWrite(ledAlarme, HIGH);
      }
      Serial.println(millis());
      delay(250);
      digitalWrite(ledAlarme, LOW);
      delay(250);
    }

    noTone(buzzer);
    delay(100);
    
    digitalWrite(ledAlarme, LOW);

    if(!dispararAlarme){
      tone(buzzer, tomLibera, 10000);
      digitalWrite(ledLibera, HIGH);
      porta.write(anguloPortaAberta);
    }

    delay(5000);
    digitalWrite(ledLibera, LOW);
    porta.write(anguloPortaFechada);
    noTone(buzzer);
    dispararAlarme = true;
  }
  delay(60);
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
  //Serial.println("-----------------------------------");
  //Serial.println("ID capturado: "+idTagLida);
  mfrc522.PICC_HaltA();
  return true;
}