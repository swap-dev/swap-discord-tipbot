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

#ifndef SWAPBOT_WALLETNAPI_H
#define SWAPBOT_WALLETAPI_H

#include <string>
#include "curl_header.h"
#include "json.hpp"


class WalletApi
{
public:
    struct Wallet
    {
        std::string address {""};
        int64_t index {0};
    };

    struct WithdrawlReceipt
    {
        bool isSuccess {false};
        std::string txHash {""};
        int64_t txFee {0};
    };

    int64_t getBlockHeight();
    WalletApi::Wallet createNewWallet(std::string label);
    int64_t getUnlockedBalance();
    int64_t getUnlockedDeposit(int64_t subaddress_index);
    int64_t getDeposit(int64_t subaddress_index);
    WalletApi::WithdrawlReceipt withdrawToWallet (std::string walletAddress, int64_t amount, uint8_t mixin);

    void setPortNumber(uint16_t portNumber);

private:
    uint16_t mPortNumber;
    std::string mWalletJsonHttp;
    curl::curl_header mHeader;
};

#endif // SWAPBOT_WALLETAPI_H