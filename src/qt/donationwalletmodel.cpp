// Copyright (c) 2021 Blake Copeland
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/donationwalletmodel.h>

DonationWalletModel::DonationWalletModel(QObject *parent) : QAbstractListModel(parent)
{
    activeDonationWallets = DonationWallets::GetActiveDonationWallets(true);
}

QVariant DonationWalletModel::data(const QModelIndex &index, int role) const
{
    unsigned int row = index.row();
    if(row < activeDonationWallets.size())
    {
        switch(role)
        {
        case Qt::EditRole:
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return QVariant(QString::fromStdString(donationWallets[row].name));
        }
    }
    return QVariant();
}

QString DonationWalletModel::GetWebsite(int index) const
{
    return QString::fromStdString(activeDonationWallets[index].link);
}

int DonationWalletModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return DonationWallets::GetActiveDonationWallets().size();
}
