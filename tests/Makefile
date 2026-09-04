CXX ?= c++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -Werror

ROOT := ..
BUILD_DIR := /tmp/poom-arduino-tests
GRAPHICS_TEST := $(BUILD_DIR)/test_graphics
BUZZER_TEST := $(BUILD_DIR)/test_buzzer
STORAGE_TEST := $(BUILD_DIR)/test_storage
IMU_TEST := $(BUILD_DIR)/test_imu
DISPLAY_TEST := $(BUILD_DIR)/test_display

.PHONY: test clean

test: $(GRAPHICS_TEST) $(BUZZER_TEST) $(STORAGE_TEST) $(IMU_TEST) $(DISPLAY_TEST)
	$(GRAPHICS_TEST)
	$(BUZZER_TEST)
	$(STORAGE_TEST)
	$(IMU_TEST)
	$(DISPLAY_TEST)

$(GRAPHICS_TEST): test_graphics.cpp support/Arduino.h $(ROOT)/src/PoomGraphics.cpp $(ROOT)/src/PoomFramebuffer.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I support -I $(ROOT)/src test_graphics.cpp \
		$(ROOT)/src/PoomGraphics.cpp $(ROOT)/src/PoomFramebuffer.cpp -o $(GRAPHICS_TEST)

$(BUZZER_TEST): test_buzzer.cpp support/Arduino.h $(ROOT)/src/PoomBuzzer.cpp $(ROOT)/src/PoomStorage.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I support -I $(ROOT)/src test_buzzer.cpp \
		$(ROOT)/src/PoomBuzzer.cpp $(ROOT)/src/PoomStorage.cpp -o $(BUZZER_TEST)

$(STORAGE_TEST): test_storage.cpp support/Arduino.h support/Preferences.h $(ROOT)/src/PoomStorage.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -DARDUINO_ARCH_ESP32 -I support -I $(ROOT)/src test_storage.cpp \
		$(ROOT)/src/PoomStorage.cpp -o $(STORAGE_TEST)

$(IMU_TEST): test_imu.cpp support/Arduino.h support/Wire.h $(ROOT)/src/PoomIMU.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I support -I $(ROOT)/src test_imu.cpp \
		$(ROOT)/src/PoomIMU.cpp -o $(IMU_TEST)

$(DISPLAY_TEST): test_display.cpp support/Arduino.h support/Wire.h support/Adafruit_SH110X.h $(ROOT)/src/PoomDisplay.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I support -I $(ROOT)/src test_display.cpp \
		$(ROOT)/src/PoomDisplay.cpp -o $(DISPLAY_TEST)

clean:
	rm -f $(GRAPHICS_TEST) $(BUZZER_TEST) $(STORAGE_TEST) $(IMU_TEST) $(DISPLAY_TEST)
