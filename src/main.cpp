#define DEBUG 1

#include <assert.h>
#include <Arduino.h>
#include <stm32f103xb.h>
#include <STM32F103_CanBus.h>


uint8_t counter = 0;
uint8_t frameLength = 0;
unsigned long previousMillis = 0;     // stores last time output was updated
const long interval = 1000;           // transmission interval (milliseconds)

/*
    Объявление сообщений CanBus
*/
CAN_msg_t CAN_TX_msg;
CAN_msg_t CAN_RX_msg;

/*
    Инициализация переферии
*/
void setup() 
{
  Serial2.begin(115200);
 
  //Инициализация Can шины
  bool rasult = CanBusInitialize(CAN_20KBPS, CANBUS_PINS_MAPPED_RX_PB8_TX_PB9);
}

/*
    Циклическая отправка сообщения
    в CanBus
*/
void CyclicSendCanBusMSG()
{

  CAN_TX_msg.data[0] = 0x00;
  CAN_TX_msg.data[1] = 0x01;
  CAN_TX_msg.data[2] = 0x02;
  CAN_TX_msg.data[3] = 0x03;
  CAN_TX_msg.data[4] = 0x04;
  CAN_TX_msg.data[5] = 0x05;
  CAN_TX_msg.data[6] = 0x06;
  CAN_TX_msg.data[7] = 0x07;
  CAN_TX_msg.len = frameLength;

  unsigned long currentMillis = millis();

  
  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;

    if ((counter % 2) == 0) 
    {
      CAN_TX_msg.type = DATA_FRAME; 

      if (CAN_TX_msg.len == 0) 
      {
        CAN_TX_msg.type = REMOTE_FRAME;
      }

      CAN_TX_msg.format = EXTENDED_FORMAT;
      CAN_TX_msg.id = 0x32F103;
    } 
    else 
    {
      CAN_TX_msg.type = DATA_FRAME;

      if (CAN_TX_msg.len == 0) 
      {
        CAN_TX_msg.type = REMOTE_FRAME;
      }

      CAN_TX_msg.format = STANDARD_FORMAT;
      CAN_TX_msg.id = 0x103;
    }

    CANSend(&CAN_TX_msg);
    frameLength++;

    if (frameLength == 9) 
    {
      frameLength = 0;
    }

    counter++;
  }

}

/*
    Бесконечный цикл
*/
void loop() 
{

  if(CanBusIsAvailable() == true) 
  {
    CanBusReadMsg(&CAN_RX_msg);

    if (CAN_RX_msg.format == EXTENDED_FORMAT) 
    {
      Serial2.print("Extended ID: 0x");
      if (CAN_RX_msg.id < 0x10000000) Serial2.print("0");
      if (CAN_RX_msg.id < 0x1000000) Serial2.print("00");
      if (CAN_RX_msg.id < 0x100000) Serial2.print("000");
      if (CAN_RX_msg.id < 0x10000) Serial2.print("0000");
      Serial2.print(CAN_RX_msg.id, HEX);
    } 
    else 
    {
      Serial2.print("Standard ID: 0x");
      if (CAN_RX_msg.id < 0x100) Serial2.print("0");
      if (CAN_RX_msg.id < 0x10) Serial2.print("00");
      Serial2.print(CAN_RX_msg.id, HEX);
      Serial2.print("     ");
    }

    Serial2.print(" DLC: ");
    Serial2.print(CAN_RX_msg.len);
    if (CAN_RX_msg.type == DATA_FRAME) 
    {
      Serial2.print(" Data: ");
      for(int i=0; i<CAN_RX_msg.len; i++) 
      {
        Serial2.print("0x"); 
        Serial2.print(CAN_RX_msg.data[i], HEX); 
        if (i != (CAN_RX_msg.len-1))  
        {
          Serial2.print(" ");
        }
      }
      Serial2.println();
    } 
    else 
    {
      Serial2.println(" Data: REMOTE REQUEST FRAME");
    }
  }
}