
/*
  
 This little hack will let you load several games inside a single tile.

 Each subgame should copied into the ske3tch folder and renamed from .ino to .h.
 Also make sure thes games are proper C - so function declarations come before they are accessed.
 
 (I know this sucks and I really wanted to get have the sub-games be submodules, but you just
 can't with Arduino - it is hardcoded to not let you include a .ino file. Arg.)
 
*/

// This is a little tricky. We are actually pulling in the entire game with the #include.
// But I think an elgant solution since we do not have to modify the orginal game file at all, right?
// This means that the setup() and loop() for each subgame will be segrigated, and they can happen to
// have common function names between them and conflict. Cool, right?


namespace subgame0 {
  #include "Mortals.h"  
}


namespace subgame1 {
  #include "Fracture.h"  
}

#define SUBGAME_COUNT 2

void setup() {

  subgame0::setup();
  subgame1::setup();

}

byte subgame=0;

Timer newGameTimer;

#define SUBGAME0_TOKEN 15
#define SUBGAME1_TOKEN 14

byte subgameToken( byte subgame_m ) {

  switch (subgame_m) {

    case 0: return SUBGAME0_TOKEN;
    case 1: return SUBGAME1_TOKEN;
    
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:

  if (buttonLongPressed()) {

    subgame++;

    if (subgame==SUBGAME_COUNT) {
      subgame=0;
    }

    newGameTimer.set(500);
     
  }

  FOREACH_FACE(f) {

    if (!isValueReceivedOnFaceExpired(f) ) {

      byte lastMessage = getLastValueReceivedOnFace(f);
  
      if ( lastMessage == SUBGAME0_TOKEN ) {
  
         if (subgame != 0 ) {
          
          subgame=0;
          newGameTimer.set(500);
  
         }
           
      } 
  
      if ( lastMessage == SUBGAME1_TOKEN ) {
  
         if (subgame != 1 ) {
          
          subgame=1;
          newGameTimer.set(500);
  
         }
      } 
    }
  }

  if (!newGameTimer.isExpired()) {

    setValueSentOnAllFaces( subgameToken( subgame ) );
    return;

    
  }
      

  // Oh I know there is a better way to do with with templates... but this is whole program is a hack!

  switch (subgame) {

    case 0:  
      subgame0::loop();
      break;

    case 1: 
      subgame1::loop();
      break;
    
  }

}
