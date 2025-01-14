uint32_t onBootFreeHeap = ESP.getFreeHeap();

#define STRIPPIN     D3
#define NUMPIXELS    38

#include <Adafruit_NeoPixel.h>
#include <vector>
#include <AxiusSSD.h>
AxiusSSD axius;

#include "LEDNode.h"
LEDNode LEDNode;

struct Color {
  uint8_t R, G, B;
  void set(uint8_t r, uint8_t g, uint8_t b) {
    R = r;
    G = g;
    B = b;
  }
  void mul(float r, float g, float b) {
    R = uint8_t(r * R);
    G = uint8_t(g * G);
    B = uint8_t(b * B);
  }  
};

Color ledArray[NUMPIXELS];

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
  axius.begin("AxiusLEDS v1", MemoryChip::c16, 10000.0f, D4, true, onBootFreeHeap);
}

void onIncomingPayload(float rssi, uint8_t sender, char* prefix, uint8_t payloadSize, uint8_t* payload) {
  
}

void lastPreparation() {
  axius.setButtons(true, D2, D1, D4, false, true);
}

void onIncomingPacket(esppl_frame_info *info) {
  
}

bool isLockScreenOverrided() {
  return false;
}

void lockScreenRender() {
  axius.showStatusBar = false;
  axius.drawText("AFK MODE", -1);
}

void loop() {
  axius.tick();
  axius.endRender();
}

void applyIcons() {
  
}
