#include "sfp.h"
#include <stdlib.h>
sfp int2sfp(int input){
	int bits[16];
    int indB = 0;

    if(input <0)
    {
        bits[indB] = 1;
        input = - input;
    }
    else bits[indB] = 0;
    indB++;

    int M[17];
    int indM = 0;

    while(input>0 && indM<17)
    {
        M[indM] = input%2;
        input /= 2;
        indM++;
    }
    indM--;

    if(indM <0) return 0;

    int exp;
    exp = indM + 15;

    int tmp;
    for (int c=4; c>=0; c--)
    {
        tmp = exp >> c;
        if(tmp & 1) bits[indB] =1;
        else bits[indB] = 0;
        indB++;
    }

    if(exp ==31) {
        for(; indB<16; indB++)
        {
            bits[indB = 0];
        }
    }
    else {
        indM--;
        while(indM>=0 && indB<16) {
            bits[indB] = M[indM];
            indM--;
            indB++;
        }
        while(indB<16)
        {
            bits[indB] = 0;
            indB++;
        }
    }
    
    sfp result = 0;
    for(int i=0;i<16;i++) {
        result +=bits[i] << (15-i);
    }

    return result;
}

int sfp2int(sfp input){
    int bits[16];
    int indB = 0;

    int tmp;
    for(int c=15; c>=0; c--) {
        tmp =input >> c;
        if(tmp&1) bits[indB] = 1;
        else bits[indB] = 0;
        indB++;
    }

    int exp=0;
    for(int expi=1;expi<6;expi++){
        exp += bits[expi] << (5-expi);
    }
    if(exp == 31)
    {
        for(int fraci=6; fraci<16; fraci++)
        {
            if(bits[fraci]) return -214783648;
        }
        if(!bits[0]) return 2147483647;
        else return -2147483647;
    }

    int E =exp-15;
    if(E<0) return 0;

    int v=0;
    v+= 1<< E;
    int c = E-1;
    for(int fraci = 6; fraci<6+E && fraci <16; fraci++) {
        v += bits[fraci] << c;
        c--;
    }
    if(bits[0]) v=-v;

    return v;
}
sfp float2sfp(float input){
    int bits[16];
    int indB = 0;

    if(input<0) {
        bits[indB] = 1;
        input = -input;
    }
    else bits[indB] =0;
    indB++;

    int ipart = (int) input;
    float fpart = input - ipart;

    int integer[17];
    int indI = 0;

    while(ipart>0 && indI<17) {
        integer[indI] = ipart%2;
        ipart/=2;
        indI++;
    }
    indI--;

    int fractional[15];
    int indF = 0;
    int posF = 100;

    while(fpart && indF<15) {
        fpart*=2;
        fractional[indF] = (int) fpart;
        fpart -= (int) fpart;
        if(indI<0 && indF<posF && fractional[indF]) posF = indF;
        indF++;
    }

    int exp;
    if(indI>=0) exp = indI+15;
    else if(posF <100) exp= -(posF+1)+15;
    else exp=0;

    int tmp;
    for(int c=4; c>=0; c--) {
        tmp = exp >>c;
        if(tmp&1) bits[indB] =1;
        else bits[indB] = 0;
        indB++;
    } 

    if(exp == 31)
    {
        for(;indB<16;indB++)
        {
            bits[indB] = 0;
        }
    }
    else {
        indI--;
        while (indI>=0 && indB<16)
        {
            bits[indB] = integer[indI];
            indI--;
            indB++;
        }
        int fraci = 0;
        if(posF<100) fraci = posF+1;
        for(;fraci<indF && indB<16; fraci++)
        {
            bits[indB] = fractional[fraci];
            indB++;
        }
        while (indB<16)
        {
            bits[indB] = 0;
            indB++;
        }
    }

    sfp result = 0;
    for(int i=0; i<16; i++)
    {
        result += bits[i] << (15-i);
    }

    return result;
}


float sfp2float(sfp input){
    int bits[16];
    int indB = 0;

    int tmp;
    for(int c=15; c>=0; c--) {
        tmp = input >> c;
        if(tmp&1) bits[indB] = 1;
        else bits[indB] = 0;
        indB++;
    }

    int exp=0;
    for(int expi=1; expi<6; expi++)
    {
        exp += bits[expi] << (5-expi);
    }

    int E = exp -15;

    float v = 0.0;
    int c,fraci = 6;
    if(!exp) {
        c = -E;
    }
    else if(E<0)
    {
        c=-E;
        v+= (float) 1 / (float) (1 <<c);
    }
    else{
        c = E-1;
        v+= 1<< E;
        for(;fraci<6+E && fraci<16; fraci++) {
            v += bits[fraci] <<c ;
            c--;
        }
        c++;
    }
    c++;
    for(;fraci<16;fraci++) {
        if(bits[fraci]) v+= (float) 1 / (float) (bits[fraci]<<c);
        c++;
    }

    if(bits[0]) v = -v;

    return v;
}

sfp sfp_add(sfp a, sfp b){

    unsigned short signA, signB;
    signA = a >> 15;
    signB = b >> 15;

    unsigned short expA, expB, mntA, mntB;
    expA = a << 1; expA = expA >> 11;
    expB = b << 1; expB = expB >> 11;
    mntA = a << 6; mntA = mntA >> 6;
    mntB = b << 6; mntB = mntB >> 6;

    sfp result = 0;
    unsigned short sR, expR, mntR;

    sfp NaN = 31<<10 + 1;
    if(expA==31 && expB==31) {
        if(mntA || mntB) return NaN;
        else if(signA != signB) return NaN;
        else return a;
    }
    else if (expA == 31) return a;
    else if (expB == 31) return b;

    short shftP = expA + (~expB) + 1;
    if(expA) mntA += 1 << 10;
    if(expB) mntB += 1 << 10;

    if(shftP < 0) {
        mntA = mntA >> (~shftP);
        expR = expB;
    }
    else {
        mntB = mntB >> shftP;
        expR = expA;
    }

    if(signA != signB)
    {
        if(mntA > mntB) {
            mntR = mntA + (~mntB) +1 ;
            sR = signA;
        }
        else {
            mntR = mntB + (~mntA) + 1;
            sR = signB;
        }
    }
    else {
        mntR = mntA + mntB;
        sR = signA;
    }

    if(mntR & (1<<11)) {
        expR += 1;
        if((mntR&1) && ((mntR-1)%4)) mntR += 1;
        mntR = mntR >> 1;
    }
    else if(!(mntR & (1<<10)))
    {
        int k = -1;
        for(int i=1; i<=10; i++)
        {
            if(mntR & (1<<(10-i))) {
                k=i;
                break;
            }
        }
        if(k<0) expR = 0;
        else {
            expR -= k;
            mntR = mntR << k;
        }
    }

    result += sR << 15;
    result += expR << 10;
    result += mntR - ( mntR&(1<<10) );

    return result;
}

sfp sfp_mul(sfp a, sfp b){

    unsigned short signA, signB;
    signA = a >> 15;
    signB = b >> 15;

    unsigned short expA, expB, mntA, mntB;
    expA= a << 1; expA = expA >> 11;
    expB = b << 1; expB = expB >> 11;
    mntA = a << 6; mntA = mntA >> 6;
    mntB = b << 6; mntB = mntB >> 6;	
    
    if(expA) mntA += 1 << 10;
    if(expB) mntB += 1 << 10;

    sfp NaN = (31<<10) +1;
    sfp negative_inf = (1<<15) + (31<<10);
    sfp positive_inf = (31<<10);
    sfp negative_zero = (1<<15);
    sfp positive_zero = 0;

    if(expA==31 && mntA) return a;
    else if(expB==31 && mntB) return b;
    else if(expA==31 || expB==31) {
        if((expA==0 && mntA==0) || (expB==0 && mntB==0)) return NaN;
        else if(signA != signB) return negative_inf;
        else return positive_inf;
    }
    else if((expA==0 && mntA==0)||(expB==0 && mntB==0)) {
        if(signA != signB) return negative_zero;
        else positive_zero;
    }

    sfp result = 0;
    unsigned short sR=0, mntR=0;
    short expR=0;
    expR = expA + expB - 15;

    sfp ftmp1 = 0; sfp ftmp2 =0;
    for(int i=0; i<11; i++)
    {
        if(!(mntB&(1<<i))) continue;
        ftmp1 += mntA >> 16-i;
        if((~ftmp2&65535) < ((mntA<<i)&65535)) ftmp1+=1;
        ftmp2 += mntA << i;
    }

    int mark1 = -1, mark2 = -1;
    for(int i=5; i>=0; i--) {
        if(ftmp1 & (1<<i)) {
            mark1 = i;
            break;
        }
    }
    for(int j=15; (j>=0) && (mark1<0); j--) {
        if(ftmp2 & (1<<j)) {
            mark2 = j;
            break;
        }
    }

    if(mark1 > 0) {
        expR += mark1 - 4;
        mntR += (ftmp1 - (1<<mark1)) << (10-mark1);
        ftmp2 = ftmp2 >> (mark1+5);
        if((ftmp2&1) && ((ftmp2-1)%4)) ftmp2 += 1;
        mntR += ftmp2 >> 1;
    }
    else if(mark2 > 0) {
        expR += mark2 - 20;
        if(mark2>=10) ftmp2 = ftmp2 << (mark2 - 10);
        else ftmp2 = ftmp2 << (10-mark2);

        if((ftmp2&1) && ((ftmp2-1)%4)) ftmp2 += 1;
        mntR += ftmp2 >> 1;
    }
    else {
        expR = 0;
        mntR = 0;
    }

    sR = signA ^ signB;
    if(expR >= 31) {
        if(sR) return negative_inf;
        return positive_inf;
    }
    else if(expR < 0) expR = 0;

    result += sR << 15;
    result += expR << 10;
    result += mntR;
    return result;
}

char* sfp2bits(sfp result){
    char* bits = (char*)malloc(sizeof(char)*17);
    int indB = 0;

    int tmp;
    for(int c=15;c>=0;c--) {
        tmp = result >> c;
        if(tmp &1) bits[indB] = '1';
        else bits[indB] = '0';
        indB++;
    }
    bits[indB] = '\0';

    return bits;
}


// char* sfp_comp(sfp a, sfp b){

// }
// 기존 파일
// #include "sfp.h"
// #include <stdlib.h>
// sfp int2sfp(int input){
	
// }

// int sfp2int(sfp input){
// }

// sfp float2sfp(float input){
// }

// float sfp2float(sfp input){
// }

// sfp sfp_add(sfp a, sfp b){
	
// }

// sfp sfp_mul(sfp a, sfp b){
	
// }

// char* sfp2bits(sfp result){

// }

// char* sfp_comp(sfp a, sfp b){

// }
