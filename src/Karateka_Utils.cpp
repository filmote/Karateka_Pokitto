#include "Pokitto.h"
#include "Game.h"

#include "utils/PlayerStances.h"
#include "utils/EnemyStances.h"
#include "images/images.h"
#include "utils/Enums.h"
#include "utils/Utils.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;

uint8_t Game::inStrikingRange(uint8_t action, int16_t attackerXPos, uint8_t targetType, uint8_t targetStance, int16_t targetXPos) {

    int16_t distBetween = absT(targetXPos - attackerXPos);

    if (targetType == ENEMY_TYPE_PERSON) {

        uint16_t pos = (action * ACTIONS_NUMBER_OF_STANCES) + targetStance;
        uint8_t distance = actions_distance[ pos ] >> 2;
        uint8_t damage = actions_distance[ pos ] & 0x03;

        #ifdef DEBUG_STRIKING_RANGE
            _distBetween = distBetween;
            _distTest = distance;
            _pos = pos;
            _targetStance = targetStance;
            _action = action;
        #endif

        #ifdef DEBUG_HITS
            _distBetween = distBetween;
            _distTest = distance;
            _pos = pos;
            _targetStance = targetStance;
        #endif

        #ifdef DEBUG_SERIAL
            Serial.print("inStrikingRange: "); 
            Serial.print(pos); 
            Serial.print(" "); 
            Serial.print(distance); 
            Serial.print(" "); 
            Serial.print(distBetween); 
            Serial.print(" "); 
            Serial.println(damage); 
        #endif

        return ((distBetween <= distance) ? (damage == 0 ? 255 : damage) : 0);

    }
    else {


        // Attacking eagle ..

        if (action != ACTION_LOW_PUNCH) {
            return (distBetween > 35 && distBetween < 43 ? DAMAGE_3_POINT : 0); 
        }
        else {
            return (distBetween > 30 && distBetween < 36 ? DAMAGE_3_POINT : 0); 
        }

    }

    return 0;

}


void Game::returnFromAction(uint8_t action, uint8_t returnAction) {

    switch (returnAction) {  // Return to the default or kick ready position?

        case ACTION_RETURN_TO_DEFAULT:

            switch (action) {

                case ACTION_HIGH_KICK:
                case ACTION_MED_KICK:
                case ACTION_LOW_KICK:
                    enemyStack.push(STANCE_DEFAULT, STANCE_DEFAULT_LEAN_FORWARD);
                    break;

                case ACTION_HIGH_PUNCH:
                case ACTION_MED_PUNCH:
                case ACTION_LOW_PUNCH:
                    enemyStack.push(STANCE_DEFAULT, STANCE_DEFAULT_LEAN_FORWARD, STANCE_PUNCH_READY);
                    break;

            }

            break;

        case ACTION_RETURN_TO_ACTION_READY:

            switch (action) {

                case ACTION_HIGH_KICK:
                case ACTION_MED_KICK:
                case ACTION_LOW_KICK:
                    enemyStack.push(STANCE_KICK_READY);
                    break;

                case ACTION_HIGH_PUNCH:
                case ACTION_MED_PUNCH:
                case ACTION_LOW_PUNCH:
                    enemyStack.push(STANCE_PUNCH_READY);
                    break;

            }

        break;

    }

}


uint8_t Game::getActionFromStance(uint8_t stance) {
  
    switch (stance) {

        case STANCE_KICK_MED_END:
        case STANCE_KICK_HIGH_END:
        case STANCE_KICK_LOW_END:
            return stance - STANCE_KICK_MED_END;

        case STANCE_PUNCH_MED_RH_END:
        case STANCE_PUNCH_MED_LH_END:
            return ACTION_MED_PUNCH;

        case STANCE_PUNCH_HIGH_RH_END:
        case STANCE_PUNCH_HIGH_LH_END:
            return ACTION_HIGH_PUNCH;

        case STANCE_PUNCH_LOW_RH_END:
        case STANCE_PUNCH_LOW_LH_END:
            return ACTION_LOW_PUNCH;

        default:
            return 9;

    }

}


void Game::renderPlayerStance(int8_t x, int8_t y, uint8_t stance) {

    RenderDetails head = { 0, 0, 127, false };
    RenderDetails body = { 0, 0, 127, false };
    RenderDetails leg = { 0, 0, 127, false };

    if (stance <= STANCE_ENTRANCE_6) {

        head.xOffset = player_head[ (stance * 4) ];
        head.yOffset = player_head[ (stance * 4) + 1];
        head.index = player_head[ (stance * 4)  + 2];
        head.rev = (player_head[ (stance * 4) + 3] == 1);

    }

    if (stance <= STANCE_DEATH_2) {

        body.xOffset = player_body[ (stance * 4) ];
        body.yOffset = player_body[ (stance * 4) + 1];
        body.index = player_body[ (stance * 4)  + 2];
        body.rev = (player_body[ (stance * 4) + 3] == 1);

    }

    leg.xOffset = player_legs[ (stance * 4) ];
    leg.yOffset = player_legs[ (stance * 4) + 1];
    leg.index = player_legs[ (stance * 4)  + 2];
    leg.rev = (player_legs[ (stance * 4) + 3] == 1);  

    // if (leg.index != 127) arduboy.drawCompressedMirror(x + leg.xOffset, y + leg.yOffset, masks_leg[leg.index], BLACK, leg.rev);
    // if (leg.index != 127) arduboy.drawCompressedMirror(x + leg.xOffset, y + leg.yOffset, imgs_leg[leg.index], WHITE, leg.rev);
    // if (body.index != 127) arduboy.drawCompressedMirror(x + body.xOffset, y + body.yOffset, imgs_body[body.index], WHITE, body.rev);
    // if (head.index != 127) arduboy.drawCompressedMirror(x + head.xOffset, y + head.yOffset, imgs_head[head.index], WHITE, head.rev);

    if (leg.index != 127)       PD::drawBitmap(x + leg.xOffset, y + leg.yOffset, Images::imgs_leg[leg.index], NOROT, leg.rev ? FLIPH : 0);
    if (body.index != 127)      PD::drawBitmap(x + body.xOffset, y + body.yOffset, Images::imgs_body[body.index], NOROT, body.rev ? FLIPH : 0);
    if (head.index != 127)      PD::drawBitmap(x + head.xOffset, y + head.yOffset, Images::imgs_head[head.index], NOROT, head.rev ? FLIPH : 0);

}


void Game::renderEnemyStance(int8_t x, int8_t y, uint8_t stance) {

    RenderDetails head = { 0, 0, 127, false };
    RenderDetails body = { 0, 0, 127, false };
    RenderDetails leg = { 0, 0, 127, false };

    if (stance <= STANCE_DEATH_4) {

        head.xOffset = enemy_head[ (stance * 4) ];
        head.yOffset = enemy_head[ (stance * 4) + 1];
        head.index = enemy_head[ (stance * 4)  + 2];
        head.rev = (enemy_head[ (stance * 4) + 3 ] == 1);

    }

    if (stance <= STANCE_DEATH_2) {

        body.xOffset = enemy_body[ (stance * 4) ];
        body.yOffset = enemy_body[ (stance * 4) + 1];
        body.index = enemy_body[ (stance * 4)  + 2];
        body.rev = (enemy_body[ (stance * 4) + 3] == 1);

    }

    leg.xOffset = enemy_legs[ (stance * 4) ];
    leg.yOffset = enemy_legs[ (stance * 4) + 1];
    leg.index = enemy_legs[ (stance * 4) + 2];
    leg.rev = (enemy_legs[ (stance * 4) + 3] == 1);  

    // if (leg.index != 127) arduboy.drawCompressedMirror(x + leg.xOffset, y + leg.yOffset, masks_leg[leg.index], BLACK, leg.rev);
    // if (leg.index != 127) arduboy.drawCompressedMirror(x + leg.xOffset, y + leg.yOffset, imgs_leg[leg.index], WHITE, leg.rev);
    // if (body.index != 127) arduboy.drawCompressedMirror(x + body.xOffset, y + body.yOffset, imgs_body[body.index], WHITE, body.rev);
    // if (head.index != 127) arduboy.drawCompressedMirror(x + head.xOffset, y + head.yOffset, imgs_head[head.index], WHITE, head.rev);

    if (leg.index != 127)       PD::drawBitmap(x + leg.xOffset, y + leg.yOffset, Images::imgs_leg[leg.index], NOROT, leg.rev ? FLIPH : 0);
    if (body.index != 127)      PD::drawBitmap(x + body.xOffset, y + body.yOffset, Images::imgs_body[body.index], NOROT, body.rev ? FLIPH : 0);
    if (head.index != 127)      PD::drawBitmap(x + head.xOffset, y + head.yOffset, Images::imgs_head[head.index], NOROT, head.rev ? FLIPH : 0);

}


/* ----------------------------------------------------------------------------
 *  Draw a horizontal dotted line. 
 *  
 *  So much nicer than a solid line!
 */
void Game::drawHorizontalDottedLine(int x1, int x2, int y) {
    
    for (int z = x1; z <= x2; z+=2) {
        // arduboy.drawPixel(z, y, WHITE);
        PD::drawPixel(z, y);
    }
  
}
