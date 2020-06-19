#include "Pokitto.h"
#include "Game.h"

void Game::updateMainMenu() {



    // Handle player actions ..

    if (PC::buttons.pressed(BTN_A)) {

        this->gameState = GameState::Game;

    }


}

