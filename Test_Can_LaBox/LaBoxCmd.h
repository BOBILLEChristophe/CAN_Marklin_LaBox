/*

  LaBoxCmd.h

  Christophe Bobille - Locoduino

  The ESP32 requires to be connected to a CAN transceiver

*/

#ifndef __LA_BOX_CMD__
#define __LA_BOX_CMD__

#include "Arduino.h"
#include "CanMsg.h"

struct Loco
{
  uint32_t address;
  uint16_t speed;
  uint8_t direction;
  byte fn[28];
};

class LaBoxCmd
{
private:
    uint16_t mID;
    bool mDebug;
    bool mPower;
public:
  LaBoxCmd(uint16_t);
  ~LaBoxCmd();

  void setup();

  bool setPower(bool);
  bool setSpeed(Loco *);
  bool setDirection(Loco *);
  //bool setThrottle(Loco *);
  //bool toggleThrottleDir(Loco *);
  bool setFunction(Loco *, byte);
  bool toggleFunction(Loco *, byte);
  bool emergency();
  bool emergencyOne(Loco *);
};

#endif