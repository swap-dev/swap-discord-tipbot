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

#ifndef SWAPBOT_CONFIG_H
#define SWAPBOT_CONFIG_H

#include <string>

class Config
{
public:
    Config();
    Config(std::string fileName);
    void setConfig(std::string fileName);

    std::string discordToken;
    std::string adminID;

    std::string coinTicker;
    std::string redisPrefix;
    uint8_t coinUnit;
    uint8_t displayedDecimal;

    int mixin;
    int64_t transferFee;
    uint16_t walletRpcPort;
    uint16_t redisRpcPort;

    std::string faucetChannel;
    int64_t faucetMinReward;
    int64_t faucetMaxReward;
    int64_t faucetTimeInteval;
};

#endif // SWAPBOT_CONFIG_H