
#ifndef RFL_Protocol_Start_h
#define RFL_Protocol_Start_h

// ***********************************************************************************
// ***********************************************************************************
class _RFL_Protocol_Start : public _RFL_Protocol_BaseClass {
	
  public:

    // ***********************************************************************
    // Creator, 
    // ***********************************************************************
    _RFL_Protocol_Start () {
      Name = "Start" ;
    }
 
    // ***********************************************************************
    // ***********************************************************************
    bool RF_Decode (  ) {
    
      // *********************************************
      // Production Mode
      // The Real Learning Mode
      // Learning_Mode = 2 detection of the first protocol with statistics
      // Learning_Mode = 3 detection of all protocols with statistics
      // *********************************************
      if ( Learning_Mode <= 3 ) {
        return false ;
      }
    
    
/*  wordt al in RAW gedaan
       // ********************************************
      // if sequence is too short
      // ********************************************
      if ( RawSignal.Number < 26 ) {
    		if ( Learning_Mode >= 3 ) Serial.print ( '<' ) ;
        RawSignal.Number = 0 ;
		    return true; //false ;        
      }
	  
      // ********************************************
      // if sequence is too long
      // ********************************************
      if ( RawSignal.Number > 150 ) {
	    	if ( Learning_Mode >= 3 ) Serial.print ( '>' ) ;
        RawSignal.Number = 0 ;
		    return true;
      }
*/

      // ********************************************
      // Display Times of each puls, 
      // in LM=4 times are rounded at 30 usec
      // ********************************************
      if ( ( Learning_Mode == 4 ) || ( Learning_Mode == 5 ) ) {             
        int Time ;
        sprintf ( pbuffer, "20;%02X;", PKSequenceNumber++ ) ;
        Serial.print ( pbuffer ) ;
        Serial.print ( F( "DEBUG_Start;Pulses=" ) ) ;
        Serial.print ( RawSignal.Number - 3 ) ;
        Serial.print ( F ( ";Pulses(uSec)=" )) ;
        for ( int x=0; x<RawSignal.Number+1; x++ ) {
          Time = RawSignal.Pulses[x] ;
          if ( Learning_Mode == 5 ) {
            Time = 30 * ( Time / 30 ) ;
          }
          Serial.print ( Time ) ; 
          if (x < RawSignal.Number) Serial.print ( "," );       
        }
        Serial.println ( ";" ) ;
        return true ;
      } 


      // ********************************************
      else if ( Learning_Mode == 6 ) {
      // ********************************************
        String BitString = "" ;
        unsigned long P1 ;
        unsigned long P2 ;  

        // ********************************************
        // if shortest pulse is too short
        // or longest pulse is too longest
        // or the ratio is too small or too large
        // ********************************************
        if ( RawSignal.Min < 100  ) Serial.print ( '-' ) ;
        if ( RawSignal.Max > 1500 ) Serial.print ( '+' ) ;
        if ( ( RawSignal.Max / RawSignal.Min > 8 ) || 
             ( RawSignal.Max / RawSignal.Min < 2 ) ) Serial.print ( '~' ) ;
        if ( ( RawSignal.Min < 100 ) || 
             ( RawSignal.Max > 1500 ) || 
             ( RawSignal.Max/RawSignal.Min > 8 ) || 
             ( RawSignal.Max / RawSignal.Min < 2 )) {
          return true;
        }

        sprintf ( pbuffer, "\n20;%02X;", PKSequenceNumber++ ) ;
        Serial.print ( pbuffer ) ;
        Serial.print ( F( "DEBUG_Start;Pulses=" ) ) ;
        Serial.print ( RawSignal.Number - 3 ) ;
        Serial.print ( F ( ";Pulses(uSec)=" )) ;

        Serial.print ( "Min=") ;
        Serial.print ( RawSignal.Min ) ;
        Serial.print ( "   Max=") ;
        Serial.print ( RawSignal.Max ) ;
        Serial.print ( "   Mean=") ;
        Serial.print ( RawSignal.Mean ) ;
        Serial.print ( "   " ) ;

/*
20;0E;DEBUG_Start;Pulses=50;Pulses(uSec)=Mean=450 0000  0000  0101  1101  1111  0010  0
20;0F;EV1527;ID=005DF;SWITCH=02;CMD=ON;
*/		
		    unsigned long BitStream = 0L ;
//        for ( int x=1; x < RawSignal.Number-1; x=x+2 ) {
        for ( int x=2; x < RawSignal.Number; x=x+2 ) {
          P1 = RawSignal.Pulses [ x   ] ;
          P2 = RawSignal.Pulses [ x+1 ] ;
          if ( ( P1 > RawSignal.Mean) && ( P2 < RawSignal.Mean ) ) {
            BitString += '1' ;
            BitStream = ( BitStream << 1 ) | 1 ;
          } else if ( ( P1 < RawSignal.Mean) && ( P2 > RawSignal.Mean ) ) { 
            BitString += '0' ;
            BitStream = BitStream << 1 ;
          } else {
            BitString += "?" ;
            BitStream = BitStream << 1 ;
          }
          if ( ( (x+1) % 8 ) == 0 ) {
            BitString += "  " ;
          }
        }  
        Serial.print ( BitString ) ;
/*
20;03;DEBUG_Start;Pulses=50;Pulses(uSec)=Min=420   Max=1440   Mean=900   0111  0101  0101  0001  0101  0101      1CA5094A
20;04;DEBUG_Start;Pulses=50;Pulses(uSec)=Min=420   Max=1410   Mean=900   0111  0101  0101  0001  0101  0101      1CA5094A
20;05;DEBUG_Start;Pulses=50;Pulses(uSec)=Min=420   Max=1410   Mean=900   0111  0101  0101  0001  0101  0101      1CA5094A
20;06;DEBUG_Start;Pulses=50;Pulses(uSec)=Min=420   Max=1410   Mean=900   0111  0101  0101  0001  0101  0101      1CA5094A
20;07;DEBUG_Start;Pulses=50;Pulses(uSec)=Min=420   Max=1410   Mean=900   0111  0101  0101  0001  0101  0101      1CA5094A

*/
        sprintf ( pbuffer, "   %08X", BitStream ) ; 
        Serial.print ( pbuffer ) ;
        return true;
      }

      
      // ********************************************
      else if ( Learning_Mode == 7 ) {
      // ********************************************
        Serial.print ( "\nNPuls=" ) ;
        Serial.print ( RawSignal.Number -3 ) ;
        Serial.print ( "   Min=") ;
        Serial.print ( RawSignal.Min ) ;
        Serial.print ( "   Max=") ;
        Serial.print ( RawSignal.Max ) ;
        Serial.print ( "   Mean=") ;
        Serial.print ( RawSignal.Mean ) ;
        Serial.print ( "   " ) ;
        return false ;
      }
      
      

      // ********************************************
      // ********************************************
      #define OVERSIZED_LIMIT         291  // longest packet is handled by plugin 48
      #define FA500RM3_PulseLength    26
      #define FA500RM1_PulseLength    58
      #define RAWSIGNAL_SAMPLE_RATE   30   // Sample width / resolution in uSec for raw RF pulses.

      #define PULSE1600               1600 // /RAWSIGNAL_SAMPLE_RATE
      #define PULSE2000               2000
      #define PULSE4000               4000
      #define PULSE4200               4200
      #define PULSE3300               3300
      #define PULSE2500               2500
      #define PULSE3000               3000
      #define PULSE5000               5000

  
      byte FAconversiontype=1;      // 0=FA500R to Method 2
      //                              // 1=FA500R to Method 1
      byte HEconversiontype=0;      // 0=No conversion, 1=conversion to Elro 58 pulse protocol (same as FA500R Method 1)

      int i,j;
      

           
      // ********************************************
      // Beginning of Signal translation for Impuls 
      // ********************************************
      // 20;D7;DEBUG;Pulses=250;Pulses(uSec)=275,325,375,25,75,350,375,25,75,350,375,25,75,350,375,25,75,350,375,25,75,325,375,25,75,325,375,25,75,350,375,25,75,350,375,25,75,350,375,25,75,325,75,350,75,325,375,25,75,4700,75,325,375,25,75,325,375,25,75,350,375,25,75,350,375,25,75,350,375,25,75,325,375,25,75,325,375,25,75,350,375,25,75,350,375,25,75,350,375,25,75,350,75,350,75,325,375,25,75,4700,75,325,375,25,75,325,375,25,75,350,375,25,75,350,375,25,75,350,375,25,75,325,375,25,75,325,375,25,75,325,375,25,75,350,375,25,75,350,375,25,75,350,75,350,75,325,375,25,75,4700,75,325,375,25,75,325,375,25,75,325,375,25,75,350,375,25,75,350,375,25,75,350,375,25,75,325,375,25,75,325,375,25,75,350,375,25,75,350,375,25,75,350,75,325,75,350,375,25,75,4700,75,350,375,25,75,325,375,25,75,325,375,25,75,350,375,25,75,350,375,25,75,350,375,25,75,325,375,25,75,325,375,25,75,325,375,25,75,350,375,25,75,350,75,325,75,350,375,25,75;
//      if (RawSignal.Number == 250) {  // Impuls
      if (RawSignal.Number == 251) {  // Impuls
         if (RawSignal.Pulses[50] > PULSE4200) {      
            if (RawSignal.Pulses[100] > PULSE4200) {      
               if (RawSignal.Pulses[150] > PULSE4200) {      
                  RawSignal.Number=50;            // New packet length
                  RawSignal.Pulses[0]=02;         // signal the plugin number that should process this packet
                  return false;                   // packet detected, conversion done
               }
            }
         }
      }
      // ********************************************
      // Beginning of Signal translation for Home Confort Switches/Remotes
      // ********************************************
//      if (RawSignal.Number == 200) {
      if (RawSignal.Number == 201) {
         if (RawSignal.Pulses[1] > PULSE2000) {
            if (RawSignal.Pulses[100] > PULSE4000) {
               if (RawSignal.Pulses[101] > PULSE2000) {
                  RawSignal.Number=100;           // New packet length
                  RawSignal.Pulses[0]=11;         // signal the plugin number that should process this packet
                  return false;                   // packet detected, conversion done
               }
            }
         }
      }

      if (RawSignal.Number == RAW_BUFFER_SIZE-1) {
         for (j=50;j<104;j++) {  // Only check the total RF packet length we are looking for
             //if (RawSignal.Pulses[j]*RawSignal.Multiply > 2500) {  // input is going to fast skip to where new part is going to start
             if (RawSignal.Pulses[j] > PULSE1600) {  // input is going to fast skip to where new part is going to start
                if (j+52 > RAW_BUFFER_SIZE-1) break; // check for overflow, cant be the packet we look for
                byte x=0;                
                if ( (RawSignal.Pulses[j+52] > PULSE1600) && (RawSignal.Pulses[j+52+52] > PULSE1600) && (RawSignal.Pulses[j+52+52+52] > PULSE1600) ) x=2;
                if ( (RawSignal.Pulses[j+50] > PULSE1600) && (RawSignal.Pulses[j+50+50] > PULSE1600) && (RawSignal.Pulses[j+50+50+50] > PULSE1600) ) x=1;
                if (x !=0) {
                    for (i=0;i<52;i++){
                        RawSignal.Pulses[1+i]=RawSignal.Pulses[j+1+i]; // reorder pulse array
                    }
                    RawSignal.Number=52;            // New packet length
                    RawSignal.Pulses[0]=63;         // signal the plugin number that should process this packet
                    //Serial.println("P002");
                    return false;                   // Conversion done, stop plugin 1 and continue with regular plugins
                }
             } 
         }
      } 
      // ***************************************************
      // END of protocol Start, if the incoming packet is not oversized and resume normal processing of plugins
      // ***************************************************
//Serial.print ( "H");
//Serial.print ( RawSignal.Number - 3 ) ;
      if (RawSignal.Number < OVERSIZED_LIMIT) return false;
      

      // ***************************************************
      // for the moment ignore oversized packages
      // ***************************************************
      return true ;
    }

};
#endif
