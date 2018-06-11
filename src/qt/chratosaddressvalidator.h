// Copyright (c) 2011-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef CHRATOS_QT_CHRATOSADDRESSVALIDATOR_H
#define CHRATOS_QT_CHRATOSADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class ChratosAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit ChratosAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** Chratos address widget validator, checks for a valid chratos address.
 */
class ChratosAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit ChratosAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif // CHRATOS_QT_CHRATOSADDRESSVALIDATOR_H
