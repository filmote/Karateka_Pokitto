#include "Game.h"
#include "Pokitto.h"
#include "images/images.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;



// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void Game::splashScreen() { 

    // Decrease button counter that prevents you skipping straight over PPOt splash ..

    if (this->splashScreenVariables.buttonCounter > 0) this->splashScreenVariables.buttonCounter--;

    bool justPressed = (this->splashScreenVariables.buttonCounter > 0 ? false : PC::buttons.pressed(BTN_A) || PC::buttons.pressed(BTN_B) || PC::buttons.pressed(BTN_C));

    if (justPressed > 0) {

        this->gameState = GameState::MainMenu; 

    }
    
    if (PC::frameCount % 12 == 0) {

        splashScreenVariables.counter++;
        
        if (splashScreenVariables.counter == 4) {
            
            splashScreenVariables.counter = 0;
            
        }
        
    }
  

    //  Render the state ..

    PD::drawBitmap(22, 27, Images::Ppot_Full);
    PD::drawBitmap(32, 38, Images::Ppot[splashScreenVariables.counter]);

    if (splashScreenVariables.counter < 2) PD::drawBitmap(82, 36, Images::Ppot_Arrow);

}