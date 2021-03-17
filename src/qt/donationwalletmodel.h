// Copyright (c) 2021 Blake Copeland
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "donationwallets.h"

#include <QAbstractListModel>
#include <QString>

class DonationWalletModel : public QAbstractListModel
{
    Q_OBJECT

public:

    explicit DonationWalletModel(QObject *parent);

    // Override
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

    // Gets the web address associated for a given donation wallet
    QString GetWebsite(int index) const;

    // Override
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

private:
    std::vector<DonationWalletDescriptor> activeDonationWallets;
};
