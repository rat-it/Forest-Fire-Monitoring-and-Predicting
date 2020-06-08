#include <SPI.h>
#include <LoRa.h>
#include <dht.h>
#define dht_apin A0

char did = 'a';
dht DHT;

void setup(){
  Serial.begin(9600);
  while (!Serial);
  Serial.println("LoRa Sender");
  LoRa.setSPIFrequency(4E6);
  LoRa.setPins(10, 9, 2);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (!LoRa.begin(433E6))//;
    {     Serial.println("Trying...");    }
  }
  LoRa.setTxPower(20);
}
 
void loop(){
    DHT.read11(dht_apin);
    Serial.println("Sending packet...");
    
    LoRa.beginPacket();
    //LoRa.print("Current humidity = ");
    Serial.println(DHT.humidity);
    LoRa.print(did);
    LoRa.print(DHT.humidity);
    //LoRa.print(did+=1);
    //LoRa.print(did+=1);
    //LoRa.print("% \t");
    //LoRa.print("Current temperature = ");
    Serial.println(DHT.temperature);
    LoRa.print(DHT.temperature);
    LoRa.print(did);
    //LoRa.print("`C \n");
    LoRa.endPacket();
    delay(5000);
}
