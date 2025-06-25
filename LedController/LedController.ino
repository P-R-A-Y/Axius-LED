uint32_t onBootFreeHeap = ESP.getFreeHeap();

#include <Adafruit_NeoPixel.h>
#include <vector>
#include <AxiusSSD.h>
AxiusSSD axius;

#include "LEDNode.h"
LEDNode LEDNode(&axius);

void setup() {
  randomSeed(148854271337);
  Serial.begin(115200);
  Wire.begin(D5, D6);
  Wire.setClock(400000);

  axius.addMod(&LEDNode);

  axius.setLockScreen(lockScreenRender);
  axius.setLastPreparation(lastPreparation);
  axius.setIconApplyer(applyIcons);
  axius.setLockScreenOverrideChecker(isLockScreenOverrided);
  axius.setIncomingPacketListener(onIncomingPacket);
  axius.setIncomingPayloadListener(onIncomingPayload);
  axius.begin("AxiusLED", MemoryChip::c16, 10000.0f, D4, true, onBootFreeHeap);

  LEDNode.firsttick();
}

void onIncomingPayload(float rssi, uint8_t sender, char* prefix, uint8_t payloadSize, uint8_t* payload) {
  
}

void lastPreparation() {
  axius.setButtons(true, D2, D1, D4, false, true);
}

void onIncomingPacket(esppl_frame_info *info) {
  
}

bool isLockScreenOverrided() {
  return true;
}

void lockScreenRender() {
  axius.drawText("AFK MODE", -1);
  axius.tryForceSwitchToMod(LEDNode.getName());
}

void loop() {
  axius.tick();
  axius.endRender();
}

void applyIcons() {
  
}
