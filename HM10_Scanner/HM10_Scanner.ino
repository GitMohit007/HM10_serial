//  Pins
//  BT VCC to Arduino 5V out. 
//  BT GND to GND
//  Arduino D8 (SS RX) - BT TX no need voltage divider 
//  Arduino D9 (SS TX) - BT RX through a voltage divider (5v to 3.3v)
//
#include <AltSoftSerial.h>
AltSoftSerial BTserial; 
// https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html

int HM_check_lm=10;
bool HM_alive=false;
bool HM_scanning=false;

char c=' ';
String rp="";
String BT_reply="";

String token;
String delimiter = "OK+DISC";
String delimiter2= ":";
size_t pos=0;
size_t pos1 =0;

String UUID="";
String MAJOR_MINOR="";
float rssi=0.0;
float tx_power=0.0;
float dist=0.0;
String arr[5];
String factid = "4C000215";
size_t factid_len=factid.length();
int tx_pwr=0;

String BTreply()
{
  String R = "";
  delay(3);
  while(BTserial.available())
  {
    c = BTserial.read();
    R=R+c;
    delay(3);
  }
  return R;
  
}


String BTreply_Unbound()
{
  String R = "";
  if(BTserial.available())
  {
    c = BTserial.read();
    R=R+c;
  }
  return R;
  
}

void setup() 
{
    Serial.begin(9600);
    Serial.print("Sketch:   ");   Serial.println(__FILE__);
    Serial.print("Uploaded: ");   Serial.println(__DATE__);
    Serial.println(" ");
 
    BTserial.begin(9600);  
    Serial.println("BTserial started at 9600");
}
 
void loop()
{ 
    unsigned long LP_time = millis();
    // Read from the Bluetooth module and send to the Arduino Serial Monitor
    if(!HM_alive)
    {
      for(int i =0;i<HM_check_lm;i++)
      {
      Serial.print("hm10 connection attempt... ");
      Serial.print(i);
      BTserial.write("AT");
      String S = BTreply();
      //Serial.println(S);
      if (S.length()>0 && S == "OKOK")
      {
        HM_alive=true;
        //BTserial.write("AT+RESET");
        //String S = BTreply();
        //Serial.println(S);
      //  delay(10);
        Serial.println("HM-10 CONNECTED! \nInitiating Scanner");
        break;
      }
      delay(500);
     }
    }
    
    if(HM_alive)
     {
      String Str=" ";
      char ch=' ';
      BTserial.write("AT+DISI?");
      rp = BTreply();
      
      //delay(5);
      if(rp.length()>0 &&( rp.indexOf("OK+DISIS")>-1))
      {
        
        unsigned long wh_time;
        while(true)         
        {
          rp=rp+BTreply_Unbound();
          
          if(rp.length()>0 && (rp.indexOf("OK+DISCE")>-1))
          {
            break;
          } 
          else
          {
            //TODO: Add time counter break function
          }
        }
        
       // Serial.println();
     }   
    pos=rp.indexOf("OK+DISC");
    rp=rp.substring(pos+ delimiter.length());
    pos1 =rp.indexOf(":");
    rp=rp.substring(pos1+delimiter2.length());
    //Serial.println(rp);
    
    if(rp.length()>0)
    {
      while ((pos = rp.indexOf(delimiter)) != -1) 
      {
        token = rp.substring(0, pos);
        pos1=rp.indexOf(":");
        token=token.substring(pos1+delimiter2.length());
        Serial.println(token);
        pos1=rp.indexOf(":");
        String id_test=token.substring(pos1,pos1+factid_len);
        if(id_test==factid)
        {
          int cou=0;
          while((pos1=token.indexOf(delimiter2)) !=-1)
          {
            arr[cou]=token.substring(0,pos1);
          // Serial.println(token.substring(0,pos1));
            cou++;
            token=token.substring(pos1+delimiter2.length());
          }
          arr[4]=token;  
          rssi = arr[4].toFloat();
          tx_power=strtol(arr[2].substring(8).c_str(),NULL,16);

           dist = pow(10.0,((-tx_power-rssi)/(10.0*2)));
          token="";
        //  String H = String(rssi)+" || " + String(tx_power)+" || "+String(dist);
          Serial.println(dist);          
          }
          rp=rp.substring(pos+delimiter.length());
      }
      rp="";      
    }
    }
    //Serial.println("ssssssssssssssssssssssssssss");
    delay(10);
}
