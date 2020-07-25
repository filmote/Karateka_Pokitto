#include "Pokitto.h"
#include <LibAudio>
#include "src/Game.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;


Game game;

Audio::Sink<4, PROJ_AUD_FREQ> audio;

// void init() {


//     // Initialise pokitto ..

//     PD::loadRGBPalette(palettePico);   
//     PD::persistence = true;
//     PD::setColor(5);
//     PD::setInvisibleColor(14);
//     PC::setFrameRate(30);
//     PD::setFont(fontKoubit);

//     game.setup();

// }


// void update() {

//     game.loop();

// }


int main(){
    
    PC::begin();
    PD::loadRGBPalette(palettePico);   
    PD::persistence = true;
    PD::setColor(5);
    PD::setInvisibleColor(14);
    PC::setFrameRate(40);
    PD::setFont(fontKoubit);

    game.setup();
    
    while( PC::isRunning() ){

            game.loop();
//        PD::drawBitmap(rand()%(PD::width-32), rand()%(PD::height-32), Smile);
    }
    
    return 0;
}
