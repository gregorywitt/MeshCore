#pragma once

#include <stddef.h>
#include <cstdio>
#include <type_traits>
#include <target.h>
#include "keypad.h"

class UI {
public:
  void begin();
  bool beginDisplay();
  void update();
  bool hasNewInput();
  const char* getInput();
  bool isCommandReady();
  void setInputPreview(const char* input);
  void commitInputPreview();
  void clear();
  bool isPoweringDown() const { return keypad.isPoweringDown(); }

  template<typename... Args>
  void show(Args&&... args) {
    commitInputPreview();
    appendArgs(args...);
    appendText("\n");
    scroll_offset = 0;
    render();
  }

private:
  static constexpr size_t TEXT_BUFFER_SIZE = 4096;
  static constexpr int LINE_HEIGHT = 24;
  static constexpr size_t MAX_LINE_LENGTH = 128;

  Keypad keypad;
  char text_buffer[TEXT_BUFFER_SIZE] = {};
  size_t text_length = 0;
  size_t preview_start = 0;
  size_t scroll_offset = 0;
  bool preview_active = false;

  void appendText(const char* value);
  void ensureTextSpace(size_t count);
  size_t nextVisualLine(size_t position, char* line, size_t line_size) const;
  size_t countVisualLines() const;
  void render();

  void appendArg(const char* value) { appendText(value); }
  void appendArg(char* value) { appendText(value); }

  template<typename T>
  typename std::enable_if<std::is_integral<T>::value>::type appendArg(T value) {
    char text[32];
    if (std::is_signed<T>::value) {
      snprintf(text, sizeof(text), "%lld", static_cast<long long>(value));
    } else {
      snprintf(text, sizeof(text), "%llu", static_cast<unsigned long long>(value));
    }
    appendText(text);
  }

  template<typename T>
  typename std::enable_if<std::is_floating_point<T>::value>::type appendArg(T value) {
    char text[32];
    snprintf(text, sizeof(text), "%g", static_cast<double>(value));
    appendText(text);
  }

  template<typename T>
  void appendArgs(T&& value) {
    appendArg(value);
  }

  template<typename T, typename... Args>
  void appendArgs(T&& first, Args&&... rest) {
    appendArg(first);
    appendArgs(rest...);
  }
};
