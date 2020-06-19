#pragma once

#include "Pokitto.h"
#include "PokittoCookie.h"
#include "Enums.h"

class GameCookie : public Pokitto::Cookie {
    
    public:
    
    	uint8_t initialised;

	public:

		void initialise() {

			this->initialised = COOKIE_INITIALISED;

		}

};
