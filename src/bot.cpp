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

#include <iostream>
#include <sstream>
#include "json.hpp"

void Bot::setSettings(const Config& botConfig)
{
    mBotConfig = botConfig;
    mWalletApi.setPortNumber(mBotConfig.walletRpcPort);
    mRedisConnection.connect("localhost", mBotConfig.redisRpcPort);
    mRedisPrefix = "TIPBOT:" + mBotConfig.coinTicker + ":";
    redox::Command<std::string>& faucetBalance = mRedisConnection.commandSync<std::string>({"HGET", mRedisPrefix + "faucet", "faucet_balance"});

    if(!faucetBalance.ok())
    {
        std::string reply;
        std::cout << "Faucet balance is currently undefined. Set faucet balance to 0 (y/n)?";
        std::cin >> reply;

        if (reply == "y" || reply == "Y")
        {   mRedisConnection.commandSync<std::string>({"HSET", mRedisPrefix + "faucet", "faucet_balance", "0"});
        }
        else
        {   exit(0);
        }
    }

    faucetBalance.free();
}

void Bot::onMessage(SleepyDiscord::Message message)
{
    std::string userID = message.author.ID;

    if (ensureRedisUser(userID))
    {
        if (message.startsWith(".myaddress"))
        {
            std::string userAddress = mRedisConnection.commandSync<std::string>({"HGET", mRedisPrefix + userID, "address"}).reply();
            sendMessage(message.channelID, message.author.username + ", your TIPBOT deposit address is: " + userAddress);
        }
        else if (message.startsWith(".balance"))
        {
            //int64_t totalBalance = getTotalBalance(userID);
            int64_t unlockedBalance = getUnlockedBalance(userID);

            sendMessage(message.channelID, message.author.username + ", you have " + formatCoin(unlockedBalance, false) + " in your TIPBOT wallet.");
        }
        else if (message.startsWith(".blockheight"))
        {
            sendMessage(message.channelID, "Current wallet height is " + std::to_string(mWalletApi.getBlockHeight()));
        }
        else if (message.startsWith(".withdrawall") || message.startsWith(".sweep_all"))
        {
            int64_t amount = getUnlockedBalance(userID);

            std::vector<std::string> messageArg = splitString(message.content);
            if (messageArg.size() > 1)
            {
                std::string address = messageArg.at(1);

                if (amount > 0)
                {
                    if (getFaucetBalance() > mBotConfig.transferFee)
                    {
                        WalletApi::WithdrawlReceipt receipt;
                        receipt = mWalletApi.withdrawToWallet(address, amount, mBotConfig.mixin);

                        if (receipt.isSuccess)
                        {
                            addUserBalance (userID, -amount);
                            addFaucetBalance (-receipt.txFee);

                            sendMessage(message.channelID, message.author.username + ", " + formatCoin(amount, false)+ " have been withdrawn from your TIPBOT wallet. txhash: <" + receipt.txHash + ">\\n"
                                "(" + formatCoin(receipt.txFee) + " have been deducted from faucet to pay the tx fees)");
                        }
                        else
                        {
                            if (mWalletApi.getUnlockedBalance() < amount)
                            {   sendMessage(message.channelID, "Error sending transaction. Not enough unlocked balance in shared TIPBOT wallet. Try again in a few minute.");
                            }
                            else
                            {   sendMessage(message.channelID, "Error sending transaction. Check your address or try sending a smaller amount.");
                            }
                        }
                    }
                    else
                    {
                        sendMessage(message.channelID, "Faucet balance is not sufficient to pay the transaction fees.\\n"
                            "(A reserve of " + formatCoin(mBotConfig.transferFee) + " is required to cover any transaction cost)");
                    }
                }
                else
                {
                    sendMessage(message.channelID, message.author.username + ", you have no coins.");
                }
            }
            else
            {
                sendMessage(message.channelID, "Formatting error. The correct format is: .sweep_all [address].");
            }
        }
        else if (message.startsWith(".withdraw") || message.startsWith(".transfer"))
        {
            int64_t unlockedBalance = getUnlockedBalance(userID);

            std::vector<std::string> messageArg = splitString(message.content);
            if (messageArg.size() > 2)
            {
                std::string address = messageArg.at(1);
                int64_t amount = std::stold(messageArg.at(2)) * pow(10, mBotConfig.coinUnit);

                if (amount > 0 && address.length() > 0)
                {
                    if (unlockedBalance >= amount)
                    {
                        if (getFaucetBalance() > mBotConfig.transferFee)
                        {
                            WalletApi::WithdrawlReceipt receipt;
                            receipt = mWalletApi.withdrawToWallet(address, amount , mBotConfig.mixin);

                            if (receipt.isSuccess)
                            {
                                addUserBalance (userID, -amount);
                                addFaucetBalance (-receipt.txFee);

                                sendMessage(message.channelID, message.author.username + ", " + formatCoin(amount, false)+ " have been withdrawn from your TIPBOT wallet. txhash: <" + receipt.txHash + ">\\n"
                                    "(" + formatCoin(receipt.txFee) + " have been deducted from faucet to pay the tx fees)");
                            }
                            else
                            {
                                if (mWalletApi.getUnlockedBalance() < amount)
                                {   sendMessage(message.channelID, "Error sending transaction. Not enough unlocked balance in shared TIPBOT wallet. Try again in a few minute.");
                                }
                                else
                                {   sendMessage(message.channelID, "Error sending transaction. Check your address or try sending a smaller amount.");
                                }
                            }
                        }
                        else
                        {
                            sendMessage(message.channelID, "Faucet balance is not sufficient to pay the transaction fees.\\n"
                                "(A reserve of " + formatCoin(mBotConfig.transferFee) + " is required)");
                        }
                    }
                    else
                    {
                        sendMessage(message.channelID, message.author.username + ", you do not have the sufficient balance to perform that task.");
                    }
                }
                else
                {
                    sendMessage(message.channelID, "Formatting error. The correct format is: .transfer [address] [amount].");
                }
            }
            else
            {
                sendMessage(message.channelID, "Formatting error. The correct format is: .transfer [address] [amount].");
            }
        }
        else if (message.startsWith(".giveall") || message.startsWith(".tipall"))
        {
            int64_t amount = getUnlockedBalance(userID);

            if (message.mentions.size() > 0)
            {
                std::string targetUserID = message.mentions[0].ID;
                if (ensureRedisUser(targetUserID))
                {
                    addUserBalance(userID, -amount);
                    addUserBalance(targetUserID, amount);
                    sendMessage(message.channelID, message.mentions[0].username + " was tipped " + formatCoin(amount) + ".");
                }
                else
                {
                    sendMessage(message.channelID, "Error connecting to Redis.");
                }
            }
            else
            {
                sendMessage(message.channelID, "Formatting error. The correct format is: .tipall [@username].");
            }
        }
        else if (message.startsWith(".give") || message.startsWith(".tip") )
        {
            std::vector<std::string> messageArg = splitString(message.content);
            if (messageArg.size() > 2 && sizeof(message.mentions) > 0)
            {
                int64_t amount = std::stold(messageArg.back()) * pow (10, mBotConfig.coinUnit);
                int64_t unlockedBalance = getUnlockedBalance(userID);

                if (unlockedBalance >= amount * message.mentions.size())
                {
                    int64_t totalAmount {0};
                    std::string targetDisplayString{""};

                    for (uint8_t i = 0; i < message.mentions.size(); i++)
                    {
                        std::string targetUserID = message.mentions[i].ID;
                        if (ensureRedisUser(targetUserID))
                        {
                            addUserBalance(userID, -amount);
                            addUserBalance(targetUserID, amount);
                            totalAmount += amount;

                            if (i < message.mentions.size() - 1 && message.mentions.size() == 2)
                            {   targetDisplayString += message.mentions[i].username + " ";
                            }
                            else if (i < message.mentions.size() - 1)
                            {   targetDisplayString += message.mentions[i].username + ", ";
                            }
                            else
                            {   targetDisplayString += "and " + message.mentions[i].username;
                            }
                        }
                        else
                        {
                            sendMessage(message.channelID, "Error connecting to Redis.");
                        }
                    }

                    if (message.mentions.size() == 1)
                    {   sendMessage(message.channelID, message.mentions[0].username + " was tipped " + formatCoin(amount) + ".");
                    }
                    else
                    {   sendMessage(message.channelID, targetDisplayString + " were tipped " + formatCoin(amount) + " each, totalling " + formatCoin(totalAmount) + ".");
                    }
                }
                else
                {
                    sendMessage(message.channelID, message.author.username + ", you do not have the sufficient balance to perform that task.");
                }
            }
            else
            {
                sendMessage(message.channelID, "Formatting error. The correct format is: .tip [@username] [amount].");
            }
        }
        else if (message.startsWith(".faucet"))
        {
            sendMessage(message.channelID, "Current faucet balance is: " + formatCoin(getFaucetBalance()));
        }
        else if (message.startsWith(".donateall"))
        {
            int64_t amount = getUnlockedBalance(userID);

            if (amount > 0)
            {
                addUserBalance(userID, -amount);
                addFaucetBalance(amount);

                sendMessage(message.channelID, message.author.username + ", you've donated " + formatCoin(amount) + " to the faucet\\n(faucet balance increased to " + formatCoin(getFaucetBalance()) + ")");
            }
            else
            {
                sendMessage(message.channelID, message.author.username + ", you don't have anything to donate.");
            }
        }
        else if (message.startsWith(".donate"))
        {
            std::vector<std::string> messageArg = splitString(message.content);
            int64_t amount = std::stold(messageArg.back()) * pow (10, mBotConfig.coinUnit);

            int64_t unlockedBalance = getUnlockedBalance(userID);

            if (unlockedBalance >= amount)
            {
                addUserBalance(userID, -amount);
                addFaucetBalance(amount);

                sendMessage(message.channelID, message.author.username + ", you've donated " + formatCoin(amount) + " to the faucet.\\n(faucet balance increased to " + formatCoin(getFaucetBalance()) + ")");
            }
            else
            {
                sendMessage(message.channelID, message.author.username + ", you do not have the sufficient balance to perform that task.");
            }
        }
        else if (message.startsWith(".take"))
        {
            if (message.channelID == mBotConfig.faucetChannel)
            {
                int64_t userLastFaucetTake = std::stoll(mRedisConnection.commandSync<std::string>({"HGET", mRedisPrefix + userID, "lastFaucetTake"}).reply());
                int64_t currentHeight = mWalletApi.getBlockHeight();

                if (currentHeight >= mBotConfig.faucetTimeInteval + userLastFaucetTake)
                {
                    std::random_device rd;
                    std::default_random_engine generator(rd());
                    std::uniform_int_distribution<long long unsigned> distribution(0,0xFFFFFFFFFFFFFFFF);
                    int64_t amount = distribution(generator) % (mBotConfig.faucetMaxReward - mBotConfig.faucetMinReward) + mBotConfig.faucetMinReward;
                    int64_t faucetBalance = getFaucetBalance();

                    if (amount > faucetBalance)
                    {
                        amount = faucetBalance;
                    }

                    if (amount == 0)
                    {   sendMessage(message.channelID, "Sorry, faucet is out of coins to give out.");
                    }
                    else if (amount < 0)
                    {   sendMessage(message.channelID, "Sorry, something went horribly wrong, and the faucet is bankrupt :scream:");
                    }

                    //Automatically reduce reward when balance is low
                    if (faucetBalance < 80 * pow(10, mBotConfig.coinUnit))
                    {   amount = amount / 2;
                    }
                    if (faucetBalance < 40 * pow(10, mBotConfig.coinUnit))
                    {   amount = amount / 2;
                    }
                    if (faucetBalance < 20 * pow(10, mBotConfig.coinUnit))
                    {   amount = amount / 2;
                    }
                    if (faucetBalance < 10 * pow(10, mBotConfig.coinUnit))
                    {   amount = amount / 2;
                    }

                    addFaucetBalance(-amount);
                    addUserBalance(userID, amount);
                    mRedisConnection.commandSync<std::string>({"HSET", mRedisPrefix + userID, "lastFaucetTake", std::to_string(currentHeight)});
                    sendMessage(message.channelID, message.author.username + ", you have been granted " + formatCoin(amount) + ".\\n(faucet balance reduced to " + formatCoin(getFaucetBalance()) + ")");
                }
                else
                {
                    int64_t delay = mBotConfig.faucetTimeInteval + userLastFaucetTake - currentHeight;
                    sendMessage(message.channelID, "Too soon " + message.author.username + "! You need to wait " + std::to_string(delay) +
                        " more blocks (approx. " + std::to_string((delay >> 2) / 60) + "h" + std::to_string((delay >> 2) % 60) + "m)");
                }
            }
            else
            {
                sendMessage(message.channelID, "Please use #botspam-faucet for this command");
            }
        }
        else if (message.startsWith(".help"))
        {
            std::string helpText
            {
                "Swap TipBot Commands:\\n"
                "```"
                ".myaddress: display my deposit address.\\n"
                ".balance: dispay my balance.\\n"
                ".blockheight: display current wallet height.\\n"
                ".transfer [address] [amount]: transfer amount from my account.\\n"
                ".sweep_all [address]: transfer everything from my account.\\n"
                ".tip [@username] <@username2 @username3...> [amount]: tip amount to each @username(s).\\n"
                ".tipall [@username]: tip everything to @username.\\n"
                ".faucet: show amount in faucet.\\n"
                ".donate [amount]: donate amount to faucet.\\n"
                ".donateall: donate everything to faucet.\\n"
                ".take: collect a random amount of coins from faucet (#botspam-faucet only).\\n"

                ".help: display this message.\\n"
                "```"
            };
            sendMessage(message.channelID, helpText);
        }
        else if (message.startsWith(".sudo"))
        {
            if (userID == mBotConfig.adminID && message.content.npos > 6)
            {   sendMessage(message.channelID, message.content.substr (6, message.content.npos));
            }
            else if (message.content.npos <= 6)
            {   sendMessage(message.channelID, "Sudo what?");
            }
            else
            {   sendMessage(message.channelID, "You're not my master!");
            }
        }
        else if (message.startsWith(".release"))
        {
            if (userID == mBotConfig.adminID)
            {
                std::vector<std::string> messageArg = splitString(message.content);
                int64_t amount = std::stold(messageArg.back()) * pow (10, mBotConfig.coinUnit);
                
                if (amount < getFaucetBalance())
                {
                    addFaucetBalance(-amount);
                    addUserBalance(userID, amount);
                    sendMessage(message.channelID, message.author.username + ", " + formatCoin(amount) + " have been withdrawn from faucet.\\n(faucet balance reduced to " + formatCoin(getFaucetBalance()) + ")");
                }
                else
                {
                    sendMessage(message.channelID, "Not enough coins in faucet. " + formatCoin(getFaucetBalance()) + " is currently available.");
                }
            }
            else
            {
                sendMessage(message.channelID, "Admin privledge is required for this command.");
            }
        }
    }
    else
    {
        sendMessage(message.channelID, "Error connecting to Redis");
    }
}

bool Bot::ensureRedisUser (std::string userID)
{
    redox::Command<std::string>& redisCommand = mRedisConnection.commandSync<std::string>({"HGET", mRedisPrefix + userID, "index"});

    if(!redisCommand.ok())
    {
        WalletApi::Wallet wallet  = mWalletApi.createNewWallet(userID);

        if (wallet.index == 0)
        {
            return false;
        }

        return createNewRedisUser(userID, wallet);
    }

    return true;
}

bool Bot::createNewRedisUser(const std::string& userID, const WalletApi::Wallet& wallet)
{
    try
    {
        mRedisConnection.commandSync<std::string>({"HSET", mRedisPrefix + userID, "address", wallet.address});
        mRedisConnection.commandSync<std::string>({"HSET", mRedisPrefix + userID, "index", std::to_string(wallet.index)});
        mRedisConnection.commandSync<std::string>({"HSET", mRedisPrefix + userID, "offchain_tx", "0"});
        mRedisConnection.commandSync<std::string>({"HSET", mRedisPrefix + userID, "withdrawl", "0"});
        mRedisConnection.commandSync<std::string>({"HSET", mRedisPrefix + userID, "lastFaucetTake", "0"});
    }
    catch(int e)
    {
        std::cout << "Error creating new user: " << e;
        return false;
    }

    return true;
}

std::string Bot::formatCoin (int64_t coin, bool isRounded)
{
    std::stringstream ss;
    if (isRounded)
    {
      ss << std::fixed << std::setprecision(mBotConfig.displayedDecimal) << (coin  /  pow(10, mBotConfig.coinUnit));
    }
    else
    {
      ss << std::fixed << std::setprecision(mBotConfig.coinUnit) << (coin  /  pow(10, mBotConfig.coinUnit));
    }
    return ss.str() + " " + mBotConfig.coinTicker;
}

std::vector<std::string> Bot::splitString(std::string text)
{
    std::istringstream iss(text);
    std::vector<std::string> results((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
    return results;
}

int64_t Bot::getFaucetBalance ()
{
    return std::stoll(mRedisConnection.commandSync<std::string>({"HGET", mRedisPrefix + "faucet", "faucet_balance"}).reply());
}

int64_t Bot::addFaucetBalance (int64_t coinAdded)
{
    int64_t faucetBalance = std::stoll(mRedisConnection.commandSync<std::string>({"HGET", mRedisPrefix + "faucet", "faucet_balance"}).reply());
    int64_t newBalance = faucetBalance + coinAdded;
    mRedisConnection.commandSync<std::string>({"HSET", mRedisPrefix + "faucet", "faucet_balance", std::to_string(newBalance)});
    return newBalance;
}

int64_t Bot::getUnlockedBalance(std::string userID)
{
    int64_t walletIndex = std::stoll(mRedisConnection.commandSync<std::string>({"HGET", mRedisPrefix + userID, "index"}).reply());
    int64_t unlockedDeposit = mWalletApi.getUnlockedDeposit(walletIndex);
    int64_t offchainTx = std::stoll(mRedisConnection.commandSync<std::string>({"HGET", mRedisPrefix + userID, "offchain_tx"}).reply());
    int64_t withdrawl = std::stoll(mRedisConnection.commandSync<std::string>({"HGET", mRedisPrefix + userID, "withdrawl"}).reply());
    return unlockedDeposit + offchainTx - withdrawl;
}

int64_t Bot::getTotalBalance(std::string userID)
{
    int64_t walletIndex = std::stoll(mRedisConnection.commandSync<std::string>({"HGET", mRedisPrefix + userID, "index"}).reply());
    int64_t deposit = mWalletApi.getDeposit(walletIndex);
    int64_t offchainTx = std::stoll(mRedisConnection.commandSync<std::string>({"HGET", mRedisPrefix + userID, "offchain_tx"}).reply());
    int64_t withdrawl = std::stoll(mRedisConnection.commandSync<std::string>({"HGET", mRedisPrefix + userID, "withdrawl"}).reply());
    return deposit + offchainTx - withdrawl;
}

int64_t Bot::addUserBalance (std::string userID, int64_t coinAdded)
{
    int64_t offchainTx = std::stoll(mRedisConnection.commandSync<std::string>({"HGET", mRedisPrefix + userID, "offchain_tx"}).reply());
    int64_t newBalance = offchainTx + coinAdded;
    mRedisConnection.commandSync<std::string>({"HSET", mRedisPrefix + userID, "offchain_tx", std::to_string(newBalance)});
    return newBalance;
}
