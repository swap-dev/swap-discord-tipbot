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

#ifndef SWAPBOT_BOT_H
#define SWAPBOT_BOT_H

#include "config.h"
#include "walletapi.h"

#include <string>
#include "redox.hpp"
#include "sleepy_discord/websocketpp_websocket.h"


class Bot : public SleepyDiscord::DiscordClient
{
public:
    using SleepyDiscord::DiscordClient::DiscordClient;

    void setSettings(const Config& config);
    bool ensureRedisUser (std::string userID);
    bool createNewRedisUser(const std::string& userID, const WalletApi::Wallet& wallet);
    void onMessage(SleepyDiscord::Message message);

private:
    std::string formatCoin (int64_t coin);
    std::vector<std::string> splitString(std::string text);
    int64_t getFaucetBalance ();
    int64_t addFaucetBalance (int64_t coinAdded);
    int64_t getUnlockedBalance(std::string userID);
    int64_t getTotalBalance(std::string userID);
    int64_t addUserBalance (std::string userID, int64_t coinAdded);

    std::string mRedisPrefix{""};

    Config mBotConfig;
    WalletApi mWalletApi;
    redox::Redox mRedisConnection;
};

#endif // SWAPBOT_BOT_H