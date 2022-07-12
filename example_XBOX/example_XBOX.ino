#include <XBOXONESBT.h>
#include <usbhub.h>

// Satisfy the IDE, which needs to see the include statement in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

//debugMode (print a lot of stuff into the serial monitor)
bool debugMode = true;
// check if connected
bool onceConnected = false;

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so

/* You can create the instance of the XBOXONESBT class in two ways */
// This will start an inquiry and then pair with the Xbox One S controller - you only have to do this once
// You will need to hold down the Sync and Xbox button at the same time, the Xbox One S controller will then start to blink rapidly indicating that it is in pairing mode
XBOXONESBT Xbox(&Btd, PAIR);

// After that you can simply create the instance like so and then press the Xbox button on the device
//XBOXONESBT Xbox(&Btd);

void setup() {
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.println(F("\r\nOSC did not start"));
    while (1); //halt
  }
  if (debugMode)
    Serial.println(F("\r\nXbox One S Bluetooth Library Started with debugMode")); //F() macro. This helps save SRAM memory
  else
    Serial.println(F("\r\nXbox One S Bluetooth Library Started without debugMode")); //F() macro. This helps save SRAM memory
}
void loop() {
  Usb.Task();

  if (Xbox.connected()) {// Check if xbox is connected
    if (!onceConnected){
      Serial.println(F("Xbox One S Gamepad connected"));
      onceConnected = true;
    }
    //Check if the xbox controller is within the deadzone 
    if (Xbox.getAnalogHat(LeftHatX) > 7500 || Xbox.getAnalogHat(LeftHatX) < -7500 || Xbox.getAnalogHat(LeftHatY) > 7500 || Xbox.getAnalogHat(LeftHatY) < -7500 || Xbox.getAnalogHat(RightHatX) > 7500 || Xbox.getAnalogHat(RightHatX) < -7500 || Xbox.getAnalogHat(RightHatY) > 7500 || Xbox.getAnalogHat(RightHatY) < -7500) {
      
      //if the Left joy stick x-axis(horizontal) has reading
      //getAnalogHat(LeftHatX) is int16_t ideal range -32,768(Left Max) to 32,767(Right Max)
      if (Xbox.getAnalogHat(LeftHatX) > 7500 || Xbox.getAnalogHat(LeftHatX) < -7500) {
        if (debugMode){// print LeftHatX reading
          Serial.print(F("LeftHatX: "));
          Serial.print(Xbox.getAnalogHat(LeftHatX));
          Serial.print("\t");// \t means a tab space, a bit diff then newline
        }
      }


      //if the Left joy stick y-axis(vertical) has reading
      //getAnalogHat(LeftHatY) is int16_t ideal range -32,768(Up Max) to 32,767(Down Max)
      if (Xbox.getAnalogHat(LeftHatY) > 7500 || Xbox.getAnalogHat(LeftHatY) < -7500) {
        if (debugMode){// print LeftHatY reading
          Serial.print(F("LeftHatY: "));
          Serial.print(Xbox.getAnalogHat(LeftHatY));
          Serial.print("\t");
        }
      }

      //if the Right joy stick x-axis(horizontal) has reading
      //getAnalogHat(RightHatX) is int16_t ideal range -32,768(Left Max) to 32,767(Right Max)
      if (Xbox.getAnalogHat(RightHatX) > 7500 || Xbox.getAnalogHat(RightHatX) < -7500) {
        if (debugMode){// print RightHatX reading
          Serial.print(F("RightHatX: "));
          Serial.print(Xbox.getAnalogHat(RightHatX));
          Serial.print("\t");
        }
      }

      //if the Right joy stick y-axis(vertical) has reading
      //getAnalogHat(RightHatY) is int16_t ideal range -32,768(Up Max) to 32,767(Down Max)
      if (Xbox.getAnalogHat(RightHatY) > 7500 || Xbox.getAnalogHat(RightHatY) < -7500) {
        if (debugMode){// print RightHatX reading
          Serial.print(F("RightHatY: "));
          Serial.print(Xbox.getAnalogHat(RightHatY));
        }
      }

      if (debugMode){// print newline symbol 
        Serial.println();
      }
    }

    // check if left and right trigger has reading (RT == Right Trigger, LT == Left Trigger)
    if (Xbox.getButtonPress(LT) > 0 || Xbox.getButtonPress(RT) > 0) {
      // check if left trigger has reading
      // getButtonPress(LT) range for left trigger 0(not pulling) to 1023(pull all the way down)
      if (Xbox.getButtonPress(LT) > 0) {
        if (debugMode){// print LT reading
          Serial.print(F("LT: "));
          Serial.print(Xbox.getButtonPress(LT));//left trigger reading 
          Serial.print("\t");// print tab symbol 
        }
      }

      // check if right trigger has reading
      // getButtonPress(RT) range for righ trigger 0(not pulling) to 1023(pull all the way down)
      if (Xbox.getButtonPress(RT) > 0) {
        if (debugMode){// print RT reading
          Serial.print(F("RT: "));
          Serial.print(Xbox.getButtonPress(RT));//right trigger reading 
          Serial.print("\t");// print tab symbol 
        }
      }

      if (debugMode){// print newline symbol 
        Serial.println();
      }
    }

    // Set rumble effect
    static uint16_t oldLTValue, oldRTValue;
    if (Xbox.getButtonPress(LT) != oldLTValue || Xbox.getButtonPress(RT) != oldRTValue) {//if trigger value is different
      oldLTValue = Xbox.getButtonPress(LT);
      oldRTValue = Xbox.getButtonPress(RT);
      uint8_t leftRumble = map(oldLTValue, 0, 1023, 0, 255); // Map the trigger values(0 to 1023) into a byte(0 to 255)
      uint8_t rightRumble = map(oldRTValue, 0, 1023, 0, 255);
      if (leftRumble > 0 || rightRumble > 0)
        //set some vibration effect to the xbox gamepad
        //setRumbleOn(uint8_t leftTrigger, uint8_t rightTrigger, uint8_t leftMotor, uint8_t rightMotor)
        //setRumbleOn(leftTrigger,x,x,x) is vibration on the leftTrigger, range 0(off) to 255(Max)
        //setRumbleOn(x,rightTrigger,x,x) is vibration on the rightTrigger, range 0(off) to 255(Max)
        //setRumbleOn(x,x,leftMotor,x) is low freq vibration on the left side of the gamepad, range 0(off) to 255(Max)
        //setRumbleOn(x,x,leftMotor,x) is high freq vibration on the right side of the gamepad, range 0(off) to 255(Max)
        Xbox.setRumbleOn(leftRumble, rightRumble, 0, 0);
      else
        Xbox.setRumbleOff();
    }


    // UP, DOWN, LEFT, RIGHT button on the left bottom of the gamepad
    // UP button. True means being click, False means not being click
    // click return true once for each button press
    // i.e. getButtonPress(ButtonEnum b) will return a true as long as a button is held down, while getButtonClick(ButtonEnum b) will return true once for each button press.
    if (Xbox.getButtonClick(UP)){
      if (debugMode){
        Serial.println(F("Up key pressed"));
      }
    }

    // DOWN button. True means being click, False means not being click
    if (Xbox.getButtonClick(DOWN)){
      if (debugMode){
        Serial.println(F("Down key pressed"));
      }
    }

    // LEFT button. True means being click, False means not being click
    if (Xbox.getButtonClick(LEFT)){
      if (debugMode){
        Serial.println(F("Left key pressed"));
      }
    }

    // RIGHT button. True means being click, False means not being click
    if (Xbox.getButtonClick(RIGHT)){
      if (debugMode){
        Serial.println(F("Right key pressed"));
      }
    }


    //VIEW(center left one), MENU(center right one), Xbox(the one with xbox) button 
    //VIEW button
    if (Xbox.getButtonClick(VIEW)){
      if (debugMode){
        Serial.println(F("View key pressed"));
      }
      debugMode = !debugMode;
      if (debugMode){
        Serial.println(F("debugMode Enabled"));
      }else{
        Serial.println(F("debugMode Disabled"));
      }
    }
    
    //MENU button
    if (Xbox.getButtonClick(MENU)){
      if (debugMode){
      Serial.println(F("Menu key pressed"));
      }
    }

    //XBOX button
    if (Xbox.getButtonClick(XBOX)) {
      if (debugMode){
        Serial.println(F("Xbox key pressed"));
      }
      Xbox.disconnect();// disconnect the bluetooth connection
    }

    //LB == Left Bumper button
    if (Xbox.getButtonClick(LB)){
      if (debugMode){
        Serial.println(F("LB key pressed"));
      }
    }
    
    //RB == right Bumper button
    if (Xbox.getButtonClick(RB)){
      if (debugMode){
        Serial.println(F("RB key pressed"));
      }
    }
    
    if (Xbox.getButtonClick(LT))
      if (debugMode)
        Serial.println(F("LT"));
    if (Xbox.getButtonClick(RT))
      if (debugMode)
        Serial.println(F("RT"));
    //L3 == Left joystick button
    if (Xbox.getButtonClick(L3))
      if (debugMode)
        Serial.println(F("L3 key pressed"));
    //R3 == Right joystick button
    if (Xbox.getButtonClick(R3))
      if (debugMode)
        Serial.println(F("R3 key pressed"));


    if (Xbox.getButtonClick(A))
      if (debugMode)
        Serial.println(F("A key pressed"));
    if (Xbox.getButtonClick(B))
      if (debugMode)
        Serial.println(F("B key pressed"));
    if (Xbox.getButtonClick(X))
      if (debugMode)
        Serial.println(F("X key pressed"));
    if (Xbox.getButtonClick(Y))
      if (debugMode)
        Serial.println(F("Y key pressed"));
  
  }else{
    if (onceConnected){
      Serial.println(F("Xbox One S Gamepad disconnected"));
      onceConnected = false;
    }
  }
}
