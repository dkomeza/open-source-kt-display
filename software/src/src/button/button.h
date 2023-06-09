#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
typedef void (*callbackFunction)(void);

class Button {
 public:
  Button();
  Button(const int pin);

  void onClick(callbackFunction function);
  void onDoubleClick(callbackFunction function);
  void onLongPressStart(callbackFunction function);
  void onLongPress(callbackFunction function);
  void onLongPressStop(callbackFunction function);

  void update();
  bool isPressed();

 private:
  int _pin;
  bool _isPressed;
  unsigned long _lastChange;
  int _clicks;

  unsigned int _debounceTime = 50;
  unsigned int _clickTime = 200;
  unsigned int _pressTime = 600;

  callbackFunction _onClick = NULL;
  callbackFunction _onDoubleClick = NULL;
  callbackFunction _onLongPressStart = NULL;
  callbackFunction _onLongPress = NULL;
  callbackFunction _onLongPressStop = NULL;

  enum ButtonState : int {
    INIT = 0,
    DOWN = 1,
    UP = 2,
    COUNT = 3,
    PRESS = 4,
    PRESSEND = 5,
  };

  void tick(bool isPressed);
  void newState(ButtonState nextState);
  void reset();

  ButtonState _state = INIT;
  ButtonState _lastState = INIT;
};

#endif