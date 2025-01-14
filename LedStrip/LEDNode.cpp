#include "LEDNode.h"

void LEDNode::tick() {
  if (fixedOnParameter) {
    if (axius.readdwn()) {
      axius.updateScreen = true;
      auto parameter = modes[mode].parameters[cursor-1];
      (*(static_cast<ByteValue*>(parameter))).onDwn();
      commit();
    }
    if (axius.readup()) {
      axius.updateScreen = true;
      auto parameter = modes[mode].parameters[cursor-1];
      (*(static_cast<ByteValue*>(parameter))).onUp();
      commit();
    }
  } else {
    if (axius.readdwn()) {
      axius.updateScreen = true;
      if (cursor < modes[mode].parameters.size()/*-1*/) {
        cursor++;
        if (cursor-startpos > 6) startpos++;
      }
    }
    if (axius.readup()) {
      axius.updateScreen = true;
      if (cursor > 0) {
        cursor--;
        if (cursor-startpos < 0) startpos--;
      }
    }
  }
  if (axius.readok()) {
    axius.updateScreen = true;
    if (cursor == 0) {
      if (mode+1 >= modes.size()) mode = 0;
      else mode++;
    } else {
      if (modes[mode].parameters[cursor-1]->TYPE == 0) {
        fixedOnParameter = !fixedOnParameter;
      } else {
        auto parameter = modes[mode].parameters[cursor-1];
        (*(static_cast<ByteValue*>(parameter))).value = !(*(static_cast<ByteValue*>(parameter))).value;
        commit();
      }
    }
    
    
  }
  for (byte i = startpos; i < startpos + (modes[mode].parameters.size()+1 < 7 ? modes[mode].parameters.size()+1 : 7); i++) {
    if (i == 0) {
      axius.drawTextSelector(modes[mode].name, i-startpos, i == cursor);
    } else {
      auto parameter = modes[mode].parameters[i-1];
      String name = parameter->name;
      if (parameter->TYPE == 0) {//byte
        name += ": "+String(   (*(static_cast<ByteValue*>(parameter))).value   );
      } else if (parameter->TYPE == 1) {//bool
        if (  (*(static_cast<BoolValue*>(parameter))).value  ) {
          name += ": true";
        } else {
          name += ": false";
        }
      }
      axius.drawTextSelectorWithBorder(name, i-startpos, i == cursor, (i == cursor && fixedOnParameter));
    }
    
  }
}