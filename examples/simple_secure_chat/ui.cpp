#include "ui.h"
#include <Arduino.h>
#include <string.h>
#include <target.h>

void UI::begin() {
  keypad.begin();
}

bool UI::beginDisplay() {
#ifdef DISPLAY_CLASS
  return display.begin();
#else
  return false;
#endif
}

void UI::update() {
  keypad.update();

  if (keypad.newBattStatus()) {
    const int batt_status = keypad.getBattStatus();
    // Handle battery status update here
    if(batt_status <= 10) {
      int percent = batt_status * 10;
      show("batt ", percent, "%");
      Serial.printf("batt %d%%\n", percent);
    }
    if(batt_status == 11) show("batt full");
    if(batt_status == 12) show("batt charging");
    if(batt_status == 13) show("batt low");
    if(batt_status == 14) show("batt ok");
  }

  const int scroll_delta = keypad.takeScrollDelta();
  if (scroll_delta > 0) {
    scroll_offset += static_cast<size_t>(scroll_delta);
    render();
  } else if (scroll_delta < 0) {
    const size_t lines_down = static_cast<size_t>(-scroll_delta);
    scroll_offset = lines_down >= scroll_offset ? 0 : scroll_offset - lines_down;
    render();
  }
}

bool UI::hasNewInput() {
  return keypad.hasNewInput();
}

const char* UI::getInput() {
  return keypad.getInput();
}

bool UI::isCommandReady() {
  return keypad.isCommandReady();
}

void UI::setInputPreview(const char* input) {
  if (preview_active) {
    text_length = preview_start;
    text_buffer[text_length] = '\0';
    preview_active = false;
  }

  if (input && input[0]) {
    if (text_length > 0 && text_buffer[text_length - 1] != '\n') {
      appendText("\n");
    }
    ensureTextSpace(strlen(input));
    preview_start = text_length;
    appendText(input);
    preview_active = true;
  }

  scroll_offset = 0;
  render();
}

void UI::commitInputPreview() {
  if (!preview_active) return;
  preview_active = false;
  appendText("\n");
  scroll_offset = 0;
}

void UI::appendText(const char* value) {
  if (!value || !value[0]) return;

  size_t count = strlen(value);
  const size_t max_length = sizeof(text_buffer) - 1;
  if (count > max_length) {
    value += count - max_length;
    count = max_length;
    text_length = 0;
  }

  ensureTextSpace(count);
  memcpy(text_buffer + text_length, value, count);
  text_length += count;
  text_buffer[text_length] = '\0';
}

void UI::ensureTextSpace(size_t count) {
  const size_t max_length = sizeof(text_buffer) - 1;
  if (text_length + count <= max_length) return;

  const size_t required = text_length + count - max_length;
  size_t remove = required;
  while (remove < text_length && text_buffer[remove - 1] != '\n') {
    remove++;
  }

  memmove(text_buffer, text_buffer + remove, text_length - remove);
  text_length -= remove;
  text_buffer[text_length] = '\0';
}

size_t UI::nextVisualLine(size_t position, char* line, size_t line_size) const {
  if (position >= text_length || line_size == 0) return position;
  if (text_buffer[position] == '\n') {
    line[0] = '\0';
    return position + 1;
  }

#ifdef DISPLAY_CLASS
  const uint16_t display_width = display.width();
  const uint16_t wrap_margin = display.getTextWidth("W");
  const uint16_t max_line_width = wrap_margin < display_width
      ? display_width - wrap_margin
      : display_width;
#endif

  size_t cursor = position;
  size_t length = 0;
  while (cursor < text_length && text_buffer[cursor] != '\n' && length + 1 < line_size) {
    line[length] = text_buffer[cursor];
    line[length + 1] = '\0';
#ifdef DISPLAY_CLASS
    if (display.getTextWidth(line) >= max_line_width && length > 0) {
      line[length] = '\0';
      return cursor;
    }
#endif
    length++;
    cursor++;
  }

  if (cursor < text_length && text_buffer[cursor] == '\n') cursor++;
  return cursor;
}

size_t UI::countVisualLines() const {
  char line[MAX_LINE_LENGTH];
  size_t count = 0;
  for (size_t position = 0; position < text_length; count++) {
    const size_t next = nextVisualLine(position, line, sizeof(line));
    if (next <= position) break;
    position = next;
  }
  return count;
}

void UI::render() {
#ifdef DISPLAY_CLASS
  if (!display.isOn()) return;

  const size_t row_count = display.height() / LINE_HEIGHT > 0
      ? display.height() / LINE_HEIGHT
      : 1;
  const size_t line_count = countVisualLines();
  const size_t max_scroll = line_count > row_count ? line_count - row_count : 0;
  if (scroll_offset > max_scroll) scroll_offset = max_scroll;
  const size_t first_line = max_scroll - scroll_offset;

  display.startFrame();
  display.setCursor(0, 0);

  char line[MAX_LINE_LENGTH];
  size_t position = 0;
  size_t line_number = 0;
  while (position < text_length && line_number < first_line) {
    position = nextVisualLine(position, line, sizeof(line));
    line_number++;
  }

  for (size_t row = 0; row < row_count && position < text_length; row++) {
    position = nextVisualLine(position, line, sizeof(line));
    display.print(line);
    display.print("\n");
  }

  display.endFrame();
  
#endif
}

void UI::clear() {
  keypad.clear();
}
