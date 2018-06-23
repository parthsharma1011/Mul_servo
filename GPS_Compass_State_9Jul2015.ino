/*
 * uBlox UBX Protocol Reader (runs on Arduino Leonardo, or equivalent)
 *
 * Note: RX pad on 3DR Module is output, TX is input
 */

#include <PString.h>
#include <Wire.h>
#include <HMC5883L.h>

#define MAX_LENGTH 512

#define  POSLLH_MSG  0x02
#define  SBAS_MSG    0x32
#define  VELNED_MSG  0x12
#define  STATUS_MSG  0x03
#define  SOL_MSG     0x06
#define  DOP_MSG     0x04
#define  DGPS_MSG    0x31

#define LONG(X)    *(long*)(&data[X])
#define ULONG(X)   *(unsigned long*)(&data[X])
#define INT(X)     *(int*)(&data[X])
#define UINT(X)    *(unsigned int*)(&data[X])

long lon,lat;
unsigned long hAcc,vAcc;
int gpsSat;
bool printit=false;
char var='#';
//compass
HMC5883L compass;
int error = 0;
#define  COMP_ADD 0x1E
int x,y,z; //triple axis data
float compDeg=0.0;
float heading=0.0;
//-10° 48' Negative West = -10+ -(48/60)=-10.8
//to rad -0.1885
float declinationAngle = -0.18995;//-1.047198;
  MagnetometerScaled scaled;
  
unsigned char  state, lstate, code, id, chk1, chk2, ck1, ck2;
unsigned int  length, idx, cnt;

unsigned char data[MAX_LENGTH];

long lastTime = 0;

void enableMsg (unsigned char id, boolean enable) {
  //               MSG   NAV   < length >  NAV
  byte cmdBuf[] = {0x06, 0x01, 0x03, 0x00, 0x01, id, enable ? 1 : 0};
  sendCmd(sizeof(cmdBuf), cmdBuf);
}

void setup() {
  
//compass setup
  Wire.begin(); // Start the I2C interface.
  compass = HMC5883L(); // Construct a new HMC5883 compass.
  error = compass.SetScale(1.3); // Set the scale of the compass to 1.3Ga
  error = compass.SetMeasurementMode(Measurement_Continuous); // Set the measurement mode to Continuous
  
  Serial.begin(115200);
  Serial1.begin(38400);
  if (false) {
    while(Serial.available() == 0)
      ;
    lstate = state = 0;
  }
  // Modify these to control which messages are sent from module
  enableMsg(POSLLH_MSG, true);    // Enable position messages
  enableMsg(SBAS_MSG, false);      // Enable SBAS messages
  enableMsg(VELNED_MSG, false);    // Enable velocity messages
  enableMsg(STATUS_MSG, true);    // Enable status messages
  enableMsg(SOL_MSG, false);       // Enable soluton messages
  enableMsg(DOP_MSG, false);       // Enable DOP messages
  enableMsg(DGPS_MSG, false);     // Disable DGPS messages
}


void loop() {
  
 if (Serial.available() > 0) {                                                    // check to see if commands are waiting on the serial line
    var = Serial.read();    // read commands from computer
    switch (var) {
    case 'g':
      Serial.print(gpsSat,DEC);
      Serial.print(" ");
      printLatLon(lat);
      Serial.print(" ");
      printLatLon(lon);
      Serial.print(" ");
      Serial.print(vAcc,DEC);     
      Serial.print(" ");
      Serial.print(hAcc,DEC); 
      Serial.print(' ');
      updateCompass();
      Serial.print(compDeg);
      Serial.print(' ');
      break;
    }
  }
  if (Serial1.available()) {
    unsigned char cc = Serial1.read();
    switch (state) {
      case 0:    // wait for sync 1 (0xB5)
        ck1 = ck2 = 0;
        if (cc == 0xB5)
          state++;
        break;
      case 1:    // wait for sync 2 (0x62)
        if (cc == 0x62)
          state++;
        else
          state = 0;
        break;
      case 2:    // wait for class code
        code = cc;
        ck1 += cc;
        ck2 += ck1;
        state++;
        break;
      case 3:    // wait for Id
        id = cc;
        ck1 += cc;
        ck2 += ck1;
        state++;
        break;
      case 4:    // wait for length byte 1
        length = cc;
        ck1 += cc;
        ck2 += ck1;
        state++;
        break;
      case 5:    // wait for length byte 2
        length |= (unsigned int) cc << 8;
        ck1 += cc;
        ck2 += ck1;
        idx = 0;
        state++;
        if (length > MAX_LENGTH)
          state= 0;
        break;
      case 6:    // wait for <length> payload bytes
        data[idx++] = cc;
        ck1 += cc;
        ck2 += ck1;
        if (idx >= length) {
          state++;
        }
        break;
      case 7:    // wait for checksum 1
        chk1 = cc;
        state++;
        break;
      case 8:    // wait for checksum 2
        chk2 = cc;
        boolean checkOk = ck1 == chk1  &&  ck2 == chk2;
        if (checkOk) {
          switch (code) {
            case 0x01:      // NAV-
              // Add blank line between time groups
//              if (lastTime != ULONG(0)) {
//                lastTime = ULONG(0);
//                Serial.print(F("\nTime: "));
//                Serial.println(ULONG(0), DEC);
//              }              
              switch (id) {
                case 0x02:  // NAV-POSLLH
                  //if(printit){
                    //Serial.print(F("POSLLH: lon = "));
                    //printLatLon(LONG(4));
                    //Serial.print(" ");
                    //Serial.print(F(", lat = "));
                    //printLatLon(LONG(8));
                    //Serial.print(F(", vAcc = "));
                    //Serial.print(" ");
                    //Serial.print(ULONG(24), DEC);
                    //Serial.print(" ");
                    //Serial.print(F(" mm, hAcc = "));
                    //Serial.print(ULONG(20), DEC);
                    //Serial.print(F(" mm"));
                    lon=LONG(4);
                    lat=LONG(8);
                    vAcc=ULONG(24);     
                    hAcc=ULONG(20);  
                  //}
                 // printit=false;
                 break;
                case 0x03:  // NAV-STATUS
                    //Serial.print(F("STATUS: gpsFix = "));
//                    Serial.print(data[4], DEC);
//                    Serial.print(" ");
                    gpsSat=data[4];

                    //if (data[5] & 2) {
                    //  Serial.print(F(", dgpsFix"));
                    //}
                  break;
                case 0x04:  // NAV-DOP
                  Serial.print(F("DOP:    gDOP = "));
                  Serial.print((float) UINT(4) / 100, 2);
                  Serial.print(F(", tDOP = "));
                  Serial.print((float) UINT(8) / 100, 2);
                  Serial.print(F(", vDOP = "));
                  Serial.print((float) UINT(10) / 100, 2);
                  Serial.print(F(", hDOP = "));
                  Serial.print((float) UINT(12) / 100, 2);
                  break;
                case 0x06:  // NAV-SOL
                  Serial.print(F("SOL:    week = "));
                  Serial.print(UINT(8), DEC);
                  Serial.print(F(", gpsFix = "));
                  Serial.print(data[10], DEC);
                  Serial.print(F(", pDOP = "));
                  Serial.print((float) UINT(44) / 100.0, 2);
                  Serial.print(F(", pAcc = "));
                  Serial.print(ULONG(24), DEC);
                  Serial.print(F(" cm, numSV = "));
                  Serial.print(data[47], DEC);
                  break;
                case 0x12:  // NAV-VELNED
                  Serial.print(F("VELNED: gSpeed = "));
                  Serial.print(ULONG(20), DEC);
                  Serial.print(F(" cm/sec, sAcc = "));
                  Serial.print(ULONG(28), DEC);
                  Serial.print(F(" cm/sec, heading = "));
                  Serial.print((float) LONG(24) / 100000, 2);
                  Serial.print(F(" deg, cAcc = "));
                  Serial.print((float) LONG(32) / 100000, 2);
                  Serial.print(F(" deg"));
                  break;
                case 0x31:  // NAV-DGPS
                  Serial.print(F("DGPS:   age = "));
                  Serial.print(LONG(4), DEC);
                  Serial.print(F(", baseId = "));
                  Serial.print(INT(8), DEC);
                  Serial.print(F(", numCh = "));
                  Serial.print(INT(12), DEC);
                  break;
                case 0x32:  // NAV-SBAS
                  Serial.print(F("SBAS:   geo = "));
                  switch (data[4]) {
                    case 133:
                      Serial.print(F("Inmarsat 4F3"));
                      break;
                    case 135:
                      Serial.print(F("Galaxy 15"));
                      break;
                    case 138:
                      Serial.print(F("Anik F1R"));
                      break;
                    default:
                      Serial.print(data[4], DEC);
                      break;
                  }
                  Serial.print(F(", mode = "));
                  switch (data[5]) {
                    case 0:
                      Serial.print(F("disabled"));
                      break;
                    case 1:
                      Serial.print(F("enabled integrity"));
                      break;
                    case 2:
                      Serial.print(F("enabled test mode"));
                      break;
                    default:
                      Serial.print(data[5], DEC);
                  }
                  Serial.print(F(", sys = "));
                   switch (data[6]) {
                    case 0:
                      Serial.print(F("WAAS"));
                      break;
                    case 1:
                      Serial.print(F("EGNOS"));
                      break;
                    case 2:
                      Serial.print(F("MSAS"));
                      break;
                     case 16:
                      Serial.print(F("GPS"));
                      break;
                   default:
                      Serial.print(data[6], DEC);
                  }
                  break;
                default:
                  printHex(id);
              }
              //Serial.println();
              break;
            case 0x05:      // ACK-
              //Serial.print(F("ACK-"));
              switch (id) {
                case 0x00:  // ACK-NAK
               // Serial.print(F("NAK: "));
                break;
                case 0x01:  // ACK-ACK
               // Serial.print(F("ACK: "));
                break;
              }
              //printHex(data[0]);
             // Serial.print(" ");
             // printHex(data[1]);
             // Serial.println();
              break;
          }
        }
        state = 0;
        break;
    }
  }   
}

void updateCompass(){
  scaled = compass.ReadScaledAxis();  // Retrieve the scaled values from the magnetometer (scaled to the configured scale).
  heading = atan2(scaled.XAxis,scaled.YAxis);
  heading+=M_PI;
  // Once you have your heading, you must then add your 'Declination Angle',
  // which is the 'Error' of the magnetic field in your location. Mine is 0.0404 
  // Find yours here: http://www.magnetic-declination.com/
  heading += declinationAngle;
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
  // Convert radians to degrees for readability.
  compDeg = heading * 180.0/M_PI; 
}

  // Convert 1e-7 value packed into long into decimal format
void printLatLon (long val) {
  char buffer[14];
  PString str(buffer, sizeof(buffer));
  str.print(val, DEC);
  char len = str.length();
  char ii = 0;
  while (ii < (len - 7)) {
    Serial.write(buffer[ii++]);
  }
  Serial.write('.');
  while (ii < len) {
    Serial.write(buffer[ii++]);
  }
}

void printHex (unsigned char val) {
  if (val < 0x10)
    Serial.print("0");
  Serial.print(val, HEX);
}

void sendCmd (unsigned char len, byte data[]) {
  Serial1.write(0xB5);
  Serial1.write(0x62);
  unsigned char chk1 = 0, chk2 = 0;
  for (unsigned char ii = 0; ii < len; ii++) {
    unsigned char cc = data[ii];
    Serial1.write(cc);
    chk1 += cc;
    chk2 += chk1;
  }
  Serial1.write(chk1);
  Serial1.write(chk2);
}

