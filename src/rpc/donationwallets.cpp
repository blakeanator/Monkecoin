// Copyright (c) 2021 Blake Copeland
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <rpc/donationwallets.h>

#include <chainparams.h>
#include <donationwallets.h>
#include <rpc/server.h>
#include <rpc/util.h>
#include <validation.h>

#include <univalue.h>

// TEST
#include <sidh.h>
#include <crypto/sha3.h>
// TEST

static UniValue getblockdonation(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() > 1)
        throw std::runtime_error(
            RPCHelpMan{"getblockdonation",
                "\nReturns how much currency gets minted for donation wallets for a given block.\n",
                {
                    {"height", RPCArg::Type::NUM, /* default */ "1", "Specifies the block to get the donation reward for."},
                },
                RPCResult{
                    RPCResult::Type::NUM, "donation", "How much Monkecoin is distributed to all donations wallets for a given block"
                },
                RPCExamples{
                    HelpExampleCli("getblockdonation", "1000")
            + HelpExampleRpc("getblockdonation", "1000")
                },
            }.ToString());

    int nHeight = !request.params[0].isNull() ? request.params[0].get_int() : 1;
    
    if (nHeight < 0)
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Block height out of range");

    std::string result = std::to_string((double)GetBlockDonationSubsidy(nHeight, Params().GetConsensus()) / (double)COIN) + " MKE";

    return UniValue(result);
}

static UniValue getblockreward(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() > 1)
        throw std::runtime_error(
            RPCHelpMan{"getblockreward",
                "\nReturns the reward for mining a given block.\n",
                {
                    {"height", RPCArg::Type::NUM, /* default */ "1", "Specifies the block to get the reward value for."},
                },
                RPCResult{
                    RPCResult::Type::NUM, "reward", "How many Monkecoin are given to the miner as a reward for completing a given block"
                },
                RPCExamples{
                    HelpExampleCli("getblockreward", "1000")
            + HelpExampleRpc("getblockreward", "1000")
                },
            }.ToString());

    int nHeight = !request.params[0].isNull() ? request.params[0].get_int() : 1;

    if (nHeight < 0)
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Block height out of range");

    std::string result = std::to_string(GetBlockSubsidy(nHeight, Params().GetConsensus()) / COIN) + " MKE";

    return UniValue(result);
}

static UniValue getblockrewardhalving(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() != 1)
        throw std::runtime_error(
            RPCHelpMan{"getblockrewardhalving",
                "\nReturns the reward for mining a given block.\n",
                {
                    {"height", RPCArg::Type::NUM, RPCArg::Optional::NO, "Specifies the block to get the halving value for."},
                },
                RPCResult{
                    RPCResult::Type::NUM, "halving", "How many times the inital reward is halved for a given block"
                },
                RPCExamples{
                    HelpExampleCli("getblockrewardhalving", "1000")
            + HelpExampleRpc("getblockrewardhalving", "1000")
                },
            }.ToString());

    int nHeight = !request.params[0].isNull() ? request.params[0].get_int() : 1;

    if (nHeight < 0 || nHeight > (int)GetMaxMinableBlocks(Params().GetConsensus()))
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Block height out of range");

    return GetBlockSubsidyHalving(nHeight, Params().GetConsensus());
}

static UniValue getblockrewardhalvingmax(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() != 0)
        throw std::runtime_error(
            RPCHelpMan{"getblockrewardhalvingmax",
                "\nReturns how many times the mining reward will be halved.\n",
                {},
                RPCResult{
                    RPCResult::Type::NUM, "steps", "The total number of halving steps in the lifetime of the minting process"
                },
                RPCExamples{
                    HelpExampleCli("getblockrewardhalvingmax", "")
            + HelpExampleRpc("getblockrewardhalvingmax", "")
                },
            }.ToString());


    return GetBlockSubsidyHalvingMax(Params().GetConsensus());
}

static UniValue getcelebrationblock(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() != 0)
        throw std::runtime_error(
            RPCHelpMan{"getcelebrationblock",
                "\nReturns the celebration block. This block is the block after the last minable block, and rewards _ MKE to donation wallets.\n",
                {},
                RPCResult{
                    RPCResult::Type::NUM, "block", "The block number for the Celebration Block (if there is one)"
                },
                RPCExamples{
                    HelpExampleCli("getcelebrationblock", "")
            + HelpExampleRpc("getcelebrationblock", "")
                },
            }.ToString());

    // If the celebration block is enabled
    if (ENABLE_CELEBRATION_BLOCK)
    {
        return GetCelebrationBlock(Params().GetConsensus());
    }

    return UniValue("Celebration block not enabled");
}

static UniValue getdonationwallets(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() != 0) {
        throw std::runtime_error(
            RPCHelpMan{"getdonationwallets",
                "\nReturns information on all the donation wallets.\n",
                {},
                RPCResult {
                    RPCResult::Type::ARR, "", "",
                    {
                        {RPCResult::Type::OBJ, "", "",
                        {
                            {RPCResult::Type::STR, "name", "The name of the non-profit associated with the wallet"},
                            {RPCResult::Type::STR, "address", "The current address for the nonprofit"},
                            {RPCResult::Type::BOOL, "active", "If the non-profit is active or not"},
                            {RPCResult::Type::NUM, "confirmations", "The number of confirmations of the most recent transaction included"},
                        }},
                    }
                },
                RPCExamples{
                    HelpExampleCli("getdonationwallets", "")
            + HelpExampleRpc("getdonationwallets", "")
                },
            }.ToString());
    }

    UniValue result(UniValue::VARR);

    // Go through all the donation wallets (skip the first index)
    for (unsigned int i = 1; i < DonationWallets::GetSize(); i++)
    {
        const DonationWalletDescriptor& wallet = donationWallets[i];

        UniValue obj(UniValue::VOBJ);
        obj.pushKV("name", wallet.name);
        //obj.pushKV("address", wallet.pubkey);
        obj.pushKV("address", "");
        obj.pushKV("active", wallet.active);
        result.push_back(obj);
     }

    return result;
}

static UniValue getmaxminableblocks(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() != 0)
        throw std::runtime_error(
            RPCHelpMan{"getmaxminableblocks",
                "\nReturns the maximum number of blocks that can be mined for a reward.\n",
                {},
                RPCResult{
                    RPCResult::Type::NUM, "height", "Last block that has a mining reward"
                },
                RPCExamples{
                    HelpExampleCli("getmaxminableblocks", "")
            + HelpExampleRpc("getmaxminableblocks", "")
                },
            }.ToString());


    return GetMaxMinableBlocks(Params().GetConsensus());
}

static UniValue getsupplyratio(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() != 0)
        throw std::runtime_error(
            RPCHelpMan{"getsupplyratio",
                "\nReturns supply ratio for how much of the total currency supply is allocated to minres vs donations.\n",
                {},
                RPCResult{
                    RPCResult::Type::STR, "ratio", "The supply distribution percentage between miners and non-profits"
                },
                RPCExamples{
                    HelpExampleCli("getsupplyratio", "")
            + HelpExampleRpc("getsupplyratio", "")
                },
            }.ToString());

    std::string result = "Miners:    " + std::to_string(GetTotalMiningSubsidyPercentage(Params().GetConsensus()) * 100.0) + "%\nDonations:  " + std::to_string(GetTotalDonationSubsidyPercentage(Params().GetConsensus()) * 100.0) + "%";

    return UniValue(result);
}

static UniValue getsupplytotal(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() != 0)
        throw std::runtime_error(
            RPCHelpMan{"getsupplytotal",
                "\nReturns the entire supply of Monkecoins.\n",
                {},
                RPCResult{
                    RPCResult::Type::STR, "distribution", "The supply distribution between miners and non-profits"
                },
                RPCExamples{
                    HelpExampleCli("getsupplytotal", "")
            + HelpExampleRpc("getsupplytotal", "")
                },
            }.ToString());

    std::string result = "Total:     " + std::to_string(MAX_MONEY / COIN) + " MKE\n-----------------------\nMiners:    " + std::to_string(GetTotalMiningSubsidySupply(Params().GetConsensus()) / COIN) + " MKE\nDonations:  " + std::to_string(GetTotalDonationSubsidySupply(Params().GetConsensus()) / COIN) + " MKE";

    return UniValue(result);
}

static UniValue donationtest(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() != 0)
        throw std::runtime_error(
            RPCHelpMan{"donationtest",
                "\ntest.\n",
                {},
                RPCResult{
                    RPCResult::Type::STR, "test", "test"
                },
                RPCExamples{
                    HelpExampleCli("donationtest", "")
            + HelpExampleRpc("donationtest", "")
                },
            }.ToString());

    uint8_t privateKey[374] = "";
    uint8_t publicKey[330]=  "";
    //unsigned char pk[330]=  "";
    //unsigned char pk[374] = "";
    int i = crypto_kem_keypair_SIKEp434(publicKey, privateKey);

    //std::vector<uint8_t> prikv(privateKey, privateKey + 374);
    //std::string prikvs = HexStr(prikv);
    //std::string prikvs = HexStr(Span<uint8_t>(privateKey, privateKey + 374));

    std::vector<uint8_t> pubkv(publicKey, publicKey + 330);
    std::string pubkvs = HexStr(pubkv);

    std::string result = "SIDH (" + std::to_string(i) + ")\nprivate: " + HexStr(MakeUCharSpan(privateKey)) + "\npublic: " + pubkvs;

    return UniValue(result);
}

static UniValue donationtest2(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() != 0)
        throw std::runtime_error(
            RPCHelpMan{"donationtest2",
                "\ntest2.\n",
                {},
                RPCResult{
                    RPCResult::Type::STR, "test2", "test2"
                },
                RPCExamples{
                    HelpExampleCli("donationtest2", "")
            + HelpExampleRpc("donationtest2", "")
                },
            }.ToString());

        
    /*uint8_t test[0] = {};

    CSHA256 sha = CSHA256();
    sha.Write(test, 0);
    unsigned char buf[CSHA256::OUTPUT_SIZE];
    sha.Finalize(buf);
    return UniValue(HexStr(MakeUCharSpan(buf)));*/






    std::string s = "monke";
    std::vector<uint8_t> test2(s.begin(), s.end());

    //uint8_t empty[SHA3_256::OUTPUT_SIZE] = "";
    //Span<uint8_t> output(empty);

    uint8_t output[SHA3_256::OUTPUT_SIZE];

    SHA3_256 sha = SHA3_256();
    sha.Write(test2);
    sha.Finalize(output);

    //std::vector<uint8_t> test3(output, output + SHA3_256::OUTPUT_SIZE);
    //std::vector<uint8_t> test3;
    //test3.insert(test3.begin(), std::end(output), std::begin(output));
    //uint256 test4 = uint256(test3);
    //return UniValue(test4.GetHex());

    return UniValue(HexStr(MakeUCharSpan(output)));
    //return UniValue(HexStr(output));
}

const CRPCCommand commands[] =
{ //  category              name                                actor (function)                argNames
  //  -----------------     ------------------------            -----------------------         ----------
    { "donation",           "getblockdonation",                 &getblockdonation,              {"height"} },
    { "donation",           "getblockreward",                   &getblockreward,                {"height"} },
    { "donation",           "getblockrewardhalving",            &getblockrewardhalving,         {"height"} },
    { "donation",           "getblockrewardhalvingmax",         &getblockrewardhalvingmax,      {} },
    { "donation",           "getcelebrationblock",              &getcelebrationblock,           {} },
    { "donation",           "getdonationwallets",               &getdonationwallets,            {} },
    { "donation",           "getmaxminableblocks",              &getmaxminableblocks,           {} },
    { "donation",           "getsupplyratio",                   &getsupplyratio,                {} },
    { "donation",           "getsupplytotal",                   &getsupplytotal,                {} },
    { "donation",           "donationtest",                     &donationtest,                  {} },
    { "donation",           "donationtest2",                    &donationtest2,                 {} },
};

void RegisterDonationWalletRPCCommands(CRPCTable& t)
{
    for (const auto& c : commands) {
        t.appendCommand(c.name, &c);
    }
}
