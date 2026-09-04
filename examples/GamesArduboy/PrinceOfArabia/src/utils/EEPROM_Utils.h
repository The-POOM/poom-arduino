#pragma once

#include "Arduboy2Ext.h"
#include "../PoomFX.h"
#include "../entities/Cookie.h"


class EEPROM_Utils {

  public: 

    EEPROM_Utils(){};
        
    static void loadCookie(Cookie &cookie);
    static void saveCookie(Cookie &cookie);

};


/* ---------------------------------------------------------------------------- */

void EEPROM_Utils::saveCookie(Cookie &cookie) {
    FX::saveGameState(cookie);
}

void EEPROM_Utils::loadCookie(Cookie &cookie) {
    if (!FX::loadGameState(cookie)) {
        cookie.hasSavedLevel = false;
        cookie.hasSavedScore = false;
    }
}
