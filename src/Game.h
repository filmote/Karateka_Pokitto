#pragma once

#include "Pokitto.h"
#include <LibAudio>

#include "utils/Enums.h"
#include "utils/GameCookie.h"
#include "utils/Stack.h"
#include "utils/Structs.h"


class Game {
    
    public:

        void setup(GameCookie *cookie);
        void loop();

    private:


        // Splash Screen

        void splashScreen(); 


        // Title Screen

        void updateMainMenu();



        // Main Game

        void gameSetup();
        void draw_background();
        void play_loop();
        void eagleMovements();
        void emperor_loop();
        void enemy_approach_init();
        void enemyMovements();
        void playerMovements();
        void drawPrincessBackground_1();
        void drawPrincessBackground_2();
        void showScene();
        void finalScene();

        uint8_t inStrikingRange(uint8_t action, int16_t attackerXPos, uint8_t targetType, uint8_t targetStance, int16_t targetXPos);
        uint8_t getActionFromStance(uint8_t stance);
        void renderPlayerStance(int8_t x, int8_t y, uint8_t stance);
        void returnFromAction(uint8_t action, uint8_t returnAction);
        void renderEnemyStance(int8_t x, int8_t y, uint8_t stance);
        void drawHorizontalDottedLine(int x1, int x2, int y);


    private:

        GameState gameState = GameState::SplashScreen;
        SplashScreenVariables splashScreenVariables;
        TitleScreenVars titleScreenVars;
        GameCookie *cookie;

        Stack <uint8_t, 30> playerStack;
        Stack <uint8_t, 30> enemyStack;

        int8_t mainSceneX = 0;
        bool displayHealth = false;
        bool outdoors = true;

        uint8_t enemyHit = 0;
        uint8_t playerHit = 0;

        uint8_t eagleMode = EAGLE_MODE_NONE;
        bool eagleWingUp = false;

        uint8_t emperorMode = EMPEROR_MODE_INIT;
        uint8_t finalSceneMode = FINAL_SCENE_INACTIVE;

        #ifdef USE_DIFFERENT_BAMS
        const uint8_t *bam_images[] =               { bam1, bam2, bam3 };
        const uint8_t *bam_masks[] =                { bam1_mask, bam2_mask, bam3_mask };
        #endif

        bool outside = true;
        GameStateDetails gameStateDetails;

        #ifdef DEBUG_HITS
        int16_t _distBetween = 0;
        int16_t _distTest = 0;
        uint8_t _targetStance = 0;
        uint8_t _action = 0;
        uint16_t _pos = 0;
        #endif

        Entity player = { STANCE_DEFAULT, 10, 0, 0, 55, 0, HEALTH_STARTING_POINTS, 0, HEALTH_STARTING_POINTS, 0, true, true, false };
        Entity enemy = { STANCE_DEFAULT, 153, 0, 0, 55, 0, HEALTH_STARTING_POINTS, 0, HEALTH_STARTING_POINTS, 0, true, true, false };

        bool enemyImmediateAction = false;
        bool enemyImmediateRetreat = false;


};


