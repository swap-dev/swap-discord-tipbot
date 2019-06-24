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

#include "config.h"

#include <fstream>
#include "json.hpp"

Config::Config()
{
    setConfig("config.json");
}

Config::Config(std::string fileName)
{
    setConfig(fileName);
}

void Config::setConfig(std::string fileName)
{
    std::ifstream inFile(fileName);
    nlohmann::json botConfig;
    inFile >> botConfig;

    if (!botConfig["discord_token"].is_null())
    {   discordToken = botConfig["discord_token"];
    }
    if (!botConfig["admin_id"].is_null())
    {   adminID = botConfig["admin_id"];
    }
    if (!botConfig["coin_ticker"].is_null())
    {   coinTicker = botConfig["coin_ticker"];
    }
    if (!botConfig["redisPrefix"].is_null())
    {   redisPrefix = botConfig["redisPrefix"];
    }
    if (!botConfig["coin_unit"].is_null())
    {   coinUnit = botConfig["coin_unit"];
    }
    if (!botConfig["displayed_decimal"].is_null())
    {   displayedDecimal = botConfig["displayed_decimal"];
    }
    if (!botConfig["mixin"].is_null())
    {   mixin = botConfig["mixin"];
    }
    if (!botConfig["transfer_fee"].is_null())
    {   transferFee = botConfig["transfer_fee"];
    }
    if (!botConfig["wallet_rpc_port"].is_null())
    {   walletRpcPort = botConfig["wallet_rpc_port"];
    }
    if (!botConfig["redis_rpc_port"].is_null())
    {   redisRpcPort = botConfig["redis_rpc_port"];
    }
    if (!botConfig["faucet_channel"].is_null())
    {   faucetChannel = botConfig["faucet_channel"];
    }
    if (!botConfig["faucet_min_reward"].is_null())
    {   faucetMinReward = botConfig["faucet_min_reward"];
    }
    if (!botConfig["faucet_max_reward"].is_null())
    {   faucetMaxReward = botConfig["faucet_max_reward"];
    }
    if (!botConfig["faucet_time_inteval"].is_null())
    {   faucetTimeInteval = botConfig["faucet_time_inteval"];
    }
}