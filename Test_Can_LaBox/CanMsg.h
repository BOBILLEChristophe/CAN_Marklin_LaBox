#ifndef __CAN_MSG__
#define __CAN_MSG__

#include <Arduino.h>
#include <ACAN_ESP32.h>

class CanMsg
{
private:
  static gpio_num_t RxPin;
  static gpio_num_t TxPin;
  static uint32_t DESIRED_BIT_RATE;

public:
  CanMsg() = delete;

  static void setup();
  static void canReceiveMsg(void *);
  static void sendMsg(CANMessage &);

  static void sendMsg(byte, byte, byte, byte, byte);
  static void sendMsg(byte, byte, byte, byte, byte, byte);
  static void sendMsg(byte, byte, byte, byte, byte, byte, byte);
  static void sendMsg(byte, byte, byte, byte, byte, byte, byte, byte);
  static void sendMsg(byte, byte, byte, byte, byte, byte, byte, byte, byte);
  static void sendMsg(byte, byte, byte, byte, byte, byte, byte, byte, byte, byte);
  static void sendMsg(byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte);
  static void sendMsg(byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte);
  static void sendMsg(byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte);
  static void sendMsg(byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte, byte);
};

#endif