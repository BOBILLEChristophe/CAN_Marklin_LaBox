/*

  CanMsg.cpp

*/

#include "CanMsg.h"

gpio_num_t CanMsg::RxPin = GPIO_NUM_22;               // Optional, default Rx pin is GPIO_NUM_4
gpio_num_t CanMsg::TxPin = GPIO_NUM_23;               // Optional, default Tx pin is GPIO_NUM_5
uint32_t CanMsg::DESIRED_BIT_RATE = 1000UL * 1000UL;  // 250 Kb/s;

CANMessage frameIn;

void CanMsg::setup() {
  Serial.printf("[CanMsg %d] : Configure ESP32 CAN\n", __LINE__);
  ACAN_ESP32_Settings settings(DESIRED_BIT_RATE);
  settings.mRxPin = RxPin;
  settings.mTxPin = TxPin;

  uint32_t errorCode;

  // without filter
  errorCode = ACAN_ESP32::can.begin(settings);
  Serial.printf("[CanMsg %d] : config without filter\n", __LINE__);

  if (errorCode == 0)
    Serial.printf("[CanMsg %d] : configuration OK !\n", __LINE__);
  else {
    Serial.printf("[CanMsg %d] : configuration error 0x%x\n", __LINE__, errorCode);
    return;
  }
  xTaskCreatePinnedToCore(canReceiveMsg, "CanReceiveMsg", 2 * 1024, NULL, 5, NULL, 0);
}

// Lecture des informations en retour de LaBox
void CanMsg::canReceiveMsg(void *pvParameter) {
  while (1) {
    CANMessage frameIn;
    if (ACAN_ESP32::can.receive(frameIn)) {
      const uint8_t cmde = ((frameIn.id & 0x1FE0000) >> 17);  // Commande
      const uint16_t exped = (frameIn.id & 0xFFFF);           // Expéditeur
      const uint8_t resp = ((frameIn.id & 0x10000) >> 16);    // Commande = 0 / Reponse = 1

      //Serial.println(cmde, HEX);

      if (exped == 0x1810)  // Il s'agit d'un envoi de commande de la Box
      {
        Serial.printf("<- Recu de 0x%0X : Commande 0x%0X\n", exped, cmde);
        if (frameIn.rtr)  // Remote frame
          ACAN_ESP32::can.tryToSend(frameIn);
        else {
          switch (cmde) {
            case 0x04:
              if (resp) {
                Serial.printf("   Confirmation commande de vitesse : Loco %d - Vitesse %d\n",
                              (frameIn.data[0] << 24) + (frameIn.data[1] << 16) + (frameIn.data[2] << 8) + frameIn.data[3],
                              (frameIn.data[4] << 8) + frameIn.data[5]);
              }
              break;
            case 0x05:
              if (resp) {
                Serial.printf("   Confirmation commande de direction : Loco %d - Direction %d\n",
                              (frameIn.data[0] << 24) + (frameIn.data[1] << 16) + (frameIn.data[2] << 8) + frameIn.data[3],
                              frameIn.data[4]);
              }
              break;
            case 0x0C:
              if (frameIn.data[0] == 2)  // OVERLOAD
                Serial.printf("   Power overload.\n");
              else {
                Serial.printf("   Power %s\n", frameIn.data[0] ? "on" : "off");
                Serial.printf("   Mesure de courant : %d\n", (frameIn.data[1] << 8) + frameIn.data[2]);
              }
              break;
            case 0x00:
              if (resp) {
                switch (frameIn.data[4]) {
                  case (0x00):
                  case (0x01):
                    Serial.printf("   Confirmation power : %s\n", frameIn.data[4] ? "<1>" : "<0>");
                    break;
                  case (0x02):
                    Serial.printf("   Confirmation emergency - Loco %d\n",
                                  (frameIn.data[0] << 24) + (frameIn.data[1] << 16) + (frameIn.data[2] << 8) + frameIn.data[3]);
                    break;
                }
              }
              break;
            case 0x02:
              if (resp) {
                Serial.printf("   Confirmation pour emergency stop\n");
              }
              break;
          }
        }
      }
    }
    vTaskDelay(100 / portTICK_RATE_MS);
  }
}

/*--------------------------------------
  Envoi CAN
  --------------------------------------*/

void CanMsg::sendMsg(CANMessage &frame) {
  if (0 == ACAN_ESP32::can.tryToSend(frame))
    Serial.printf("Echec envoi message CAN\n");
}

auto formatMsg = [](CANMessage &frame, byte prio, byte cmde, byte thisNodeId, byte destNodeId, byte resp) -> CANMessage {
  frame.id |= prio << 25;  // Priorite 0, 1, 2 ou 3
  frame.id |= cmde << 17;  // commande appelée
  frame.id |= resp << 16;  // Réponse
  frame.id |= thisNodeId;  // ID expediteur
  frame.ext = true;
  return frame;
};

void CanMsg::sendMsg(byte prio, byte cmde, byte thisNodeId, byte destNodeId, byte resp) {
  CANMessage frame;
  frame = formatMsg(frame, prio, cmde, thisNodeId, destNodeId, resp);
  frame.len = 0;
  CanMsg::sendMsg(frame);
}

void CanMsg::sendMsg(byte prio, byte cmde, byte thisNodeId, byte destNodeId, byte resp, byte data0) {
  CANMessage frame;
  frame = formatMsg(frame, prio, cmde, thisNodeId, destNodeId, resp);
  frame.len = 1;
  frame.data[0] = data0;
  CanMsg::sendMsg(frame);
}

void CanMsg::sendMsg(byte prio, byte cmde, byte thisNodeId, byte destNodeId, byte resp, byte data0, byte data1) {
  CANMessage frame;
  frame = formatMsg(frame, prio, cmde, thisNodeId, destNodeId, resp);
  frame.len = 2;
  frame.data[0] = data0;
  frame.data[1] = data1;
  CanMsg::sendMsg(frame);
}
void CanMsg::sendMsg(byte prio, byte cmde, byte thisNodeId, byte destNodeId, byte resp, byte data0, byte data1, byte data2) {
  CANMessage frame;
  frame = formatMsg(frame, prio, cmde, thisNodeId, destNodeId, resp);
  frame.len = 3;
  frame.data[0] = data0;
  frame.data[1] = data1;
  frame.data[2] = data2;
  CanMsg::sendMsg(frame);
}
void CanMsg::sendMsg(byte prio, byte cmde, byte thisNodeId, byte destNodeId, byte resp, byte data0, byte data1, byte data2, byte data3) {
  CANMessage frame;
  frame = formatMsg(frame, prio, cmde, thisNodeId, destNodeId, resp);
  frame.len = 4;
  frame.data[0] = data0;
  frame.data[1] = data1;
  frame.data[2] = data2;
  frame.data[3] = data3;
  CanMsg::sendMsg(frame);
}

void CanMsg::sendMsg(byte prio, byte cmde, byte thisNodeId, byte destNodeId, byte resp, byte data0, byte data1, byte data2, byte data3, byte data4) {
  CANMessage frame;
  frame = formatMsg(frame, prio, cmde, thisNodeId, destNodeId, resp);
  frame.len = 5;
  frame.data[0] = data0;
  frame.data[1] = data1;
  frame.data[2] = data2;
  frame.data[3] = data3;
  frame.data[4] = data4;
  CanMsg::sendMsg(frame);
}

void CanMsg::sendMsg(byte prio, byte cmde, byte thisNodeId, byte destNodeId, byte resp, byte data0, byte data1, byte data2, byte data3, byte data4, byte data5) {
  CANMessage frame;
  frame = formatMsg(frame, prio, cmde, thisNodeId, destNodeId, resp);
  frame.len = 6;
  frame.data[0] = data0;
  frame.data[1] = data1;
  frame.data[2] = data2;
  frame.data[3] = data3;
  frame.data[4] = data4;
  frame.data[5] = data5;
  CanMsg::sendMsg(frame);
}

void CanMsg::sendMsg(byte prio, byte cmde, byte thisNodeId, byte destNodeId, byte resp, byte data0, byte data1, byte data2, byte data3, byte data4, byte data5, byte data6) {
  CANMessage frame;
  frame = formatMsg(frame, prio, cmde, thisNodeId, destNodeId, resp);
  frame.len = 7;
  frame.data[0] = data0;
  frame.data[1] = data1;
  frame.data[2] = data2;
  frame.data[3] = data3;
  frame.data[4] = data4;
  frame.data[5] = data5;
  frame.data[6] = data6;
  CanMsg::sendMsg(frame);
}

void CanMsg::sendMsg(byte prio, byte cmde, byte thisNodeId, byte destNodeId, byte resp, byte data0, byte data1, byte data2, byte data3, byte data4, byte data5, byte data6, byte data7) {
  CANMessage frame;
  frame = formatMsg(frame, prio, cmde, thisNodeId, destNodeId, resp);
  frame.len = 8;
  frame.data[0] = data0;
  frame.data[1] = data1;
  frame.data[2] = data2;
  frame.data[3] = data3;
  frame.data[4] = data4;
  frame.data[5] = data5;
  frame.data[6] = data6;
  frame.data[7] = data7;
  CanMsg::sendMsg(frame);
}
