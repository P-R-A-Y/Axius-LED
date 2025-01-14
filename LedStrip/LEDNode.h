#include <Arduino.h>
#include <vector>

#include <AxiusSSD.h>
extern AxiusSSD axius;

class LEDNode : public Mod {
public:
  LEDNode() : strip(NUMPIXELS, STRIPPIN, NEO_GRB + NEO_KHZ800) {};
  void tick() override;
  void firsttick() override {
    strip.begin();
    strip.show();
  };
  void setup() override {};
  String getName() override {return "STATIC LED MODES";}
private:
  Adafruit_NeoPixel strip;
  uint8_t cursor = 0, startpos = 0, mode = 0;
  bool fixedOnParameter = false;

  struct ParameterBase {
    String name;
    uint8_t TYPE;
    ParameterBase(String name, uint8_t type) : name(name), TYPE(type) {};
    virtual void onUp () {};
    virtual void onDwn() {};
    virtual void onOk () {};
  };

  struct ByteValue : public ParameterBase {
    uint8_t value, step = 1;
    uint8_t minValue, maxValue;

    ByteValue(String name, uint8_t defaultValue, uint8_t min, uint8_t max) : ParameterBase(name, 0), value(defaultValue), minValue(min), maxValue(max) {}

    void onUp() override {
      if (value + step <= maxValue) value += step;
      else value = maxValue;
    }

    void onDwn() override {
      if (value - step >= minValue) value -= step;
      else value = minValue;
    }

    void setStepSize(uint8_t ss) {
      step = ss;
    }
  };

  struct BoolValue : public ParameterBase {
    bool value = false;
    
    BoolValue(String name, bool defaultValue) : ParameterBase(name, 1), value(defaultValue) {}

    void onOk() override {
      value = !value;
    }
  };

  struct Mode {
    String name;
    std::vector<ParameterBase*> parameters;
    std::function<void(const std::vector<ParameterBase*>&)> applyChanges;
  };

  std::vector<Mode> modes = {
    {"White", {
      new ByteValue("cold", 20, 0, 40), new ByteValue("brightness", 25, 0, 50)
    }, [&](const std::vector<ParameterBase*>& parameters) {
      uint8_t coldValue = (*(static_cast<ByteValue*>(parameters[0]))).value;
      float brightness = (*(static_cast<ByteValue*>(parameters[1]))).value / 50.0;
      for (uint8_t i = 0; i < NUMPIXELS; i++) {
        ledArray[i].set(255, 255, uint8_t( 255.0 * (coldValue / 40.0) ));
        ledArray[i].mul(brightness, brightness, brightness);
      }
    }},


    {"RGB", {
      new ByteValue("step size", 10, 1, 20), new ByteValue("brightness", 40, 0, 255), new ByteValue("red", 255, 0, 255),
      new ByteValue("green", 255, 0, 255),    new ByteValue("blue", 255, 0, 255),     new BoolValue("random", false)
    }, [&](const std::vector<ParameterBase*>& parameters) {
      uint8_t step = (*(static_cast<ByteValue*>(parameters[0]))).value;
      for (uint8_t i = 1; i < 5; i++) (*(static_cast<ByteValue*>(parameters[i]))).setStepSize(step);

      float brightness = (*(static_cast<ByteValue*>(parameters[1]))).value / 255.0;
      
      ByteValue* R = &(*(static_cast<ByteValue*>(parameters[2])));
      ByteValue* G = &(*(static_cast<ByteValue*>(parameters[3])));
      ByteValue* B = &(*(static_cast<ByteValue*>(parameters[4])));
      if ((*(static_cast<BoolValue*>(parameters[5]))).value) {
        (*(static_cast<BoolValue*>(parameters[5]))).value = false;
        R->value = random(256);
        G->value = random(256);
        B->value = random(256);
      }
      for (uint8_t i = 0; i < NUMPIXELS; i++) {
        ledArray[i].set(R->value, G->value, B->value);
        ledArray[i].mul(brightness, brightness, brightness);
      }
    }},


    {"Pointer", {
      new ByteValue("step size", 10, 1, 20), new ByteValue("cold", 25, 0, 60), new ByteValue("brightness", 50, 0, 255),
      new ByteValue("x position", NUMPIXELS/2, 0, NUMPIXELS-1), new ByteValue("width", 10, 1, NUMPIXELS)
    }, [&](const std::vector<ParameterBase*>& parameters) {
      uint8_t step = (*(static_cast<ByteValue*>(parameters[0]))).value;
      for (uint8_t i = 1; i < 3; i++) (*(static_cast<ByteValue*>(parameters[i]))).setStepSize(step);

      uint8_t coldValue = (*(static_cast<ByteValue*>(parameters[1]))).value;
      float brightness = (*(static_cast<ByteValue*>(parameters[2]))).value / 255.0;

      uint8_t xpos  = (*(static_cast<ByteValue*>(parameters[3]))).value,
              width = (*(static_cast<ByteValue*>(parameters[4]))).value;
      for (uint8_t i = 0; i < NUMPIXELS; i++) {
        if (i < xpos + width/2 && i > xpos - width/2) {
          ledArray[i].set(255, 255, uint8_t( 255.0 * (coldValue / 60.0) ));
          ledArray[i].mul(brightness, brightness, brightness);
        } else {
          ledArray[i].set(0, 0, 0);
        }
      }
    }},
  };






  void commit() {
    modes[mode].applyChanges(modes[mode].parameters);
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(ledArray[i].R, ledArray[i].G, ledArray[i].B));
    }
    strip.show();
  }
};