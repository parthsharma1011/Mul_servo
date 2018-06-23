/*
Author: Danny van den Brande.
With this code you can control multiple servo motors by using a android app.
You can add as many as you like untill your pins run out. You can add more Servos in the code.
If you want the source of the app just contact me.
 */

#include <Servo.h> 
Servo myservo1, myservo2, myservo3, myservo4, myservo5, myservo6, myservo7, myservo8, myservo9, myservo10;

byte serialA;

void setup()
{
  myservo1.attach(9);
  myservo2.attach(10);
  myservo3.attach(11);
  myservo4.attach(3);
  myservo5.attach(5);
  myservo6.attach(6);
//  myservo7.attach(); fill in pin numbers if you use more then 6 servos i left them blank
//  myservo8.attach();
//  myservo9.attach();
//  myservo10.attach();
  Serial.begin(9600);//change your baudrate to your Bluetooth modules baudrate if needed.
}

void loop()
{
  if (Serial.available() > 2) {serialA = Serial.read();Serial.println(serialA);}
  {
    unsigned int servopos = Serial.read();
    unsigned int servopos1 = Serial.read();
    unsigned int realservo = (servopos1 *256) + servopos; 
    Serial.println(realservo); 
    
    if (realservo >= 1000 && realservo <1180){
    int servo1 = realservo;
    servo1 = map(servo1, 1000,1180,0,180);
    myservo1.write(servo1);
    Serial.println("servo 1 ON");
    delay(10);

    }
    
    if (realservo >=2000 && realservo <2180){
      int servo2 = realservo;
      servo2 = map(servo2,2000,2180,0,180);
      myservo2.write(servo2);
      Serial.println("servo 2 On");
      delay(10);
      
    }
    
    if (realservo >=3000 && realservo < 3180){
      int servo3 = realservo;
      servo3 = map(servo3, 3000, 3180,0,180);
      myservo3.write(servo3);
      Serial.println("servo 3 On");
      delay(10);
    }
    if (realservo >=4000 && realservo < 4180){
      int servo4 = realservo;
      servo4 = map(servo4, 4000, 4180,0,180);
      myservo4.write(servo4);
      Serial.println("servo 4 On");
      delay(10);
    }
    
    if (realservo >=5000 && realservo < 5180){
      int servo5 = realservo;
      servo5 = map(servo5, 5000, 5180,0,180);
      myservo5.write(servo5);
      Serial.println("servo 5 On");
      delay(10);
    }
    
    if (realservo >=6000 && realservo < 6180){
      int servo6 = realservo;
      servo6 = map(servo6, 6000, 6180,0,180);
      myservo6.write(servo6);
      Serial.println("servo 6 On");
      delay(10);
    }

//    if (realservo >=7000 && realservo < 7180){ //UNCOMMENT if using more then 6 Servos.
//      int servo7 = realservo;
//      servo7 = map(servo7, 7000, 7180,0,180);
//      myservo7.write(servo7);
//      Serial.println("servo 7 On");
//      delay(10);
//    }
//
//    if (realservo >=8000 && realservo < 8180){
//      int servo8 = realservo;
//      servo8 = map(servo8, 8000, 8180,0,180);
//      myservo8.write(servo8);
//      Serial.println("servo 8 On");
//      delay(10);
//    }
//
//    if (realservo >=9000 && realservo < 9180){
//      int servo8 = realservo;
//      servo8 = map(servo9, 9000, 9180,0,180);
//      myservo8.write(servo8);
//      Serial.println("servo 9 On");
//      delay(10);
//    }
//
//    if (realservo >=10000 && realservo < 10180){
//      int servo10 = realservo;
//      servo10 = map(servo10, 10000, 10180,0,180);
//      myservo10.write(servo10);
//      Serial.println("servo 10 On");
//      delay(10);
//    }
 
  }


}
