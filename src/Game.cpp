#include "Pokitto.h"
#include "Game.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;



void Game::setup(GameCookie *cookie) { 
    
    this->cookie = cookie;
    this->splashScreenVariables.buttonCounter = 16;

}

void Game::loop() {
    
    PC::buttons.pollButtons();
    PD::clear();

    switch(gameState) {

        case GameState::SplashScreen: 
            splashScreen(); 
            break;

        case GameState::Game: 

            switch (this->gameStateDetails.getCurrState()) {

                case GAME_STATE_TITLE_SCENE:

                    player = { STANCE_DEFAULT, 10, 0, 0, 75, 0, HEALTH_STARTING_POINTS, 0, HEALTH_STARTING_POINTS, 0, true, true, false };
                    enemy = { STANCE_DEFAULT, 153, 0, 0, 75, 0, HEALTH_STARTING_POINTS, 0, HEALTH_STARTING_POINTS, 0, true, true, false };
                    showScene();
                    break;

                case GAME_STATE_FOLLOW_SEQUENCE:
                    {
                        gameStateDetails.setCurrState(gameSequence[ (gameStateDetails.sequence * GAME_STATE_SEQ_SIZE) ]); 
                        gameStateDetails.delayInterval = gameSequence[ (gameStateDetails.sequence * GAME_STATE_SEQ_SIZE) + 1]; 
                    
                        uint8_t arches = gameSequence[ (gameStateDetails.sequence * GAME_STATE_SEQ_SIZE) + 2];
                        gameStateDetails.intArch = (arches <= 2 ? arches : 0); 
                        gameStateDetails.extArch = (arches >= 3 ? arches - 2 : 0); 

                        if (arches == 0) {

                        gameStateDetails.archXPos = 0;
                        gameStateDetails.enemyType = gameSequence[ (gameStateDetails.sequence * GAME_STATE_SEQ_SIZE) + 3]; 
                        
                        }
                        else {

                            gameStateDetails.enemyType = 0;
                            if (arches % 2 == 1) {
                                gameStateDetails.archXPos = gameSequence[ (gameStateDetails.sequence * GAME_STATE_SEQ_SIZE) + 3];
                            }
                            else {
                                gameStateDetails.archXPos = (uint16_t)110 - (uint16_t)gameSequence[ (gameStateDetails.sequence * GAME_STATE_SEQ_SIZE) + 3];
                            }
                        
                        }

                        gameStateDetails.showCrevice = (gameSequence[ (gameStateDetails.sequence * GAME_STATE_SEQ_SIZE) + 4] == 1); 
                        gameStateDetails.hasDelay = (gameStateDetails.delayInterval > 0);
                        gameStateDetails.activity = gameSequence[ (gameStateDetails.sequence * GAME_STATE_SEQ_SIZE) + 5];
                        gameStateDetails.outside = gameSequence[ (gameStateDetails.sequence * GAME_STATE_SEQ_SIZE) + 6];
                        gameStateDetails.sequence++;

                    }
                    break;
                
                case GAME_STATE_CASTLE_SCENE:
                case GAME_STATE_THE_END:
                case GAME_STATE_ENEMY_APPROACH:
                case GAME_STATE_PRINCESS:
                    showScene();
                    break;

                case GAME_STATE_EMPEROR_INIT:
                    playerStack.clear();
                    enemyStack.clear();
                    emperorMode = EMPEROR_MODE_INIT;

                    enemy = { STANCE_DEFAULT, 153, 0, 0, 75, 0, HEALTH_STARTING_POINTS, 0, HEALTH_STARTING_POINTS, 0, true, true, false };
                    gameStateDetails.setCurrState(GAME_STATE_EMPEROR);
                    gameStateDetails.outside = false;
                    break;

                case GAME_STATE_EMPEROR:
                    emperor_loop();
                    break;
            
                case GAME_STATE_ENTRANCE_INIT:
                    playerStack.clear();
                    enemyStack.clear();
                    gameStateDetails.setCurrState(GAME_STATE_ENTRANCE);
                    player.stance = STANCE_ENTRANCE_1;
                    enemy.xPos = 140;
                    break;

                case GAME_STATE_ENTRANCE:
                    gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);
                    player.stance = STANCE_ENTRANCE_1;

                    playerStack.push(STANCE_STANDING_UPRIGHT, STANCE_ENTRANCE_6, STANCE_ENTRANCE_5);
                    playerStack.push(STANCE_ENTRANCE_4, STANCE_ENTRANCE_3, STANCE_ENTRANCE_2);
                    playerStack.push(STANCE_ENTRANCE_1);

                    break;

                case GAME_STATE_PLAY_INIT:
                    
                    gameStateDetails.setCurrState(GAME_STATE_PLAY);
                    
                    player.xPos = 6;
                    player.xPosOverall = 0;
                    
                    if (gameStateDetails.enemyType == ENEMY_TYPE_PERSON) {
                        if (player.health - 10 < HEALTH_STARTING_POINTS) { player.health = player.health + 10; }
                        if (player.health - 10 < HEALTH_STARTING_POINTS) { player.health = player.health + 10; }
                    }

                    player.regainLimit = (enemy.health < HEALTH_STARTING_POINTS - (2 * HEALTH_UNIT) ? player.health + (2 * HEALTH_UNIT) : HEALTH_STARTING_POINTS);
                    enemy = { STANCE_DEFAULT, (gameStateDetails.enemyType == ENEMY_TYPE_EAGLE ? 170 : 140), 0, 0, 75, 0, HEALTH_STARTING_POINTS, 0, 0, true, true, false };
                    enemy.health = ((HEALTH_STARTING_POINTS * 2) - player.health  > HEALTH_MAX_POINTS ? HEALTH_MAX_POINTS : (HEALTH_STARTING_POINTS * 2) - player.health);
                    enemy.regainLimit = enemy.health;
                    eagleMode = EAGLE_MODE_FLY_INIT;
                    break;

                case GAME_STATE_PLAY:
                    play_loop();
                    break;

                case GAME_STATE_ENEMY_APPROACH_INIT:
                    playerStack.clear();
                    enemyStack.clear();
                    enemy_approach_init();
                    break;

                case GAME_STATE_GO_THROUGH_GATE:
                    playerStack.clear();
                    enemyStack.clear();
                    player.xPosOverall = -30;
                    gameStateDetails.setCurrState(GAME_STATE_PLAY);
                    break;
                
                case GAME_STATE_FINAL_SCENE:
                    finalScene();
                    break;

            }

            break;


    }
	
}



