#include "sfp.h"
#include <stdlib.h>

sfp int2sfp(int input)
{

    int fin[16];
    int finPosition = 0;

    if (input > 0)
    {
        fin[finPosition] = 0;
    }
    else
    {
        fin[finPosition] = 1;
        input = -input;
    }

    int bin[17];
    int binPosition = 0;

    while (1)
    {
        bin[binPosition] = input % 2;
        input = input / 2;

        binPosition++;

        if (input == 0)
            break;
    }
    binPosition--;

    // exp -> exp 값을 이진수로 시프트 연산자로 변환해서 배열에 추가.
    int exp;
    exp = binPosition + 15;
    int ex;
    finPosition++;
    for (int i = 4; i >= 0; i--) // 2의 4제곱부터 2의 0제곱까지 나눠줌으로써 이진수 변환
    {
        ex = exp >> i;
        if (ex & 1)
            fin[finPosition] = 1;
        else
            fin[finPosition] = 0;
        finPosition++;
    }

    // frac
    if (exp == 31) // special
    {
        for (; finPosition < 16; finPosition++)
        {
            fin[finPosition] = 0;
        }
    }
    else
    {
        binPosition--;
        while (binPosition >= 0 && finPosition < 16) // bin에 있는 값을 역순으로 fin의 7번째 자리부터 대입
        {
            fin[finPosition] = bin[binPosition];
            binPosition--;
            finPosition++;
        }
        while (finPosition < 16) // 나머지 0으로 채움
        {
            fin[finPosition] = 0;
            finPosition++;
        }
    }
    sfp int2sfpResult = 0;
    for (int i = 15; i >= 0; i--)
    {
        int2sfpResult += fin[i];
    }

    return int2sfpResult;
}

int sfp2int(sfp input)
{
    int fin[16];
    int finPosition = 0;

    int ex;
    for (int i = 15; i >= 0; i--)
    {
        ex = input >> i;
        if (ex & 1)
            fin[finPosition] = 1;
        else
            fin[finPosition] = 0;
        finPosition++;
    }

    int exp = 0;
    for (int expIndex = 1; expIndex < 6; expIndex++)
    {
        exp += fin[expIndex] << (5 - expIndex);
    }
    if (exp == 31)
    {
        for (int fracIndex = 6; fracIndex < 16; fracIndex++)
        {
            if (fin[fracIndex])
                return -2147483648;
        }
        if (!fin[0])
            return 2147483647;
        else
            return -2147483648;
    }

    int E = exp - 15;
    if (E < 0)
        return 0; // round to zero

    int value = 0;
    value += 1 << E;
    int c = E - 1;
    for (int fracIndex = 6; fracIndex < 6 + E && fracIndex < 16; fracIndex++)
    {
        value += fin[fracIndex] << c;
        c--;
    }
    if (fin[0])
        value = -value;

    return value;
}

sfp float2sfp(float input)
{
    int bits[16];
    int indB = 0;

    if (input < 0)
    {
        bits[indB] = 1;
        input = -input;
    }
    else
        bits[indB] = 0;
    indB++;

    int ipart = (int)input;
    float fpart = input - ipart;

    int integer[17];
    int indI = 0;
    while (ipart > 0 && indI < 17)
    {
        integer[indI] = ipart % 2;
        ipart /= 2;
        indI++;
    }
    indI--;

    int fractional[15];
    int indF = 0;
    int posF = 100;
    while (fpart && indF < 15)
    {
        fpart *= 2;
        fractional[indF] = (int)fpart;
        fpart -= (int)fpart;
        if (indI < 0 && indF < posF && fractional[indF])
            posF = indF;
        indF++;
    }

    int exp;
    if (indI >= 0)
        exp = indI + 15;
    else if (posF < 100)
        exp = -(posF + 1) + 15;
    else
        exp = 0;

    int tmp;
    for (int c = 4; c >= 0; c--)
    {
        tmp = exp >> c;
        if (tmp & 1)
            bits[indB] = 1;
        else
            bits[indB] = 0;
        indB++;
    }

    if (exp == 31)
    {
        for (; indB < 16; indB++)
        {
            bits[indB] = 0;
        }
    }
    else
    {
        indI--;
        while (indI >= 0 && indB < 16)
        {
            bits[indB] = integer[indI];
            indI--;
            indB++;
        }

        int fraci = 0;
        if (posF < 100)
            fraci = posF + 1;
        for (; fraci < indF && indB < 16; fraci++)
        {
            bits[indB] = fractional[fraci];
            indB++;
        }
        while (indB < 16)
        {
            bits[indB] = 0;
            indB++;
        }
    }

    sfp result = 0;
    for (int i = 0; i < 16; i++)
    {
        result += bits[i] << (15 - i);
    }
    return result;
}

float sfp2float(sfp input)
{
}

sfp sfp_add(sfp a, sfp b)
{
}

sfp sfp_mul(sfp a, sfp b)
{
}

char *sfp2bits(sfp result)
{
}

char *sfp_comp(sfp a, sfp b)
{
}
