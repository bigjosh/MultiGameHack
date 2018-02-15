
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


// Once you have inclided the subgames above, you need to add an entry for ewach one
// one to subgames[] below that maps the loop and setup functions from that namespace

struct Subgame {

  void (*setup)();
  void (*loop)();

  Subgame( void (*setup_m)() , void (*loop_m)() ) { setup=setup_m; loop=loop_m; };
  
};

Subgame subgames[] = {

  Subgame( subgame0::setup , subgame0::loop ),
  Subgame( subgame1::setup , subgame1::loop ),
    
};

void setup() {

  for( byte i=0; i< COUNT_OF( subgames ); i++ ) {
    (subgames[i].setup)(); 
  }
}

byte subgame=0;

Timer newGameTimer;

// Convert the subgame index into an IR code

byte subgame2Token( byte subgame_m ) {
  return (31 - subgame_m );       // Hack. API should have IR_MAX_VALUE
}

byte token2Subgame( byte token ) {

  return (31 - token);       // Hack. API should have IR_MAX_VALUE

}


boolean isSubgameToken( byte possibleToken  ) {

  return ( possibleToken >= 31 - COUNT_OF( subgames ) );
}
  
void loop() {
  // put your main code here, to run repeatedly:

  if (buttonLongPressed()) {

    subgame++;

    if (subgame==COUNT_OF( subgames ) ) {
      subgame=0;
    }

    newGameTimer.set(500);
     
  }

  boolean gameChangeMessageReceived = false;  

  FOREACH_FACE(f) {

    if (!isValueReceivedOnFaceExpired(f) ) {

      byte lastMessage = getLastValueReceivedOnFace(f);
  
      if ( isSubgameToken( lastMessage ) ) {

        gameChangeMessageReceived = true; 

        byte receivedSubgame = token2Subgame( lastMessage );

        if (receivedSubgame != subgame ) {
                   
          subgame=receivedSubgame;
          newGameTimer.set(500);

       }
           
      } 
    }
   
  }

  if (!newGameTimer.isExpired()) {

    setValueSentOnAllFaces( subgame2Token( subgame ) );

    // Don't let normal loop() run while we are changing games. 
    return;
    
  }      

  if (gameChangeMessageReceived) {
    return;                         // Protect the games from ever seeing a game change value they would not understand
  }

  subgames[subgame].loop();

}
