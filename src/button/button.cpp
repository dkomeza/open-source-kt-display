#include "button.h"

Button::Button() { _pin = -1; }

Button::Button(const int pin) {
  _pin = pin;
  pinMode(pin, true);
}

void Button::onClick(callbackFunction function) { _onClick = function; }
void Button::onDoubleClick(callbackFunction function) {
  _onDoubleClick = function;
}
void Button::onLongPressStart(callbackFunction function) {
  _onLongPressStart = function;
}
void Button::onLongPress(callbackFunction function) { _onLongPress = function; }
void Button::onLongPressStop(callbackFunction function) {
  _onLongPressStop = function;
}

void Button::update() {
  if (_pin >= 0) {
    bool isPressed = digitalRead(_pin) == LOW;
    tick(isPressed);
  }
}

void Button::tick(bool isPressed) {
  unsigned long now = millis();
  unsigned long elapsed = now - _lastChange;
  switch (_state) {
  case INIT:
    if (isPressed) {
      newState(DOWN);
      _lastChange = now;
      _clicks = 0;
    }
    break;
  case DOWN:
    if ((!isPressed) && (elapsed < _debounceTime)) {
      newState(_lastState);
    } else if (!isPressed) {
      newState(UP);
      _lastChange = now;
    } else if ((isPressed) && (elapsed > _pressTime)) {
      if (_onLongPressStart)
        _onLongPressStart();
      newState(UP);
    }
    break;
  case UP:
    if ((isPressed) && (elapsed < _debounceTime)) {
      newState(_lastState);
    } else if (elapsed >= _debounceTime) {
      _clicks++;
      newState(COUNT);
    }
    break;
  case COUNT:
    if (isPressed) {
      newState(DOWN);
      _lastChange = now;
    } else if ((elapsed > _clickTime) || (_clicks == 2)) {
      if (_clicks == 1) {
        if (_onClick)
          _onClick();
      } else {
        if (_onDoubleClick)
          _onDoubleClick();
      }
      reset();
    }
    break;

  case PRESS:
    if (!isPressed) {
      newState(PRESSEND);
      _lastChange = now;
    }
    break;

  case PRESSEND:
    if ((isPressed) && (elapsed < _debounceTime)) {
      newState(_lastState);
    } else if (elapsed >= _debounceTime) {
      if (_onLongPressStop)
        _onLongPressStop();
      reset();
    }
    break;

  default:
    newState(INIT);
    break;
  }
}

void Button::newState(ButtonState nextState) {
  _lastState = _state;
  _state = nextState;
}

void Button::reset() {
  _clicks = 0;
  _lastChange = 0;
  _state = INIT;
  _lastState = INIT;
}