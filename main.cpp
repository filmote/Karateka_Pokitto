#include "Pokitto.h"
#include <LibAudio>
#include "src/Game.h"
#include "src/utils/GameCookie.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;


Game game;
GameCookie cookie;


Audio::Sink<4, PROJ_AUD_FREQ> audio;

void init() {


    // Initialise pokitto ..

    cookie.begin("Karateka", sizeof(cookie), (char*)&cookie);

    PD::loadRGBPalette(palettePico);   
    PD::persistence = true;
    PD::setColor(5);
    PD::setInvisibleColor(14);
    PC::setFrameRate(45);
    PD::setFont(fontKoubit);

    if (cookie.initialised != COOKIE_INITIALISED) {

        cookie.initialise();

    }

    
    // Kick off the random number generator ..

    game.setup(&cookie);

}


void update() {

    game.loop();

}
//https://felipemanga.github.io/PokittoEmu/?url=https://github.com/filmote/RogueBoy_Pokitto/raw/master/RogueBoy_Pokitto.bin
/*
2 LeverPull.raw
3 OpenChest.raw
4 PickUpCoin.raw
*/