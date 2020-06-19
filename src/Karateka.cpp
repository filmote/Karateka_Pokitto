#include "Pokitto.h"
#include "Game.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;

#include "utils/Stack.h"
#include "utils/PlayerStances.h"
#include "utils/EnemyStances.h"
#include "images/images.h"
#include "utils/Enums.h"
#include "utils/Utils.h"


// ---------------------------------------------------------------------------------------------------------------
//  Setup
// ---------------------------------------------------------------------------------------------------------------

void Game::gameSetup() {
  
  gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);

}


  
// ---------------------------------------------------------------------------------------------------------------
//  Draw the background ..
// ---------------------------------------------------------------------------------------------------------------

void Game::draw_background() {

    const uint8_t *backdrop_img = (outdoors ? Images::Backdrop: Images::Backdrop2);

    // arduboy.drawFastHLine(0, 47, WIDTH, WHITE);
    PD::drawLine(0, 47, 110, 47);


    for (int i = 49; i < 58; i++) {
        drawHorizontalDottedLine(i % 2, 110, i);
    }

    if (gameStateDetails.showCrevice) {
        // arduboy.drawCompressedMirror(-player.xPosOverall, 0, crevice_mask, BLACK, false);
        // arduboy.drawCompressedMirror(-player.xPosOverall, 0, crevice, WHITE, false);
        PD::drawBitmap(-player.xPosOverall, 0, Images::Crevice);
    }

    if (outside) {
        // arduboy.drawCompressedMirror(mainSceneX, -2, backdrop_img, WHITE, false);
        // arduboy.drawCompressedMirror(mainSceneX + MAIN_SCENE_IMG_WIDTH, -2, backdrop_img, WHITE, false);
        // arduboy.drawCompressedMirror(mainSceneX + (2 * MAIN_SCENE_IMG_WIDTH), -2, backdrop_img, WHITE, false);
        // arduboy.drawCompressedMirror(mainSceneX + (3 * MAIN_SCENE_IMG_WIDTH), -2, backdrop_img, WHITE, false);
        PD::drawBitmap(mainSceneX, -2, backdrop_img);
        PD::drawBitmap(mainSceneX + MAIN_SCENE_IMG_WIDTH, -2, backdrop_img);
        PD::drawBitmap(mainSceneX + (2 * MAIN_SCENE_IMG_WIDTH), -2, backdrop_img);
        PD::drawBitmap(mainSceneX + (3 * MAIN_SCENE_IMG_WIDTH), -2, backdrop_img);
    }


    // Draw player triangles ..

    if (PC::frameCount % ANIMATION_FLASHING_TRIANGLES == 0) displayHealth = !displayHealth;

    if (player.health > 20 || displayHealth) {

        for (uint8_t i = 0; i < (player.health / 10); i++) {
//            arduboy.drawCompressedMirror((i * 4), 59, arrow, WHITE, false);
            PD::drawBitmap((i * 4), 59, Images::ArrowLeft);
        }

    }


    // Draw enemy triangles ..

    if (gameStateDetails.enemyType == ENEMY_TYPE_PERSON) {

        if (enemy.health > 20 || displayHealth) {

            for (uint8_t i = (enemy.health / 10); i > 0; i--) {
                // arduboy.drawCompressedMirror(129 - (i * 4), 59, arrow, WHITE, true);
                PD::drawBitmap(129 - (i * 4), 59, Images::ArrowRight);
            }

        }

    }

}


// ---------------------------------------------------------------------------------------------------------------
//  Play Karateka !
// ---------------------------------------------------------------------------------------------------------------

void Game::play_loop() {

    draw_background();
    if (gameStateDetails.hasDelay && gameStateDetails.delayInterval == 0 && playerStack.isEmpty())   { 

        gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE); 

    }
    else {

        playerMovements();

    }

    switch (gameStateDetails.enemyType) {

        case ENEMY_TYPE_PERSON:
            enemyMovements();
            break;

        case ENEMY_TYPE_EAGLE:
            eagleMovements();
            break;

    }



    // Has the player or enemy been hit ?

    uint8_t player_BamX = 0;
    uint8_t player_BamY = 0;

    uint8_t enemy_BamX = 0;
    uint8_t enemy_BamY = 0;

    #ifdef USE_DIFFERENT_BAMS
        const int8_t player_BamXPos[] = {  32,  31,  33,            33,  33,  34,  34,  29,  29 };
        const int8_t enemy_BamXPos[] =  { -17, -17, -19,           -19, -19, -18, -18, -18, -18 };
        const int8_t both_BamYPos[] =   { -30, -17, -42,           -37, -37, -41, -41, -27, -27 };
    #endif
    //                           Kick   M,   L,   H     Punch   MR,  ML,  HR,  HL,  LR,  LL     
    #ifndef USE_DIFFERENT_BAMS
        const int8_t player_BamXPos[] = {  27,  25,  30,            30,  30,  30,  30,  23,  23 };
        const int8_t enemy_BamXPos[] =  { -20, -23, -19,           -22, -22, -18, -18, -24, -24 };
        const int8_t both_BamYPos[] =   { -33, -21, -42,           -39, -39, -45, -45, -31, -31 };
    #endif

    if (PC::frameCount % ANIMATION_NUMBER_OF_FRAMES == 0) {

        enemyHit = 0;
        playerHit = 0;

        if (gameStateDetails.hasDelay && gameStateDetails.delayInterval > 0)      { gameStateDetails.delayInterval--; }


        // Update the player and enemy stances from the stack ..

        if (!playerStack.isEmpty()) {
            player.stance = playerStack.pop();
        }
        else {
            player.xPosDelta = 0;
        }

        if (!enemyStack.isEmpty()) {
            enemy.stance = enemyStack.pop();
        }
        else {
            enemy.xPosDelta = 0;
        }


        // If we are fighting, check to see if a strike has been made ..

        if (gameStateDetails.enemyType != ENEMY_TYPE_NONE) {

            enemyImmediateAction = false;

            if (player.stance >= STANCE_KICK_MED_END && player.stance <= STANCE_PUNCH_LOW_LH_END) {

                enemyHit = inStrikingRange(getActionFromStance(player.stance), player.xPos, gameStateDetails.enemyType, enemy.stance, enemy.xPos);

            }

            if (enemy.stance >= STANCE_KICK_MED_END && enemy.stance <= STANCE_PUNCH_LOW_LH_END) {

                playerHit = inStrikingRange(getActionFromStance(enemy.stance), enemy.xPos, ENEMY_TYPE_PERSON, player.stance, player.xPos);

            }
            if (playerHit > 0 || enemyHit > 0) {
            //sounds

            }

        }

    }


    // Render the background, acrhes and the players ..

    if (player.xPosDelta != 0) {

    int16_t archXPos = gameStateDetails.archXPos;

    if ((gameStateDetails.extArch == 0 && gameStateDetails.intArch == 0) || 
        (gameStateDetails.extArch == ARCH_RIGHT_HAND && archXPos > 96) ||
        (gameStateDetails.intArch == ARCH_RIGHT_HAND && archXPos > 96) ||
        (gameStateDetails.extArch == ARCH_LEFT_HAND && archXPos < 30) ||
        (gameStateDetails.intArch == ARCH_LEFT_HAND && archXPos < 30)
        ) {

        mainSceneX = mainSceneX + player.xPosDelta;
        player.xPosOverall = player.xPosOverall - player.xPosDelta;
        enemy.xPos = enemy.xPos + player.xPosDelta;
        gameStateDetails.archXPos = archXPos + player.xPosDelta;

        if (mainSceneX < -MAIN_SCENE_IMG_WIDTH) { mainSceneX = 0; }
        if (mainSceneX > 0) { mainSceneX = mainSceneX - MAIN_SCENE_IMG_WIDTH; }

    }
    else {

        player.xPos = player.xPos - player.xPosDelta;
        player.xPosOverall = player.xPosOverall - player.xPosDelta;

    }

    }

    if (gameStateDetails.extArch == ARCH_RIGHT_HAND) {
        // arduboy.drawCompressedMirror(gameStateDetails.archXPos, -2, arch_exterior_lh_mask, BLACK, false);
        // arduboy.drawCompressedMirror(gameStateDetails.archXPos, -2, arch_exterior_lh, WHITE, false);
        PD::drawBitmap(gameStateDetails.archXPos, -2, Images::ArchExterior_LH);
    }

    if (gameStateDetails.extArch == ARCH_LEFT_HAND) {
        // arduboy.drawCompressedMirror(gameStateDetails.archXPos + 16, -2, arch_exterior_lh_mask, BLACK, true);
        // arduboy.drawCompressedMirror(gameStateDetails.archXPos + 16, -2, arch_exterior_lh, WHITE, true);
        PD::drawBitmap(gameStateDetails.archXPos + 16, -2, Images::ArchExterior_LH);
    }

    if (gameStateDetails.intArch == ARCH_RIGHT_HAND) {
        // arduboy.drawCompressedMirror(gameStateDetails.archXPos + 2, 7, arch_interior_rh_mask, BLACK, true);
        // arduboy.drawCompressedMirror(gameStateDetails.archXPos + 2, 7, arch_interior_rh, WHITE, true);
        PD::drawBitmap(gameStateDetails.archXPos + 2, 7, Images::ArchExterior_RH);
    }

    if (enemy.xPosDelta != 0) { enemy.xPos = enemy.xPos + enemy.xPosDelta; }
    renderPlayerStance(player.xPos, player.yPos, player.stance);
    if (gameStateDetails.enemyType != ENEMY_TYPE_NONE && enemy.xPos < 128) renderEnemyStance(enemy.xPos, enemy.yPos, enemy.stance);
    if (gameStateDetails.prevState == GAME_STATE_GO_THROUGH_GATE) renderEnemyStance(enemy.xPos, enemy.yPos, STANCE_DEATH_6); 

    if (gameStateDetails.extArch == ARCH_RIGHT_HAND) {
        // arduboy.drawCompressedMirror(gameStateDetails.archXPos + 17, -2, arch_exterior_rh_mask, BLACK, false);
        // arduboy.drawCompressedMirror(gameStateDetails.archXPos + 17, -2, arch_exterior_rh, WHITE, false);
        PD::drawBitmap(gameStateDetails.archXPos + 17, -2, Images::ArchExterior_RH);
    }

    if (gameStateDetails.extArch == ARCH_LEFT_HAND) {
        // arduboy.drawCompressedMirror(gameStateDetails.archXPos - 6, -2, arch_exterior_rh_mask, BLACK, true);
        // arduboy.drawCompressedMirror(gameStateDetails.archXPos - 6, -2, arch_exterior_rh, WHITE, true);
        PD::drawBitmap(gameStateDetails.archXPos - 6, -2, Images::ArchExterior_RH);
    }

    if (gameStateDetails.intArch == ARCH_RIGHT_HAND) {
        // arduboy.drawCompressedMirror(gameStateDetails.archXPos + 16, 3, arch_interior_lh_mask, BLACK, true);
        // arduboy.drawCompressedMirror(gameStateDetails.archXPos + 16, 3, arch_interior_lh, WHITE, true);
        PD::drawBitmap(gameStateDetails.archXPos + 16, 3, Images::ArchInterior_LH);
    }



    //  If the player or enemy has previously been hit, then update their health and render ..

    if (playerHit > 0 || enemyHit > 0) {

        if (playerHit > 0) {

            uint8_t index = (playerHit > 3 ? 2 : playerHit - 1);

            player_BamX = (enemy.xPos + enemy_BamXPos[enemy.stance - STANCE_KICK_MED_END]); 
            if (player_BamX > 128) { player_BamX = 0; }
            player_BamY = enemy.yPos + both_BamYPos[enemy.stance - STANCE_KICK_MED_END]; 

            #ifndef DEBUG_HIDE_BAM

            #ifdef USE_DIFFERENT_BAMS
                // arduboy.drawCompressedMirror(player_BamX, player_BamY, bam_masks[index], BLACK, false);
                // arduboy.drawCompressedMirror(player_BamX, player_BamY, bam_images[index], WHITE, false);
                PD::drawBitmap(player_BamX, player_BamY, Images::bam_images[index], WHITE, false);
            #endif

            #ifndef USE_DIFFERENT_BAMS
                // arduboy.drawCompressedMirror(player_BamX, player_BamY, bam3_mask, BLACK, false);
                // arduboy.drawCompressedMirror(player_BamX, player_BamY, bam3, WHITE, false);
                PD::drawBitmap(player_BamX, player_BamY, Images::Bam3);
            #endif

            #endif

            player.health = (player.health - playerHit < 10 ? 0 : player.health - playerHit);
            player.regain = 0;

        }

        if (enemyHit > 0) {

            uint8_t index = (enemyHit > 3 ? 2 : enemyHit - 1);
            enemy_BamX = player.xPos + player_BamXPos[player.stance - STANCE_KICK_MED_END] ; enemy_BamY = player.yPos + both_BamYPos[player.stance - STANCE_KICK_MED_END]; 

            #ifdef USE_DIFFERENT_BAMS
                // arduboy.drawCompressedMirror(enemy_BamX, enemy_BamY, bam_masks[index], BLACK, false);
                // arduboy.drawCompressedMirror(enemy_BamX, enemy_BamY, bam_images[index], WHITE, false);
                PD::drawBitmap(enemy_BamX, enemy_BamY, Imaghes::bam_images[index]);
            #endif

            #ifndef USE_DIFFERENT_BAMS
                // arduboy.drawCompressedMirror(enemy_BamX, enemy_BamY, bam3_mask, BLACK, false);
                // arduboy.drawCompressedMirror(enemy_BamX, enemy_BamY, bam3, WHITE, false);
                PD::drawBitmap(enemy_BamX, enemy_BamY, Images::Bam3);
            #endif

            if (gameStateDetails.enemyType == ENEMY_TYPE_PERSON) {

                enemy.health = (enemy.health - enemyHit < 10 ? 0 : enemy.health - enemyHit);
                enemy.regain = 0;

                if (enemyStack.isEmpty()) {

                    enemyImmediateAction = (random(0, 2) == 0);     // Should the enemy take an immediate action?
                    enemyImmediateRetreat = (random(0, 3) == 0);    // Should the enemy retreat immediately?

                }

            }
            else {

                eagleMode = EAGLE_MODE_FLY_AWAY;

            }

        }

    }
    else {


        // Let the player and enemy regain some health if they haven't been hit in a while ..

        if (playerHit == 0) {

            player.regain++;

            if (player.regain >= HEALTH_REGAIN_LIMIT) {

                player.health = (player.health + HEALTH_REGAIN_POINTS < player.regainLimit ? player.health + HEALTH_REGAIN_POINTS : player.regainLimit);
                player.regain = 0;

            }

        }

        if (enemyHit == 0) {

            enemy.regain++;

            if (enemy.regain >= HEALTH_REGAIN_LIMIT) {

                enemy.health = (enemy.health + HEALTH_REGAIN_POINTS < enemy.regainLimit ? enemy.health + HEALTH_REGAIN_POINTS : enemy.regainLimit);
                enemy.regain = 0;

            }

        }

    }


    // Has the player died ?

    if (!player.dead && player.health == 0) {

        playerStack.clear();
        playerStack.push(STANCE_DEATH_6, STANCE_DEATH_5, STANCE_DEATH_4);
        playerStack.push(STANCE_DEATH_3, STANCE_DEATH_2, STANCE_DEATH_1);
        player.dead = true;

        if (eagleMode == EAGLE_MODE_NONE || eagleMode == EAGLE_MODE_FLY_INIT) {

            enemyStack.insert(STANCE_DEFAULT_LEAN_BACK);
            for (int i = 0; i < 20; i++) {
                enemyStack.insert(STANCE_STANDING_UPRIGHT);
            }

            enemy.xPosDelta = 0;

        }

        player.xPosDelta = 0;

    }


    // Has the enemy died ?

    if (!enemy.dead && enemy.health == 0) {

        enemyStack.clear();
        enemyStack.push(STANCE_DEATH_6, STANCE_DEATH_5, STANCE_DEATH_4);
        enemyStack.push(STANCE_DEATH_3, STANCE_DEATH_2, STANCE_DEATH_1);
        enemy.dead = true;

        playerStack.insert(STANCE_DEFAULT_LEAN_BACK);
        for (int i = 0; i < 15; i++) {
            playerStack.insert(STANCE_STANDING_UPRIGHT);
        }

        player.xPosDelta = 0;
        enemy.xPosDelta = 0;

    }


    if (gameStateDetails.enemyType != ENEMY_TYPE_NONE && playerStack.isEmpty() && enemyStack.isEmpty() && player.dead)    { gameStateDetails.setCurrState(GAME_STATE_THE_END); }
    if (gameStateDetails.enemyType == ENEMY_TYPE_PERSON && playerStack.isEmpty() && enemyStack.isEmpty() && enemy.dead)   { gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE); }

    if (gameStateDetails.prevState == GAME_STATE_GO_THROUGH_GATE && player.xPos > 128 && playerStack.isEmpty()) {
    gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);
    }

    #ifdef DEBUG_ONSCREEN
        // int16_t distBetween = absT(enemy.xPos - player.xPos);
        // arduboy.fillRect(0, 0, 75, 10, BLACK);
        // arduboy.setCursor(1, 1);
        // arduboy.print(distBetween);
        // arduboy.setCursor(25, 1);
        // arduboy.print(enemyHit);
        // arduboy.setCursor(35, 1);
        // arduboy.print(playerHit);
        // arduboy.setCursor(45, 1);
        // arduboy.print(player.stance);
        // arduboy.setCursor(60, 1);
        // arduboy.print(enemy.stance);
    #endif

    #ifdef DEBUG_HITS
        // int16_t distBetween = absT(enemy.xPos - player.xPos);
        // arduboy.fillRect(0, 0, WIDTH, 10, BLACK);
        // arduboy.setCursor(1, 1);
        // arduboy.print(_distBetween);
        // arduboy.setCursor(15, 1);
        // arduboy.print(_distTest);
        // arduboy.setCursor(30, 1);
        // arduboy.print(enemyHit);
        // arduboy.setCursor(40, 1);
        // arduboy.print(playerHit);
        // arduboy.setCursor(50, 1);
        // arduboy.print(_targetStance);
        // arduboy.setCursor(65, 1);
        // arduboy.print(_pos);
        // arduboy.setCursor(85, 1);
        // arduboy.print(_action);
    #endif

    #ifdef DEBUG_STRIKING_RANGE
        // int16_t distBetween = absT(enemy.xPos - player.xPos);
        // arduboy.fillRect(0, 0, WIDTH, 10, BLACK);
        // arduboy.setCursor(1, 1);
        // arduboy.print(_distBetween);
        // arduboy.setCursor(15, 1);
        // arduboy.print(_distTest);
        // arduboy.setCursor(30, 1);
        // arduboy.print(enemyHit);
        // arduboy.setCursor(40, 1);
        // arduboy.print(playerHit);
        // arduboy.setCursor(50, 1);
        // arduboy.print(_targetStance);
        // arduboy.setCursor(65, 1);
        // arduboy.print(_pos);
        // arduboy.setCursor(85, 1);
        // arduboy.print(_action);
    #endif
  
}
