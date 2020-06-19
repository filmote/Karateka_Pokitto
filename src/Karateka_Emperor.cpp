#include "Pokitto.h"
#include "Game.h"

#include "utils/PlayerStances.h"
#include "utils/EnemyStances.h"
#include "images/images.h"
#include "utils/Enums.h"
#include "utils/Utils.h"


using PC = Pokitto::Core;
using PD = Pokitto::Display;


void Game::emperor_loop() {

    if (PC::buttons.pressed(BTN_A)) {

        enemyStack.clear();
        gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);

    }
    else {


        // Draw background ..

        PD::drawLine(0, 52, 110, 52);
        // arduboy.drawFastHLine(0, 52, WIDTH);

        for (int i=54; i< 64; i+=2) {

            drawHorizontalDottedLine(i % 2, 110, i);

        }

        if (enemyStack.isEmpty()) {

            switch (emperorMode) {

                case EMPEROR_MODE_INIT:

                    enemyStack.push(STANCE_RUNNING_STRAIGHTEN_UP_REV);

                    for (int i = 0; i < 2; i++) {

                        enemyStack.push(STANCE_RUNNING_RF_END_REV, STANCE_RUNNING_8_REV, STANCE_RUNNING_5_REV);
                        enemyStack.push(STANCE_RUNNING_4_REV, STANCE_RUNNING_LF_END_REV, STANCE_RUNNING_2_REV);
                        enemyStack.push(STANCE_RUNNING_7_REV, STANCE_RUNNING_4_REV);

                    }

                    enemy.xPosDelta = 5;
                    enemy.xPos = -48;

                    break;

                case EMPEROR_MODE_PAUSE_1:
                    for (int i = 0; i < 5; i++) {
                        enemyStack.push(STANCE_STANDING_UPRIGHT_REV);
                    }

                    break;

                case EMPEROR_LIFT_ARM:

                    for (int i =0; i < 9; i++) {
                        enemyStack.push(STANCE_STANDING_UPRIGHT_REV);
                    }

                    break;

                case EMPEROR_MODE_PAUSE_2:
                case EMPEROR_MODE_PAUSE_3:

                    for (int i = 0; i < 3; i++) {
                    enemyStack.push(STANCE_STANDING_UPRIGHT_REV);
                    }

                    break;

                case EMPEROR_MODE_BOW:

                    enemyStack.push(STANCE_BOW_1, STANCE_BOW_2);
                    enemyStack.push(STANCE_BOW_2, STANCE_BOW_1);

                    break;

                case EMPEROR_MODE_FIGHTER_LEAVE:

                    for (int i = 0; i < 2; i++) {

                        enemyStack.push(STANCE_RUNNING_RF_END, STANCE_RUNNING_8, STANCE_RUNNING_5);
                        enemyStack.push(STANCE_RUNNING_4, STANCE_RUNNING_LF_END, STANCE_RUNNING_2);
                        enemyStack.push(STANCE_RUNNING_7, STANCE_RUNNING_4);

                    }

                    enemy.xPosDelta = -5;
                    break;

            }

        }


        // Render screen ..
        
        if (PC::frameCount % ANIMATION_NUMBER_OF_FRAMES == 0) {
        
            enemy.stance = enemyStack.pop();
            enemy.xPos = enemy.xPos + enemy.xPosDelta;

            if (enemyStack.isEmpty()) {

                enemy.xPosDelta = 0;
                emperorMode++;

                if (emperorMode > EMPEROR_MODE_FIGHTER_LEAVE) { gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE); }

            }
        
        }

        // arduboy.drawCompressedMirror(92, 45, emperor_mask, BLACK, false);
        // arduboy.drawCompressedMirror(87, 4, emperor, WHITE, false);
        PD::drawBitmap(87, 4, Images::Emperor);

        if (emperorMode >= EMPEROR_LIFT_ARM && emperorMode < EMPEROR_MODE_FIGHTER_LEAVE) {

    //    arduboy.drawCompressedMirror(70, 19, emperor_arm_out, WHITE, false);
            PD::drawBitmap(87, 4, Images::Emperor);
        
        }
        else {

            // arduboy.drawCompressedMirror(87, 21, emperor_arm_normal, WHITE, false);
            PD::drawBitmap(87, 4, Images::EmperorArmNormal);
        
        }

        // arduboy.drawCompressedMirror(16, 7, arch_interior_rh_mask, BLACK, false);
        // arduboy.drawCompressedMirror(16, 7, arch_interior_rh, WHITE, false);
        PD::drawBitmap(16, 7, Images::ArchInterior_RH);
        renderEnemyStance(enemy.xPos, enemy.yPos, enemy.stance);
        // arduboy.drawCompressedMirror(0, 3, arch_interior_lh_mask, BLACK, false);
        // arduboy.drawCompressedMirror(0, 3, arch_interior_lh, WHITE, false);
        PD::drawBitmap(0, 3, Images::ArchInterior_LH);
        
    }

}
