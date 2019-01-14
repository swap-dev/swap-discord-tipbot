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

#include "walletapi.h"

#include "curl_easy.h"
#include "curl_exception.h"

int64_t WalletApi::getBlockHeight()
{
    std::ostringstream str;
    curl::curl_ios<std::ostringstream> writer(str);
    curl::curl_easy easy(writer);

    nlohmann::json httpPost;
    httpPost["method"] = "get_height";

    easy.add(curl::curl_pair<CURLoption,std::string>(CURLOPT_URL, mWalletJsonHttp));
    easy.add(curl::curl_pair<CURLoption,curl::curl_header>(CURLOPT_HTTPHEADER, mHeader));
    easy.add<CURLOPT_SSL_VERIFYPEER>(false);
    easy.add(curl::curl_pair<CURLoption,std::string>(CURLOPT_POSTFIELDS,httpPost.dump()));

    try
    {
        easy.perform();
    }
    catch (curl::curl_easy_exception error)
    {
        curl::curlcpp_traceback errors = error.get_traceback();
        error.print_traceback();
        return 0;
    }

    nlohmann::json httpReponse;
    httpReponse = nlohmann::json::parse(str.str());
    return httpReponse["result"]["height"];
}

WalletApi::Wallet WalletApi::createNewWallet(std::string label)
{
    std::ostringstream str;
    curl::curl_ios<std::ostringstream> writer(str);
    curl::curl_easy easy(writer);

    nlohmann::json httpPost;
    httpPost["method"] = "create_address";
    httpPost["params"]["account_index"] = 0;
    httpPost["params"]["label"] = label;

    easy.add(curl::curl_pair<CURLoption,std::string>(CURLOPT_URL, mWalletJsonHttp));
    easy.add(curl::curl_pair<CURLoption,curl::curl_header>(CURLOPT_HTTPHEADER, mHeader));
    easy.add<CURLOPT_SSL_VERIFYPEER>(false);
    easy.add(curl::curl_pair<CURLoption,std::string>(CURLOPT_POSTFIELDS,httpPost.dump()));

    Wallet wallet;

    try
    {
        easy.perform();
    }
    catch (curl::curl_easy_exception error)
    {
        curl::curlcpp_traceback errors = error.get_traceback();
        error.print_traceback();
        return wallet;
    }

    nlohmann::json httpReponse;
    httpReponse = nlohmann::json::parse(str.str());

    wallet.address = httpReponse["result"]["address"];
    wallet.index = httpReponse["result"]["address_index"];
    return wallet;
}

int64_t WalletApi::getUnlockedBalance()
{
    std::ostringstream str;
    curl::curl_ios<std::ostringstream> writer(str);
    curl::curl_easy easy(writer);

    nlohmann::json httpPost;
    httpPost["method"] = "get_balance";

    easy.add(curl::curl_pair<CURLoption,std::string>(CURLOPT_URL, mWalletJsonHttp));
    easy.add(curl::curl_pair<CURLoption,curl::curl_header>(CURLOPT_HTTPHEADER, mHeader));
    easy.add<CURLOPT_SSL_VERIFYPEER>(false);
    easy.add(curl::curl_pair<CURLoption,std::string>(CURLOPT_POSTFIELDS,httpPost.dump()));

    try
    {
        easy.perform();
    }
    catch (curl::curl_easy_exception error)
    {
        curl::curlcpp_traceback errors = error.get_traceback();
        error.print_traceback();
    }

    nlohmann::json httpReponse;
    httpReponse = nlohmann::json::parse(str.str());
    return httpReponse["result"]["unlocked_balance"];
}


int64_t WalletApi::getUnlockedDeposit(int64_t subaddress_index)
{
    std::ostringstream str;
    curl::curl_ios<std::ostringstream> writer(str);
    curl::curl_easy easy(writer);

    nlohmann::json httpPost;
    httpPost["method"] = "get_transfers";
    httpPost["params"]["in"] = true;
    httpPost["params"]["failed"] = false;
    httpPost["params"]["pending"] = false;
    httpPost["params"]["pool"] = false;
    httpPost["params"]["subaddr_indices"] = {subaddress_index};

    easy.add(curl::curl_pair<CURLoption,std::string>(CURLOPT_URL, mWalletJsonHttp));
    easy.add(curl::curl_pair<CURLoption,curl::curl_header>(CURLOPT_HTTPHEADER, mHeader));
    easy.add<CURLOPT_SSL_VERIFYPEER>(false);
    easy.add(curl::curl_pair<CURLoption,std::string>(CURLOPT_POSTFIELDS,httpPost.dump()));
    
    try
    {
        easy.perform();
    }
    catch (curl::curl_easy_exception error)
    {
        curl::curlcpp_traceback errors = error.get_traceback();
        error.print_traceback();
    }

    nlohmann::json httpReponse;
    httpReponse = nlohmann::json::parse(str.str());
    int64_t totalDeposit = 0;

    for (const auto& element: httpReponse["result"]["in"])
    {
        totalDeposit += (int64_t)element["amount"];
    }

    return totalDeposit;
}

int64_t WalletApi::getDeposit(int64_t subaddress_index)
{
    std::ostringstream str;
    curl::curl_ios<std::ostringstream> writer(str);
    curl::curl_easy easy(writer);

    nlohmann::json httpPost;
    httpPost["method"] = "get_transfers";
    httpPost["params"]["in"] = true;
    httpPost["params"]["failed"] = false;
    httpPost["params"]["pending"] = true;
    httpPost["params"]["pool"] = true;
    httpPost["params"]["subaddr_indices"] = {subaddress_index};

    easy.add(curl::curl_pair<CURLoption,std::string>(CURLOPT_URL, mWalletJsonHttp));
    easy.add(curl::curl_pair<CURLoption,curl::curl_header>(CURLOPT_HTTPHEADER, mHeader));
    easy.add<CURLOPT_SSL_VERIFYPEER>(false);
    easy.add(curl::curl_pair<CURLoption,std::string>(CURLOPT_POSTFIELDS,httpPost.dump()));

    try
    {
        easy.perform();
    }
    catch (curl::curl_easy_exception error)
    {
        curl::curlcpp_traceback errors = error.get_traceback();
        error.print_traceback();
    }

    nlohmann::json httpReponse;
    httpReponse = nlohmann::json::parse(str.str());
    int64_t totalPendingDeposit = 0;

    for (const auto& element: httpReponse["result"]["in"])
    {
        totalPendingDeposit += (int64_t)element["amount"];
    }

    return totalPendingDeposit;
}

WalletApi::WithdrawlReceipt WalletApi::withdrawToWallet (std::string walletAddress, int64_t amount, uint8_t mixin)
{
    std::ostringstream str;
    curl::curl_ios<std::ostringstream> writer(str);
    curl::curl_easy easy(writer);

    nlohmann::json destination;
    destination["address"] = walletAddress;
    destination["amount"] = amount;

    nlohmann::json httpPost;

    httpPost["method"] = "transfer";
    httpPost["params"]["destinations"] = {destination};
    httpPost["params"]["mixin"] = mixin;
    httpPost["params"]["priority"] = 1;

    easy.add(curl::curl_pair<CURLoption,std::string>(CURLOPT_URL, mWalletJsonHttp));
    easy.add(curl::curl_pair<CURLoption,curl::curl_header>(CURLOPT_HTTPHEADER, mHeader));
    easy.add<CURLOPT_SSL_VERIFYPEER>(false);
    easy.add(curl::curl_pair<CURLoption,std::string>(CURLOPT_POSTFIELDS,httpPost.dump()));

    try
    {
        easy.perform();
    }
    catch (curl::curl_easy_exception error)
    {
        curl::curlcpp_traceback errors = error.get_traceback();
        error.print_traceback();
    }

    nlohmann::json httpReponse;
    httpReponse = nlohmann::json::parse(str.str());

    WalletApi::WithdrawlReceipt receipt;

    if (!httpReponse["result"]["tx_hash"].is_null())
    {
        receipt.txHash = httpReponse["result"]["tx_hash"];
        receipt.txFee = httpReponse["result"]["fee"];
        receipt.isSuccess = true;
    }

    return receipt;
}

void WalletApi::setPortNumber(uint16_t portNumber)
{
    mPortNumber = portNumber;
    mWalletJsonHttp = "http://127.0.0.1:" + std::to_string(portNumber) + "/json_rpc";
    mHeader.add("Content-Type: application/json");
}