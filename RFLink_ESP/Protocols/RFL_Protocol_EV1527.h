
#ifndef RFL_Protocol_EV1527_h
#define RFL_Protocol_EV1527_h

// ***********************************************************************************
// ***********************************************************************************
class _RFL_Protocol_EV1527 : public _RFL_Protocol_BaseClass
{

public:
  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _RFL_Protocol_EV1527()
  {
    Name = "EV1527";
  }

  // ***********************************************************************
  // ***********************************************************************
  bool RF_Decode()
  {
#define PulseCount 48

    // ****************************************************
    // Check the length of the sequence
    // ****************************************************
    if (RawSignal.Number != PulseCount + 3)
    {
      //        Count = 0 ;
      return false;
    }

    // ****************************************************
    //  Translate the sequence in bit-values and
    //     jump out this method if errors are detected
    // 0-bit = a short high followed by a long low
    // 1-bit = a long  high followed by a short low
    // ****************************************************
    unsigned long BitStream = 0L;
    unsigned long P1;
    unsigned long P2;
    for (byte x = 2; x < PulseCount + 1; x = x + 2)
    {
      P1 = RawSignal.Pulses[x];
      P2 = RawSignal.Pulses[x + 1];
      if (P1 > RawSignal.Mean)
      {
        BitStream = (BitStream << 1) | 0x1; // append "1"
      }
      else
      {
        BitStream = BitStream << 1; // append "0"
      }
    }

    //==================================================================================
    // Prevent repeating signals from showing up
    //==================================================================================
    //      if ( BitStream == Last_BitStream  ) {
    //             Count += 1 ;
    //      } else Count  = 0 ;

    if ((BitStream != Last_BitStream) ||
        (millis() > 700 + Last_Detection_Time))
    {
      // not seen the RF packet recently
      Last_BitStream = BitStream;
    }
    else
    {
      // already seen the RF packet recently
      //         if ( ! Last_Floating ) {
      //           if ( millis() > 700 + Last_Detection_Time ) {
      //             Count = 0 ;                                   // <<< lijkt niet te werken
      //           }
      return true;
      //         }
    }

    String Device = Name;

    // ****************************************************
    // Here we have a valid detection, but we don't know if it's
    //    a EV1527
    // or a PT2262
    // if the number of floats in the first 16 bits is large enough
    // we assume a PT2262 device
    // The PT2262 consists of 12 terniair bits,
    //     0 1 = float
    //     0 0 = 0
    //     1 1 = 1
    // ****************************************************
    int Floating = 0;
    P1 = 0x800000;
    for (byte x = 0; x < 8; x++)
    {
      P2 = P1 >> 1;
      if (((BitStream & P1) == 0) && ((BitStream & P2) != 0))
        Floating += 1;
      P1 = P1 >> 2;
    }

    unsigned long Id = BitStream >> 4;
    unsigned long Switch = BitStream & 0xF;
    // ****************************************************
    // PT2262
    // ****************************************************
    if (Floating >= 6)
    {
      //        Last_Floating = true ;
      //Serial.printf ( "\nCount=%i\n", Count ) ;
      //        if ( Count == 2 ) {
      Device = "PT2262";
      Switch = 1;
      String On_Off = "OFF";
      if ((BitStream & 0x03) != 0)
        On_Off = "ON";
      //sprintf ( pbuffer, "20;%02X;%s;ID=%05X;SWITCH=01;CMD=%s;", PKSequenceNumber++, Device.c_str(), Id, On_Off.c_str() ) ;
      sprintf(pbuffer, "%s;ID=%05X;", Device.c_str(), Id);
      sprintf(pbuffer2, "SWITCH=01;CMD=%s;", On_Off.c_str());
      //        }
      //        else return true; //ok, but no output
    }

    // ****************************************************
    // EV1527
    // ****************************************************
    else
    {
      //        Last_Floating = false ;
      //        unsigned long Switch = BitStream & 0xF ;
      //sprintf ( pbuffer, "20;%02X;%s;ID=%05X;SWITCH=%02X;CMD=ON;", PKSequenceNumber++, Device.c_str(), Id, Switch ) ;
      sprintf(pbuffer, "%s;ID=%05X;", Device.c_str(), Id);
      sprintf(pbuffer2, "SWITCH=%02X;CMD=ON;", Switch);
    }

    /*
      if ( Unknown_Device ( pbuffer ) ) return false ;
      Serial.print   ( PreFix ) ;
      Serial.print   ( pbuffer ) ;
      Serial.println ( pbuffer2 ) ;
      PKSequenceNumber += 1 ;
      return true;
      */
    return Send_Message(Device, Id, Switch, "ON");
  }

  // ***********************************************************************************
  // ***********************************************************************************
  bool Home_Command(String Device, unsigned long ID, int Switch, String On)
  {
    if (Device != Name)
      return false;

    unsigned long Data = (ID << 4) | Switch;
    int NData = 24;
    unsigned long Mask;
    bool Zero;
    int uSec = 300;
    // ************************************
    // send the sequence a number of times
    // ************************************
    for (int R = 0; R < 9; R++)
    {
      // *************************
      // start at the most significant bit position
      // *************************
      Mask = 0x01 << (NData - 1);

      // *************************
      // preamble
      // *************************
      digitalWrite(TRANSMIT_PIN, HIGH);
      delayMicroseconds(uSec);
      digitalWrite(TRANSMIT_PIN, LOW);
      delayMicroseconds(31 * uSec);

      // *************************
      // 20 address bits + 4 data bits
      // *************************
      for (int i = 0; i < NData; i++)
      {
        Zero = (Data & Mask) == 0;
        if (Zero)
        {
          digitalWrite(TRANSMIT_PIN, HIGH);
          delayMicroseconds(uSec);
          digitalWrite(TRANSMIT_PIN, LOW);
          delayMicroseconds(3 * uSec);
        }
        else
        {
          digitalWrite(TRANSMIT_PIN, HIGH);
          delayMicroseconds(3 * uSec);
          digitalWrite(TRANSMIT_PIN, LOW);
          delayMicroseconds(uSec);
        }
        // *************************
        // go to the next bit
        // *************************
        Mask = Mask >> 1;
      }
    }
    return true;
  }

  //  private:
  //    int  Count = 0 ;
  //    bool Last_Floating ;
};
#endif

/*  PT2262 encoding
AA / BB / CC / DD is de draaiknop aan de achterkant
AA
1L = 0001  0101  0001  0101  0101  0111  
1R = 0001  0101  0001  0101  0101  0100
BB
1L = 0101  0100  0001  0101  0101  0111
1R = 0101  0100  0001  0101  0101  0100
DD
1L = 0100  0101  0001  0101  0101  0111
1R = 0100  0101  0001  0101  0101  0100 
CC
1L = 0101  0001  0001  0101  0101  0111  
1R = 0101  0001  0001  0101  0101  0100
2L = 0101  0001  0100  0101  0101  0111 
2R = 0101  0001  0100  0101  0101  0100
3L = 0101  0001  0101  0001  0101  0111
3R = 0101  0001  0101  0001  0101  0100
                 S S   S S
0/1/float        0 F F F
                 F 0 F F
                 F F 0 F
     B B   B B
AA   0 F F F
BB   F F F 0
CC   F F 0 F
DD   F 0 F F
                 
                 
On/Off zit in de laatste 2 bits 
11 = On
00 = Off

Draaischakelaar zit in de eerste

*/
