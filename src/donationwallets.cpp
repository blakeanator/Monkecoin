// Copyright (c) 2021 Blake Copeland
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <donationwallets.h>

std::vector<DonationWalletDescriptor> DonationWallets::GetActiveDonationWallets()
{
    std::vector<DonationWalletDescriptor> descriptors;

    // Go through each donation wallet (include the "all" option)
    for (unsigned int i = 1; i <= DonationWallets::GetSize(); i++)
    {
        // If the wallet is active
        if (donationWallets[i].active)
        {
            descriptors.push_back(donationWallets[i]);
        }
    }

    return descriptors;
}

unsigned int DonationWallets::GetSize()
{
    return GetSizeIncludingAll() - 1;
}

unsigned int DonationWallets::GetSizeIncludingAll()
{
    return sizeof(donationWallets) / sizeof(donationWallets[0]);
}

bool DonationWallets::IsAddressValid(CScript scriptPubKey)
{
    // Go through each wallet descriptor
    for (const DonationWalletDescriptor& donationWallet : donationWallets)
    {
        if (scriptPubKey == donationWallet.GetCurrentPubKey())
        {
            return true;
        }
    }

    return false;
}
