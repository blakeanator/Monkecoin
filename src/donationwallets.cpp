// Copyright (c) 2021 Blake Copeland
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <donationwallets.h>

std::vector<DonationWalletDescriptor> DonationWallets::GetActiveDonationWallets(bool IncludeAllWallet)
{
    std::vector<DonationWalletDescriptor> descriptors;

    // Go through each donation wallet (include the "all" option)
    for (unsigned int i = 0; i < DonationWallets::GetSize(); i++)
    {
        // If the wallet is active
        if (donationWallets[i].active)
        {
            descriptors.push_back(donationWallets[i]);
        }
    }

    if (!IncludeAllWallet)
    {
        // Remove the "All" wallet
        descriptors.erase(descriptors.begin());
    }

    return descriptors;
}

unsigned int DonationWallets::GetSize(bool IncludeAllWallet)
{
    unsigned int size = sizeof(donationWallets) / sizeof(donationWallets[0]);

    if (!IncludeAllWallet)
    {
        // Remove the "All" wallet
        size--;
    }

    return size;
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
