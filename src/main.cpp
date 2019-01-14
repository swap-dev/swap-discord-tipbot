/*
* Swap Discord Tipbot - Copyright © 2019 Long Huynh (longp.huynh@gmail.com)
*
*  This program is free software; you can redistribute it and/or modify it
*  under the terms of the GNU General Public License as published by the Free
*  Software Foundation; either version 3 of the License, or (at your option)
*  any later version.
*
*  This program is distributed in the hope that it will be useful, but
*  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
*  for more details.
*/

#include "bot.h"
#include "config.h"

#include "curl_easy.h"
#include "curl_header.h"
#include "curl_exception.h"
#include "json.hpp"

int main () 
{
    Config botConfig("config.json");

    Bot bot(botConfig.discordToken, 2);
    bot.setSettings (botConfig);
    
    bot.run();

    return 0;
}
