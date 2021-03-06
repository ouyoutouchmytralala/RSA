#include "bigrandomprime.h"
#include <QTime>
#include <QDebug>
#include <iostream>

BigRandomPrime::BigRandomPrime(const quint32 length) : m_bitnumber(length)
{
    if(length == 0)
        return;
    Q_ASSERT_X(length >= 2, "length", "length of new prime must be >= 2");
    randomize();
    m_bitnumber.setBit(0, 1);
    m_bitnumber.setBit(m_bitnumber.size() - 1, 1); //no even numbers!
    for(quint32 itr = 1; itr < m_bitnumber.size() - 1; ++itr)
        m_bitnumber.setBit(itr, qrand() % 2);
}

void BigRandomPrime::show(const quint8 shift) const
{
    if(m_bitnumber.size() == 0)
    {
        std::cout << "0 (empty)" << std::endl << std::flush;
        return;
    }
    for(quint8 itr = 0; itr < shift; ++itr)
        std::cout << 0 << std::flush;
    for(quint32 itr = 0; itr < m_bitnumber.size(); ++itr)
        std::cout << m_bitnumber.testBit(itr) << std::flush;
    std::cout << std::endl << std::flush;
}

BigRandomPrime operator +(const BigRandomPrime &a1, const BigRandomPrime &a2)
{
    QBitArray result = DecToBin::operator +(a1.m_bitnumber, a2.m_bitnumber);
        return BigRandomPrime(result);
}

BigRandomPrime operator %(const BigRandomPrime &a1, const BigRandomPrime &a2)
{
    QBitArray result = DecToBin::operator %(a1.m_bitnumber, a2.m_bitnumber);
    return BigRandomPrime(result);
}

BigRandomPrime operator -(const BigRandomPrime &a1, const BigRandomPrime &a2)
{
    QBitArray result = DecToBin::operator -(a1.m_bitnumber, a2.m_bitnumber);
    return BigRandomPrime(result);
}

BigRandomPrime operator *(const BigRandomPrime &a1, const BigRandomPrime &a2)
{
    QBitArray result = DecToBin::operator *(a1.m_bitnumber, a2.m_bitnumber);
    return BigRandomPrime(result);
}

BigRandomPrime operator /(const BigRandomPrime &a1, const BigRandomPrime &a2)
{
    QBitArray result = DecToBin::operator /(a1.m_bitnumber, a2.m_bitnumber);
    return BigRandomPrime(result);
}

bool operator ==(const BigRandomPrime &a1, const BigRandomPrime &a2)
{
    return DecToBin::operator ==(a1.m_bitnumber, a2.m_bitnumber);
}

bool operator !=(const BigRandomPrime &a1, const BigRandomPrime &a2)
{
    return !(DecToBin::operator ==(a1.m_bitnumber, a2.m_bitnumber));
}

quint64 BigRandomPrime::ToDec() const
{
    return DecToBin::bitArrayToDec(m_bitnumber);
}

bool BigRandomPrime::test() const
{
    using namespace BRP;
    return tableTest(primes, tableSize) && MRtest(m_bitnumber, m_bitnumber.size());
}

bool BigRandomPrime::tableTest(const quint16 table[], const quint8 tableSize) const
{
    for(quint8 itr = 0; itr < tableSize; ++itr)
        if((*this % BigRandomPrime(DecToBin::decToBitArray(table[itr])) == BigRandomPrime(0)))
        {
            if(*this == BigRandomPrime(DecToBin::decToBitArray(table[itr])))
                return true;
            qDebug() << this->ToDec() << " delitsia na " << table[itr];
            this->show();
            return false;
        }
    return true;
}

bool BigRandomPrime::MRtest(const QBitArray &prime, const quint32 rounds) const//alghoritm Miller - Rabin
{
    QBitArray mult(DecToBin::operator -(prime, DecToBin::decToBitArray(1))); //mult = prime - 1
    QBitArray powerOfTwo(0);
    while(DecToBin::operator ==(DecToBin::operator %(mult, DecToBin::decToBitArray(2)), QBitArray(0)))
    {
        powerOfTwo = DecToBin::operator +(powerOfTwo, DecToBin::decToBitArray(1));
        mult = DecToBin::operator /(mult, DecToBin::decToBitArray(2));
    }//now: prime - 1 = 2 ^ powerOfTwo * mult
    for(quint32 round = 1; round <= rounds; ++round)
    {
        QBitArray witnessOfSimplicity = BigRandom((qrand() % (m_bitnumber.size() - 2)) + 2).getQBitArray();
//        qDebug() << "mult = " << BigRandomPrime(mult).ToDec();
//        qDebug() << "powerOfTwo = " << BigRandomPrime(powerOfTwo).ToDec();
//        qDebug() << "witnessOfSimplicity = " << BigRandomPrime(witnessOfSimplicity).ToDec();
        QBitArray temp(0);
        temp = DecToBin::operator %(DecToBin::operator ^(witnessOfSimplicity, mult), prime);
        if(DecToBin::operator ==(temp, DecToBin::decToBitArray(1)) ||
                DecToBin::operator ==(temp, DecToBin::operator -(prime, DecToBin::decToBitArray(1))))
            continue;
        for(QBitArray itr(0); DecToBin::operator <(itr, DecToBin::operator -(powerOfTwo, DecToBin::decToBitArray(1)));
            itr = DecToBin::operator +(itr, DecToBin::decToBitArray(1)))
        {
            temp = DecToBin::operator %(DecToBin::operator *(temp, temp), prime);
            if(DecToBin::operator ==(temp, DecToBin::decToBitArray(1)))
                    return false;
            if(DecToBin::operator ==(temp, DecToBin::operator -(prime, DecToBin::decToBitArray(1))))
                goto labelnextitr;
        }
//        qDebug() << "mult = " << BigRandomPrime(mult).ToDec();
//        qDebug() << "powerOfTwo = " << BigRandomPrime(powerOfTwo).ToDec();
//        qDebug() << "witnessOfSimplicity = " << BigRandomPrime(witnessOfSimplicity).ToDec();
//        qDebug() << "temp = " << BigRandomPrime(temp).ToDec();

        return false;
        labelnextitr: ;
    }
    return true;
}




void BigRandomPrime::randomize() const
{
    static bool isCalled = false;
    if(isCalled)
        return;
    qsrand(time(NULL));
    isCalled = true;
}

//bool BigRandomPrime::isWitnessSimplicity(const QBitArray &prime, const QBitArray &witness, QBitArray power) const //prime is not even and > 1
//{
//    QBitArray witnessInPower(witness);
//    for(; DecToBin::operator >(power, QBitArray(1, 1)); power = DecToBin::operator -(power, QBitArray(1, 1)))
//        witnessInPower = DecToBin::operator *(witnessInPower, witness);
//    if(DecToBin::operator ==(DecToBin::operator %(witnessInPower, prime), QBitArray(1, 1)))
//        return true;


//}

quint64 DecToBin::bitArrayToDec(const QBitArray &a)
{
    Q_ASSERT_X(a.size() < 64, "quint64", "quint64 cannot have more than 63 bits length");
    quint64 result = 0;
    quint64 poweroftwo = 1;
    for(qint8 itr = a.size() - 1; itr >= 0; --itr, poweroftwo *= 2)
        if(a.testBit(itr))
            result += poweroftwo;
    return result;
}

QBitArray DecToBin::operator+(const QBitArray &a1, const QBitArray &a2)
{
    QBitArray result(qMax(a1.size(), a2.size()) + 1);
    const QBitArray *min = a1.size() < a2.size() ? &a1 : &a2;
    const QBitArray *max = a1.size() > a2.size() ? &a1 : &a2;
    quint32 itr = 1;
    bool rest = 0;
    bool nextrest = 0;
    for(; itr <= min->size(); ++itr)
    {
        result.setBit(result.size() - itr, max->testBit(max->size() - itr) ^ min->testBit(min->size() - itr));
        nextrest = max->testBit(max->size() - itr) && min->testBit(min->size() - itr);
        nextrest = (result.testBit(result.size() - itr) && rest) || nextrest;
        result.setBit(result.size() - itr, result.testBit(result.size() - itr) ^ rest);
        rest = nextrest;
    }
    nextrest = 0;
    for(;itr <= max->size(); ++itr)
    {
        result.setBit(result.size() - itr, max->testBit(max->size() - itr) ^ rest);
        rest = max->testBit(max->size() - itr) && rest;
    }
    if(rest)
    {
        result.setBit(0, 1);
        return result;
    }
    QBitArray resultWithLessSize(result.size() - 1);
    for(quint32 itr = 1; itr <= resultWithLessSize.size(); ++itr)
        resultWithLessSize.setBit(resultWithLessSize.size() - itr, result.testBit(result.size() - itr));
    return resultWithLessSize;
}

QBitArray DecToBin::operator-(const QBitArray &a1, const QBitArray &a2)
{
    QBitArray result(qMax(a1.size(), a2.size()));
    const QBitArray *min = a1 < a2 ? &a1 : &a2;
    const QBitArray *max = a1 > a2 ? &a1 : &a2;
    //qDebug() << BigRandomPrime(a1).ToDec();
    //qDebug() << BigRandomPrime(a2).ToDec();
    if(DecToBin::operator ==(*max, *min))
        return QBitArray(0);
    quint32 itr = 1;
    bool rest = 0;
    bool nextrest = 0;
    for(; itr <= min->size(); ++itr)
    {
        result.setBit(result.size() - itr, max->testBit(max->size() - itr) ^ min->testBit(min->size() - itr));
        nextrest = !max->testBit(max->size() - itr) && min->testBit(min->size() - itr);
        nextrest = (!result.testBit(result.size() - itr) && rest) || nextrest;
        result.setBit(result.size() - itr, result.testBit(result.size() - itr) ^ rest);
        rest = nextrest;
    }
    nextrest = 0;
    for(;itr <= max->size(); ++itr)
    {
        result.setBit(result.size() - itr, max->testBit(max->size() - itr) ^ rest);
        rest = !max->testBit(max->size() - itr) && rest;
    }
    if(rest)
    {
        result.setBit(0, 0);
        return result;
    }
    quint32 lessResultSize = result.size();
    for(itr = 0; itr < result.size() && !result.testBit(itr); ++itr)
        --lessResultSize;
    QBitArray resultWithLessSize(lessResultSize);
    for(itr = 1; itr <= lessResultSize; ++itr)
        resultWithLessSize.setBit(resultWithLessSize.size() - itr, result.testBit(result.size() - itr));
    return resultWithLessSize;
}

QBitArray DecToBin::operator*(const QBitArray &a1, const QBitArray &a2)
{
    QBitArray result(0);
    if(DecToBin::operator ==(a2, decToBitArray(2)))
    {
        result = a1;
        result.resize(result.size() + 1);
        return result;
    }
    if(a1.size() == 0 || a2.size() == 0)
        return result; //empty array
//    result.resize(a1.size() + a2.size());
    quint32 a1size = a1.size();
    for(quint32 itr = 1; itr <= a2.size(); ++itr)
    {
        if(a2.testBit(a2.size() - itr))
            result = result + a1;
        const_cast<QBitArray &>(a1).resize(a1.size() + 1); //in this way we make left shift
    }
    const_cast<QBitArray &>(a1).resize(a1size);
    return result;
}

QBitArray DecToBin::operator/(const QBitArray &a1, const QBitArray &a2)
{
    QBitArray result(0);
    if(DecToBin::operator ==(a2, decToBitArray(2)))
    {
        result = a1;
        result.resize(result.size() - 1);
        return result;
    }
    quint32 itrResult = 0;
    QBitArray current(0);
    quint32 itrA1 = 0;
    for(; current < a2; ++itrA1)
    {
        current.resize(current.size() + 1);
        current.setBit(itrA1, a1.testBit(itrA1));
    }
    for(; itrA1 < a1.size(); ++itrA1)
    {
        if(current >= a2)
        {
            result.resize(result.size() + 1);
            result.setBit(itrResult, 1);
            ++itrResult;
            current = current - a2;
        }
        else
        {
            result.resize(result.size() + 1);
            result.setBit(itrResult, 0);
            ++itrResult;
        }
        current.resize(current.size() + 1);
        current.setBit(current.size() - 1, a1.testBit(itrA1));
    }
    if(current >= a2)
    {
        result.resize(result.size() + 1);
        result.setBit(itrResult, 1);
        ++itrResult;
        current = current - a2;
    }
    else
    {
        result.resize(result.size() + 1);
        result.setBit(itrResult, 0);
        ++itrResult;
    }
    return result;
}

QBitArray DecToBin::operator%(const QBitArray &a1, const QBitArray &a2)
{
    if(DecToBin::operator ==(a2, decToBitArray(2)))
            return QBitArray(1, a1.testBit(a1.size() - 1));
    if(DecToBin::operator <(a1, a2))
        return a1;
    QBitArray current(0);
    quint32 itrA1 = 0;
    for(; current < a2; ++itrA1)
    {
        current.resize(current.size() + 1);
        current.setBit(itrA1, a1.testBit(itrA1));
    }
    for(; itrA1 < a1.size(); ++itrA1)
    {
        if(current >= a2)
            current = current - a2;
        current.resize(current.size() + 1);
        current.setBit(current.size() - 1, a1.testBit(itrA1));
    }
    if(current >= a2)
        current = current - a2;
    return current;
}

bool DecToBin::operator>(const QBitArray &a1, const QBitArray &a2)
{
    quint32 a1size = a1.size();
    while(a1size > 0 && !a1.testBit(a1.size() - a1size))
        --a1size;
    quint32 a2size = a2.size();
    while(a2size > 0 && !a2.testBit(a2.size() - a2size))
        --a2size;
    if(a1size > a2size)
        return true;
    if(a1size < a2size)
        return false;
    for(qint32 itr = a1size; itr > 0; --itr) //now we have QBitArrays with similar real(without preceding 0 bits) size and just need to check the bits from highest to lowest
        if(a2.testBit(a2.size() - itr) < a1.testBit(a1.size() - itr))
            return true;
        else
            if(a2.testBit(a2.size() - itr) > a1.testBit(a1.size() - itr))
                return false;
    return false;
}

bool DecToBin::operator>=(const QBitArray &a1, const QBitArray &a2)
{
    quint32 a1size = a1.size();
    while(a1size > 0 && !a1.testBit(a1.size() - a1size))
        --a1size;
    quint32 a2size = a2.size();
    while(a2size > 0 && !a2.testBit(a2.size() - a2size))
        --a2size;
    if(a1size > a2size)
        return true;
    if(a1size < a2size)
        return false;
    for(qint32 itr = a1size; itr > 0; --itr) //now we have QBitArrays with similar real(without preceding 0 bits) size and just need to check the bits from highest to lowest
        if(a2.testBit(a2.size() - itr) < a1.testBit(a1.size() - itr))
            return true;
        else
            if(a2.testBit(a2.size() - itr) > a1.testBit(a1.size() - itr))
                return false;
    return true;
}

bool DecToBin::operator<(const QBitArray &a1, const QBitArray &a2)

{
    quint32 a1size = a1.size();
    while(a1size > 0 && !a1.testBit(a1.size() - a1size))
        --a1size;
    quint32 a2size = a2.size();
    while(a2size > 0 && !a2.testBit(a2.size() - a2size))
        --a2size;
    if(a1size < a2size)
        return true;
    if(a1size > a2size)
        return false;
    for(qint32 itr = a1size; itr > 0; --itr) //now we have QBitArrays with similar real(without preceding 0 bits) size and just need to check the bits from highest to lowest
        if(a1.testBit(a1.size() - itr) > a2.testBit(a2.size() - itr))
            return false;
        else
            if(a1.testBit(a1.size() - itr) < a2.testBit(a2.size() - itr))
                return true;
    return false;
}

bool DecToBin::operator<=(const QBitArray &a1, const QBitArray &a2)
{
    quint32 a1size = a1.size();
    while(a1size > 0 && !a1.testBit(a1.size() - a1size))
        --a1size;
    quint32 a2size = a2.size();
    while(a2size > 0 && !a2.testBit(a2.size() - a2size))
        --a2size;
    if(a1size < a2size)
        return true;
    if(a1size > a2size)
        return false;
    for(qint32 itr = a1size; itr > 0; --itr) //now we have QBitArrays with similar real(without preceding 0 bits) size and just need to check the bits from highest to lowest
        if(a1.testBit(a1.size() - itr) > a2.testBit(a2.size() - itr))
            return false;
        else
            if(a1.testBit(a1.size() - itr) < a2.testBit(a2.size() - itr))
                return true;
    return true;
}

bool DecToBin::operator==(const QBitArray &a1, const QBitArray &a2)
{
    quint32 a1size = a1.size();
    while(a1size > 0 && !a1.testBit(a1.size() - a1size))
        --a1size;
    quint32 a2size = a2.size();
    while(a2size > 0 && !a2.testBit(a2.size() - a2size))
        --a2size;
    if(a1size != a2size)
        return false;
    for(quint32 itr = a1size; itr > 0; --itr)
        if(a1.testBit(a1.size() - itr) != a2.testBit(a2.size() - itr))
            return false;
    return true;
}

bool DecToBin::operator!=(const QBitArray &a1, const QBitArray &a2)
{
    return !(operator !=(a1, a2));
}

QBitArray DecToBin::operator^(const QBitArray &a1, QBitArray a2)
{
    if(DecToBin::operator ==(a2, QBitArray(0)))
            return QBitArray(1, 1);
    QBitArray result(a1);
    while(DecToBin::operator >(a2, QBitArray(1, 1)))
    {
        result = DecToBin::operator *(result, a1);
        a2 = DecToBin::operator -(a2, QBitArray(1, 1));
    }
    return result;
}

quint64 DecToBin::determinateDecSizeToBin(const quint64 &dec)
{
    if(dec == 0)
        return 1;
    quint64 result = 0;
    quint64 powerOfTwo = 1;
    while(dec >= powerOfTwo)
    {
        powerOfTwo *= 2;
        ++result;
    }
    return result;
}

QBitArray DecToBin::decToBitArray(quint64 dec)
{
    QBitArray result(determinateDecSizeToBin(dec));
    quint32 index = 1;
    while(dec > 0)
    {
        result.setBit(result.size() - index, dec % 2);
        dec /= 2;
        ++index;
    }
    return result;
}

void BigRandom::randomize() const
{
    static bool isCalled = false;
    if(isCalled)
        return;
    qsrand(time(NULL));
    isCalled = true;
}

BigRandom::BigRandom(const quint32 length) : m_bitnumber(length)
{
    Q_ASSERT_X(length >= 1, "length", "length of new big random must be >= 1");
    randomize();
    m_bitnumber.setBit(0, 1);
    for(quint32 itr = 1; itr < m_bitnumber.size(); ++itr)
        m_bitnumber.setBit(itr, qrand() % 2);
}
