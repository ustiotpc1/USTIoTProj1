// This #include statement was automatically added by the Particle IDE.
#include "SparkFunMicroOLED/SparkFunMicroOLED.h"

#include "math.h"

char Org[] = "UST";
char Disp[] = "Paul Temp";
char Locn[] = "MN";

int Temp = 44;


int led1 = D5;
int led2 = D2;



//The amount of time (in milliseconds) to wait between each 
//publication of data via the Particle WebHook
//Particle webhooks are rate limited to a maximum of 10/minute/device
//if we can only send 10 times a minute, that means we can send on average
//about once every six seconds, or 6000 milliseconds.  So to avoid any 
//rate limits by the particle cloud, keep the sendDelay at at least 6000.
int sendDelay = 10000;

//////////////////////////////////
// MicroOLED Object Declaration //
//////////////////////////////////
// Declare a MicroOLED object. If no parameters are supplied, default pins are
// used, which will work for the Photon Micro OLED Shield (RST=D7, DC=D6, CS=A2)

MicroOLED oled;

const int EMAIL_TEMP = 100;

void setup()
{

//  Time.zone(-5);//Set timezone to (CDT) Spring/Summer
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  // Display what's in the buffer (splashscreen)
  delay(1000);     // Delay 1000 ms
  oled.clear(PAGE); // Clear the buffer.
 
//  randomSeed(analogRead(A0) + analogRead(A1));
  printTitle("UST", 1);

  delay(2000);
  //Spark.syncTime();//sync with the Particle Cloud's time

  // We are also going to declare a Spark.function so that we can turn the LED on and off from the cloud.
   //Spark.function("Threshold",updateThreshold);
   // Here's the pin configuration, same as last time
   Spark.function("led",ledToggle);


   pinMode(led1, OUTPUT);
//   pinMode(led2, OUTPUT);

   // This is saying that when we ask the cloud for the function "led", it will employ the function ledToggle() from this app.
   // For good measure, let's also make sure both LEDs are off when we start:
   digitalWrite(led1, LOW);
//   digitalWrite(led2, LOW);

}

 
void loop()
{


  // Read values here
  //f = ???

  // Display to OLED Shield
  DisplayNum(Temp);

  // Send email notification if certain temperature is hit
  if (Temp == EMAIL_TEMP)
  {
      Notify();
  }
  
//  pixelExample();  // Run the pixel example function

  // Emit the sensor values to the Serial port for monitoring
/*
  Serial.println();
  Serial.println("----------");
  Serial.println();
  Serial.println("h=" + String(h) + " t=" + String(t) + " f=" + String(f));
*/
  // Generate the temperature data payload


  char payload[255];
 // snprintf(payload, sizeof(payload),"{\"s\":\"Weather\",\"u\":\"F\",\"m\":\"Temperature\",\"v\": %d,\"o\":\"%s\",\"d\":\"%s\",\"l\":\"%s\"}",Temp,Org,Disp,Locn);
  snprintf(payload, sizeof(payload),"{\"s\":\"Webhook\",\"u\":\"F\",\"m\":\"Temperature\",\"v\": %d,\"o\":\"%s\",\"d\":\"%s\",\"l\":\"%s\"}",Temp,Org,Disp,Locn);
  //Emit the payload to the serial port for monitoring purposes
//  Serial.println(payload);
  
       
  // Send the temperature data payload
  Particle.publish("PublishToEventHub", payload);
  //Wait for the specified "sendDelay" before sending the humidity data...    
  delay(sendDelay);
  

  // For demo, simulate temperature by incrementing by 1
  Temp=Temp+1;

}

int ledToggle(String command) {
    /* Spark.functions always take a string as an argument and return an integer.
    Since we can pass a string, it means that we can give the program commands on how the function should be used.
    In this case, telling the function "on" will turn the LED on and telling it "off" will turn the LED off.
    Then, the function returns a value to us to let us know what happened.
    In this case, 33 will turn on the LED and 44 will turn OFF the LED,
    */

    int threshold = 0;
    
    threshold = command.toInt();
    Temp = threshold;

    if (threshold == 33) {
        digitalWrite(led1,HIGH);
        return 1;
    }
    else if (threshold == 44) {
        digitalWrite(led1,LOW);
        return 0;
    }
    else {
        return -1;
    }
}


void DisplayNum(int num)
{
  // Demonstrate font 3. 12x48. Stopwatch demo.
  oled.setFontType(3);  // Use the biggest font

  oled.clear(PAGE);     // Clear the display
  oled.setCursor(0, 0); // Set cursor to top-left
  oled.print(num);        // Print s's value
  oled.display();       // Draw on the screen
  delay (1000);     
  
}
// Center and print a small title
// This function is quick and dirty. Only works for titles one
// line long.
void printTitle(String title, int font)
{
  int middleX = oled.getLCDWidth() / 2;
  int middleY = oled.getLCDHeight() / 2;

  oled.clear(PAGE);
  oled.setFontType(font);
  // Try to set the cursor in the middle of the screen
  oled.setCursor(middleX - (oled.getFontWidth() * (title.length()/2)),
                 middleY - (oled.getFontWidth() / 2));
  // Print the title:
  oled.print(title);
  oled.display();
  delay(1500);
  oled.clear(PAGE);
}

// Publish notification event to IFTT
void Notify()
{

  char str[10];
  itoa(Temp, str, 10);   
  
  digitalWrite(led1,HIGH);
  printTitle("Email", 1);

  Particle.publish("TempVal", str, 60, PRIVATE);

  delay(2000);
  digitalWrite(led1,LOW);

}