#include "Pokitto.h"
#include "Game.h"

#include "utils/PlayerStances.h"
#include "utils/EnemyStances.h"
#include "images/images.h"
#include "utils/Enums.h"


using PC = Pokitto::Core;
using PD = Pokitto::Display;

void Game::drawPrincessBackground_1() {

    const uint8_t groundX[] = { 48, 62, 
                                38, 72, 
                                28, 82, 
                                8, 102, 
                                28, 82, 
                                38, 72, 
                                48, 62 };

    uint8_t x = 0;

    for (int y = 70; y < 84; y = y + 2) {
    // arduboy.drawFastHLine(x, y, WIDTH - x);    
        PD::drawLine(groundX[x], y, groundX[x + 1], y);
        x = x + 2;
        // x = x + (y < 76 ? -20 : 20);
        // printf("line %i %i %i %i\n", x, y, 110 - x, y);
    }

    // arduboy.drawCompressedMirror(16, 7, arch_interior_rh_mask, BLACK, false);
    // arduboy.drawCompressedMirror(16, 7, arch_interior_rh, WHITE, false);
    // arduboy.drawCompressedMirror(89, 19, princess_seat_mask, BLACK, false);
    // arduboy.drawCompressedMirror(89, 19, princess_seat, WHITE, false);
    PD::drawBitmap(14, 17, Images::ArchInterior_RH);
    PD::drawBitmap(73, 39, Images::Princess_Seat);


}

void Game::drawPrincessBackground_2() {

    // arduboy.drawCompressedMirror(0, 3, arch_interior_lh_mask, BLACK, false);
    // arduboy.drawCompressedMirror(0, 3, arch_interior_lh, WHITE, false);
    PD::drawBitmap(0, 14, Images::ArchInterior_LH);

}

void Game::showScene() {

    if (PC::buttons.pressed(BTN_A)) {
        
        if (gameStateDetails.getCurrState() == GAME_STATE_THE_END) {
            gameStateDetails.sequence = 0;
        }     

        enemyStack.clear();
        gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);

    }
    else {
        
        switch (gameStateDetails.getCurrState()) {

            case GAME_STATE_TITLE_SCENE:
                // arduboy.drawCompressedMirror(0, 0, title, WHITE, false);
                PD::drawBitmap(0, 33, Images::Title);
                break;

            case GAME_STATE_CASTLE_SCENE:
                PD::drawBitmap(0, 0, Images::Castle);
                break;
            
            case GAME_STATE_THE_END:
                PD::drawBitmap(25, 26, Images::TheEnd);
                break;
            
            case GAME_STATE_PRINCESS:

                drawPrincessBackground_1();
                drawPrincessBackground_2();
                // arduboy.drawCompressedMirror(93, 19, princess_sitting_mask, BLACK, false);
                // arduboy.drawCompressedMirror(93, 19, princess_sitting, WHITE, false);
                PD::drawBitmap(77, 39, Images::Princess_Sitting);
                
                break;

            case GAME_STATE_ENEMY_APPROACH:

                draw_background();
            
                //  Update player and enemy positions and stances ..
            
                if (PC::frameCount % ANIMATION_NUMBER_OF_FRAMES == 0) {
                    
                    if (!enemyStack.isEmpty()) {
                        enemy.stance = enemyStack.pop();
                    }
                    else {
                        player.xPosDelta = 0;
                        gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);
                    }
                    
                }
            
                
                // Move scenery if needed ..
            
                if (player.xPosDelta != 0) {
            
                    mainSceneX = mainSceneX + player.xPosDelta;
            
                    if (mainSceneX < -MAIN_SCENE_IMG_WIDTH) { mainSceneX = 0; }
                    if (mainSceneX > 0) { mainSceneX = mainSceneX - MAIN_SCENE_IMG_WIDTH; }
            
                }
            
                if (enemy.xPos < 128) renderEnemyStance(enemy.xPos, enemy.yPos, enemy.stance);

                break;

            default: break;

        }
        
        if (PC::frameCount % ANIMATION_NUMBER_OF_FRAMES == 0) {

            if (gameStateDetails.delayInterval > 0) {

                gameStateDetails.delayInterval--;

                if (gameStateDetails.delayInterval == 0) { 

                    // if (gameStateDetails.getCurrState() == GAME_STATE_THE_END) {  // Not relevant for 'The End' scene.
                    //   gameStateSeq = 0;
                    // }            
                    gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE); 

                }

            }

        }

    }

}



void Game::finalScene() {
    
    if (PC::buttons.pressed(BTN_A)) {
        
        enemyStack.clear();
        gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);

    }
    else {
        

        // Draw background ..

        drawPrincessBackground_1();

        if (enemyStack.isEmpty() && playerStack.isEmpty()) {

            finalSceneMode++;

            switch (finalSceneMode) {

                case FINAL_SCENE_INIT:

                    for (int i = 0; i < 3; i++) {

                        playerStack.push(STANCE_RUNNING_LF_END, STANCE_RUNNING_2, STANCE_RUNNING_7);
                        playerStack.push(STANCE_RUNNING_4);

                        playerStack.push(STANCE_RUNNING_RF_END, STANCE_RUNNING_8, STANCE_RUNNING_5);
                        playerStack.push(STANCE_RUNNING_4);

                    }

                    player.xPosDelta = 5;
                    player.xPos = -64;

                    enemyStack.push(STANCE_PRINCESS_STANDING);
                    for (int i = 0; i < 12; i++) {
                        enemyStack.push(STANCE_PRINCESS_SITTING);
                    }
                    enemy.xPos = 77;//93;
                    enemy.yPos = 34;

                    break;

                case FINAL_SCENE_PAUSE:
                    playerStack.push(STANCE_STANDING_UPRIGHT);
                    playerStack.push(STANCE_RUNNING_STRAIGHTEN_UP);
                    break;

                case FINAL_SCENE_PAUSE_2:
                    player.xPosDelta = 0;
                    enemy.xPosDelta = 0;
                    for (int i = 0; i < 4; i++) {
                    playerStack.push(STANCE_STANDING_UPRIGHT);
                    }
                    break;

                case FINAL_SCENE_KISSING:
                    enemy.xPos = 74;//90;
                    enemy.yPos = 35;
                    enemyStack.push(STANCE_PRINCESS_KISSING, STANCE_PRINCESS_KISSING, STANCE_PRINCESS_KISSING);
                    break;

                case FINAL_SCENE_BREAK_UP:
                    enemy.xPos = 77;//93;
                    enemy.yPos = 35;
                    for (int i = 0; i < 18; i++) {
                    enemyStack.push(STANCE_PRINCESS_STANDING);
                    }
                    break;

            }

        }


        // Render screen ..

        if (PC::frameCount % ANIMATION_NUMBER_OF_FRAMES == 0) {

            if (!enemyStack.isEmpty()) {
                enemy.stance = enemyStack.pop();
                enemy.xPos = enemy.xPos + enemy.xPosDelta;
            }

            if (!playerStack.isEmpty()) {
                player.stance = playerStack.pop();
                player.xPos = player.xPos + player.xPosDelta;
            }

        }

        renderPlayerStance(player.xPos, player.yPos, player.stance);
        renderEnemyStance(enemy.xPos, enemy.yPos, enemy.stance);

        drawPrincessBackground_2();

        if (finalSceneMode == FINAL_SCENE_BREAK_UP && enemyStack.isEmpty()) {

            gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);

        }

    }

}