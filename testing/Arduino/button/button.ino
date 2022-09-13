 #include <OneButton.h>

 OneButton button(A1, true);                         //attach a button on pin A1 to the library
 
void setup() {
  Serial.begin(9600);
  
  button.attachDoubleClick(doubleclick);            // link the function to be called on a doubleclick event.
  button.attachClick(singleclick);                  // link the function to be called on a singleclick event.
  button.attachLongPressStop(longclick);            // link the function to be called on a longpress event.
} 
  
 
 
void loop() {
  button.tick();                                    // check the status of the button
 
 
  delay(10);                                        // a short wait between checking the button
} // loop
 
 
 
void doubleclick() {                                // what happens when button is double-clicked
  Serial.println("Double Click");
} 
 
void singleclick(){                                 // what happens when the button is clicked
  Serial.println("Single Click");
}
 
void longclick(){                                   // what happens when buton is long-pressed
  Serial.println("Long Click");
}
