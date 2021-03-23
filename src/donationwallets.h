// Copyright (c) 2021 Blake Copeland
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MONKECOIN_DONATIONWALLETS_H
#define MONKECOIN_DONATIONWALLETS_H

#include <script/script.h>
#include <script/standard.h>
#include <pubkey.h>

struct AddressRecord {

    // The time of which the key was added
    uint32_t nTime;

    // The address
    const unsigned char address[];
};

// A wallet address with an associated name and link
struct DonationWalletDescriptor {

    // Whether the wallet is active or disabled
    bool active;

    // The name of the non-profit
    std::string name;

    // A web address to the non-profit
    std::string link;

    // A record of the addresses
    //const std::vector<AddressRecord> addressRecord;

    CScript GetCurrentPubKey() const
    {
        ////const unsigned char test[addressRecord.back().address.size()] = addressRecord.back().address;
        //const unsigned char test[] = "dshjkfhjskdhfjksdhfjksdhfjksdhfjkhdsf";

        /*
const unsigned char R1Array[] =
    "\x9c\x52\x4a\xdb\xcf\x56\x11\x12\x2b\x29\x12\x5e\x5d\x35\xd2\xd2"
    "\x22\x81\xaa\xb5\x33\xf0\x08\x32\xd5\x56\xb1\xf9\xea\xe5\x1d\x7d";
const uint160 R1S = uint160(std::vector<unsigned char>(test,test+20));
        */

        ////DecodeDestination()


        //CKeyID temp = CKeyID(uint160(std::vector<unsigned char>(test, test + 20)));
        ////CKeyID(uint160(ParseHex("816115944e077fe7c803cfa57f29b36bf87c1d35")))
        ////CKeyID temp = CPubKey(vSolutions[0]).GetID();
        ////CKeyID temp = CKeyID(uint160('pEkRA9HiCLRfqVZ7RYt2r2Ppnr97952MQ2'));
        //CTxDestination temp2 = CTxDestination(temp);
        ////CTxDestination temp = CBitcoinAddress("pEkRA9HiCLRfqVZ7RYt2r2Ppnr97952MQ2").Get();
        //CScript temp3 = GetScriptForDestination(temp);
        //return temp3;

        ////return addressRecord.back().addre;

        return CScript();
    }
};

/* An array of donation wallets. The order of these wallets should never change.
   New wallets should be added at the bottom of the list. If a wallet needs to be
   removed, simply disable it instead of actually removing it from the array.
   Removing a wallet from the array would break the donationWalletIndex in previous 
   blocks' transactions.
*/
static DonationWalletDescriptor donationWallets[] = {
    {1, "All", ""/*, {{0, ""}}*/}//,
    //{1, "Test", "http://www.test.com", {{0, "pEkRA9HiCLRfqVZ7RYt2r2Ppnr97952MQ2"}}},
    //{1, "Google", "https://www.google.com", {{0, "pEkRA9HiCLRfqVZ7RYt2r2Ppnr97952MQ2"}}}
    //{"020000000000000000000000000000000000000000000000000000000000000000", "Some Non-profit", "www.google.com", 1}
    //{uint256S("0x020000000000000000000000000000000000000000000000000000000000000000"), "Some Non-profit", "www.google.com", 1}
};

namespace DonationWallets
{
    std::vector<DonationWalletDescriptor> GetActiveDonationWallets();

    // How many donation wallets there are
    unsigned int GetSize();

    // How many donation wallets there are (incuding index zero)
    unsigned int GetSizeIncludingAll();

    // Whether an address is one that belongs to a donation wallet
    bool IsAddressValid(CScript scriptPubKey);
}

#endif // MONKECOIN_DONATIONWALLETS_H
