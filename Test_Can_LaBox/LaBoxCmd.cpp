
/*

  LaBoxCmd.cpp

*/

#include "CanMsg.h"
#include "LaBoxCmd.h"

static constexpr byte PRIO_NORMAL = 0x00;
static constexpr uint32_t BOX_ID = 0x1810;
static constexpr byte RESP_CMD = 0;

/*
Les commandes ci-dessous correspondent aux sections pages :

12 et 13 - SYSTEM_CONTROL
14 - CMD_EMERGENCY
30 - LOCO_SPEED
31 - LOCO_DIREC
32 - LOCO_FUNCT

de la documentation 
https://forum.3rails.fr/uploads/short-url/6cU7HyPkVmnfWErXjLvQgg5Poiz.pdf
*/
static constexpr byte SYSTEM_CONTROL = 0x00;
static constexpr byte CMD_EMERGENCY = 0x02;  // All loco
// Emergncy only one loco : Commande = SYSTEM_CONTROL (0x00) - data[0 -> 4] = add loco - data[4] = 0x03
// Page 15 de la documentation : https://forum.3rails.fr/uploads/short-url/6cU7HyPkVmnfWErXjLvQgg5Poiz.pdf
static constexpr byte LOCO_SPEED = 0x04;
static constexpr byte LOCO_DIREC = 0x05;
static constexpr byte LOCO_FUNCT = 0x06;

LaBoxCmd::LaBoxCmd(uint16_t id)
  : mID(id),
    mDebug(false),
    mPower(false) {
}

LaBoxCmd::~LaBoxCmd() {
  if (mDebug)
    Serial.println(F("### Destroying controller"));
}

void LaBoxCmd::setup() {
}

bool LaBoxCmd::setPower(bool power) {
  CanMsg::sendMsg(
    PRIO_NORMAL,
    SYSTEM_CONTROL,
    (byte)mID,
    BOX_ID,
    RESP_CMD,
    (byte)(0),
    (byte)(0),
    (byte)((BOX_ID & 0xFF00) >> 8),
    (byte)(BOX_ID & 0x00FF),
    (byte)(power ? 1 : 0)
    );

  mPower = power;
  return power;
}

// bool LaBoxCmd::setThrottle(Loco *loco) {
//   if (!loco) return false;
//   CanMsg::sendMsg(
//     PRIO_NORMAL,
//     LOCO_SPEED,
//     (byte)mID,
//     BOX_ID,
//     RESP_CMD,
//     (byte)(0),
//     (byte)(0),
//     (byte)((loco->address & 0xFF00) >> 8) , ,
//     (byte)(loco->address & 0x00FF),
//     (byte)(0),
//     (byte)(loco->speed & 0x00FF),
//     (byte)(loco->direction ? 1 : 0));

//   return true;
// }

bool LaBoxCmd::setSpeed(Loco *loco) {
  if (!loco) return false;
  CanMsg::sendMsg(
    PRIO_NORMAL,
    LOCO_SPEED,
    (byte)mID,
    BOX_ID,
    RESP_CMD,
    (byte)(0),
    (byte)(0),
    (byte)((loco->address & 0xFF00) >> 8),
    (byte)(loco->address & 0x00FF),
    (byte)((loco->speed & 0xFF00) >> 8),
    (byte)loco->speed & 0x00FF
    );

  return true;
}

// Signification du paramètre Direction : 0 = sens de marche reste
// 1 = sens de marche avant
// 2 = sens de marche arrière
// 3 = inverser le sens de marche

bool LaBoxCmd::setDirection(Loco *loco) {
  if (!loco) return false;
  CanMsg::sendMsg(
    PRIO_NORMAL,
    LOCO_DIREC,
    (byte)mID,
    BOX_ID,
    RESP_CMD,
    (byte)(0),
    (byte)(0),
    (byte)((loco->address & 0xFF00) >> 8),
    (byte)(loco->address & 0x00FF),
    (byte)(loco->direction)
    );

  return true;
}

// bool LaBoxCmd::toggleThrottleDir(Loco *loco) {
//   if (!loco) return false;

//   loco->direction = !loco->direction;
//   return setThrottle(loco);
// }

bool LaBoxCmd::setFunction(Loco *loco, byte idx) {
  if (!loco) return false;
  if (idx >= 28) return false;

  CanMsg::sendMsg(
    PRIO_NORMAL,
    LOCO_FUNCT,
    (byte)mID,
    BOX_ID,
    RESP_CMD,
    (byte)(0),
    (byte)(0),
    (byte)((loco->address & 0xFF00) >> 8),
    (byte)(loco->address & 0x00FF),
    idx,
    (byte)(loco->fn[idx] ? 1 : 0)
    );

  return true;
}

bool LaBoxCmd::toggleFunction(Loco *loco, byte idx) {
  if (!loco) return false;
  if (idx >= 28) return false;

  loco->fn[idx] = !loco->fn[idx];
  return setFunction(loco, idx);
}

bool LaBoxCmd::emergency() {
  CanMsg::sendMsg(
    PRIO_NORMAL,
    CMD_EMERGENCY,
    (byte)mID,
    BOX_ID,
    RESP_CMD
    );

  return true;
}

bool LaBoxCmd::emergencyOne(Loco *loco) {
  CanMsg::sendMsg(
    PRIO_NORMAL,
    SYSTEM_CONTROL,
    (byte)mID,
    BOX_ID,
    RESP_CMD,
    (byte)(0),
    (byte)(0),
    (byte)((loco->address & 0xFF00) >> 8),
    (byte)(loco->address & 0x00FF),
    0x03
    );
  return true;
}
