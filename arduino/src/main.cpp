// Include the library
#include <SimpleWebSerial.h>
#include <Servo.h>

Servo servoA, servoB, servoC, servoD;

SimpleWebSerial WebSerial;

void servoCallback( char id, JSONVar msg )
{ 
  if( JSON.typeof(msg) != "number" ) { WebSerial.send("logger", "Message type not a number"); return; }
  
  const int position = static_cast<int>(msg);

  if( position < 0 || position > 180 ) { WebSerial.send("logger", "Servo position out of range"); return; }

  String logMsg = "Servo: " + String(id) + ", position: " + String( position );
  WebSerial.send("logger", logMsg.c_str() );
  
  switch( id )
  {
    case 'A': servoA.write(position); break;
    case 'B': servoB.write(position); break;
    case 'C': servoC.write(position); break;
    case 'D': servoD.write(position); break;
    default: WebSerial.send("logger", "Invalid servo ID"); return;
  }
}

void setup() 
{
  // Initialize serial communication
  Serial.begin( 57600 );

  servoA.attach(3);
  servoB.attach(5);
  servoC.attach(6);
  servoD.attach(9);

  WebSerial.on( "A", [](JSONVar msg) { servoCallback( 'A', msg ); } );
  WebSerial.on( "B", [](JSONVar msg) { servoCallback( 'B', msg ); } );
  WebSerial.on( "C", [](JSONVar msg) { servoCallback( 'C', msg ); } );
  WebSerial.on( "D", [](JSONVar msg) { servoCallback( 'D', msg ); } );
}


unsigned long nextWatchdog = 0;
const int watchdogInterval = 10000;

void loop() 
{
  // Watchdog to detect if the web application has disconnected
  const unsigned long now = millis();
  if( now >= nextWatchdog )
  {
    nextWatchdog = now + watchdogInterval;
    WebSerial.send( "logger", "Watchdog" );
  }

  // Check for new serial data every loop
  WebSerial.check();
  delay(5);  
}
