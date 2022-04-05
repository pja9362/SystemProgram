#include "sfp.h"
#include <stdlib.h>
//#include <float.h>

// convert int to sfp
sfp int2sfp(int input){
	int bitArr[16]; // array to store 16-bit
    int bIdx = 0; // index of bitArr

    int MArr[17]; // array to store 'M' value 
    int mIdx = 0; // index of MArr

    // sign bit => positive: 0, negative 1
    if(input>=0){
        if(input==0)
            return +0.0;
        bitArr[bIdx] = 0;
    }
    else {
        bitArr[bIdx] = 1;
        input = -input; // negative -> positve
    }
    bIdx++;

    // decimal to binary
    while(input>0 && mIdx<17)
    {
        MArr[mIdx++] = input%2;
        input /= 2;
    }
    mIdx--; 

    int exp;
    exp = mIdx + 15; // E = exp - 15, E == index of M

    // exp <- decimal to binary
    for (int i=5; i>0; i--) {
        if(exp !=0) {
            bitArr[i] = exp%2;
            exp /= 2;
        }
        else {
            bitArr[i] = 0;
        }
        bIdx++; // increase a bit index after assign the value
    }

    if(exp!=31) {
        mIdx--; 
        // input fractional value using bitArr and MArr
        while(mIdx>=0 && bIdx<16) 
            bitArr[bIdx++] = MArr[mIdx--];

        // assign 0 to empty space
        for(;bIdx<16;bIdx++)
            bitArr[bIdx] = 0;
    }
    // Special case: exp =111...1
    else {
        for(; bIdx<16; bIdx++)
            bitArr[bIdx]=0;
    }
    
    sfp result = 0; 
    for(int i=0;i<16;i++) 
        result += bitArr[i] << (15-i);
    // convert array type to sfp type using shift(<<)

    return result;
}
// sfp => unsigned short : 0~65535

int sfp2int(sfp input){
    int bitArr[16]; // array to store 16-bit

    // sfp to binary
    for(int i=15; i>=0; i--) {
        if(input!=0) {
            bitArr[i] = input%2;
            input /= 2;
        }
        // store 0 in remaining space
        else {
            bitArr[i] = 0;
        }
    }
    
    int exp=0;
    int W_value = 0; // to store value calculated by considering weights for each Index 
    
    for(int expIdx=1; expIdx<6; expIdx++) {
        W_value = bitArr[expIdx] << (5-expIdx);
        exp += W_value;
    }
    // Special cases: exp =111...1
    if(exp == 31)
    {
        if(bitArr[0]) return -2147483648; // -inf
        else return 2147483647; // +inf

        for(int fracIdx=6; fracIdx<16; fracIdx++)
        {
            if(bitArr[fracIdx]) return -2147483648; //NaN 
        }
    }

    // E = exp - bias, bias = 15
    int E = exp-15;
    if(E<0) return 0;

    // V = (-1)^(s) * M * 2^(E)
    int V = 0; 
    int temp = E-1;
    int W = 1<<E; // weights for each Index
    for(int fracIdx = 6; fracIdx < 16 && fracIdx < 6+E; fracIdx++) {
        W += bitArr[fracIdx] << temp;
        temp--;
    }
    V = W << E >> E;

    if(bitArr[0]) V=-V; // sign bit '1' means negative

    return V;
}


// convert float to sfp
sfp float2sfp(float input){
    int bitArr[16]; // array to store 16-bit
    int bIdx = 0; // index of bitArr

    // sign bit => positive: 0, negative 1
    if(input>=0) {
        if(input==0)
            return +0.0;
        bitArr[bIdx] = 0;
    }
    else {
        bitArr[bIdx] = 1;
        input = -input; // negative to positive
    }
    bIdx++;

    int iNum = (int) input; // int part
    float fNum = input - iNum; // float(fractional) part

    // to store values about 'int' part and index of iArr
    int iArr[17]; int iIdx = 0;

    // decimal to binary
    while(iNum>0 && iIdx<17) {
        iArr[iIdx++] = iNum%2;
        iNum/=2;
    }
    iIdx--;

    // to store values about 'float' part and index of fArr
    int fArr[15];  int fIdx = 0; 

    int pos = 999; 

    while(fNum && fIdx<15) {
        fNum*=2;
        fArr[fIdx++] = (int) fNum;
        fNum -= (int) fNum;

        if(iIdx<0) {
            for(int i=0; i<15; i++) {
                if(fArr[i]) {
                    pos = i;
                    break;
                }
            }
        }
    }
    // E = exp-15
    int exp; int E;

    // there is an integer part
    if(iIdx>=0) {
        E = iIdx;
        exp = E+15;
    } 
    // no integer part
    else {
        if(pos!=999) {
            E = -(pos+1);
            exp= E + 15;
        } 
        else 
            exp=0; 
    }
    
    // decimal to binary
    for (int i=5; i>0; i--) {
        if(exp !=0) {
            bitArr[i] = exp%2;
            exp /= 2;
        }
        else {
            bitArr[i] = 0;
        }
        bIdx++;  
    }

    // Special cases: exp = 111..1
    if(exp == 31)
    {
        for(;bIdx<16;bIdx++)
        {
            bitArr[bIdx] = 0;
        }
    }
    else {
        iIdx--; 
        while (iIdx>=0 && bIdx<16)
            bitArr[bIdx++] = iArr[iIdx--]; // put integer part

        int fracIdx = 0; 

        if(pos!=999) 
            fracIdx = pos+1; // (becuse normalized case, implied leading 1)
        
        for(;fracIdx<fIdx && bIdx<16; fracIdx++)
            bitArr[bIdx++] = fArr[fracIdx]; // put fractional(float) part
        
        for (;bIdx<16;bIdx++) // put 0 to empty index
            bitArr[bIdx] = 0;
    }

    sfp result = 0;
    for(int i=0; i<16; i++)
        result += bitArr[i] << (15-i);
    // convert array type to sfp type using shift(<<)
    return result;
}


float sfp2float(sfp input){
    
    int bitArr[16]; // array to store 16-bit

    // decimal to binary
    for(int i=15; i>=0; i--) {
        if(input!=0) {
            bitArr[i] = input%2;
            input /= 2;
        }
        else 
            bitArr[i] = 0;
    }

    int exp=0; 
    int W_value = 0; // to store value calculated by considering weights for each Index 

    for(int expIdx=1; expIdx<6; expIdx++) {
        W_value = bitArr[expIdx] << (5-expIdx);
        exp += W_value;
    }

    // E = exp - bias, V = (-1)^(s) * M * 2^(E)
    int E = exp - 15; 
    float V = 0; 
    
    int fracIdx = 6;
    int W=6;

    if(exp==0) // when exp ==0, denormarlized
        W = -E;
    else if(E<0) // No Integer part
    {
        W=-E;
         V += 1 / (float) (1 << W);
    }
    else{ // Exists Integer part
        // calculate int part
        W = E-1;
        V += 1<< E;
        for(;fracIdx<6+E && fracIdx<16; fracIdx++) {
            V += bitArr[fracIdx] << W ;
            W--;
        }
        W++; 
    }
    W++;

    // calculate frac part
    for(;fracIdx<16;fracIdx++) {
        if(bitArr[fracIdx]) 
            V += 1 / (float) (bitArr[fracIdx] << W);
        W++;
    }

    if(bitArr[0]) V = -V; // sign bit '1' means negative

    return V;
}

sfp sfp_add(sfp a, sfp b){
    // find s, exp, frac of a,b using Shift(<<,>>)

    // find sign bit , size: first 1 bit
    unsigned short signA, signB;
    signA = a >> 15; 
    signB = b >> 15;
    // find exp , size: 5 bit
    unsigned short expA, expB;
    expA = a << 1; expA = expA >> 11;
    expB = b << 1; expB = expB >> 11;
    // find frac , size: 10 bit
    unsigned short fracA, fracB;
    fracA = a << 6; fracA = fracA >> 6;   
    fracB = b << 6; fracB = fracB >> 6;

    sfp result = 0; // to store the result after addition
    unsigned short signR, expR, fracR;

    short Shift; // to store calculate bit-Shift

    // Special cases
    sfp NaN = (31<<10) + 1;
    //31 --> 111..1
    if(expA==31 && expB==31) {
        if(fracA || fracB) return NaN;
        else if(signA != signB) return NaN;
        else return a; 
    }
    else if (expA==31 || expB==31) {
        if (expA == 31) return a;
        else return b;
    }

    // In normalized case, M : implied leading 1
    if(expA!=0) fracA += (1<<10);
    if(expB!=0) fracB += (1<<10);
    
    // use 'two's complement
    Shift = expA + (~expB) + 1; 
    // when (expA >= expB)
    if (Shift >=0 ) {
        fracB = fracB >> Shift;
        expR = expA; 
    }
    // when (expA < expB)
    else {
        fracA = fracA >> (~Shift) + 1; 
        expR = expB; 
    }

    // same sign; all negative or all positive
    if(signA == signB) {
        signR = signA;
        fracR = fracA + fracB;
    }
    // different sign (+ or -)
    else {
        if(fracA > fracB) {
            signR = signA;
            fracR = fracA + (~fracB) +1 ;
        }
        else {
            signR = signB;
            fracR = fracB + (~fracA) + 1;
        }
    }

    if(fracR & (1<<11)) {
        expR += 1; //increase the exp
        fracR = fracR >> 1;
    }
    else if(!(fracR & (1<<10)))
    {
        int key = 999;
        for(int i=1; i<=10; i++)
        {
            if(fracR & 1<<(10-i)) {
                key=i;
                break;
            }
        }
        if(key!=999) {
            expR -= key;
            fracR = fracR << key;
        } 
        else expR = 0;
    }
    // put s exp,and frac together using shift operator
    signR = signR << 15;
    expR = expR << 10;
    fracR -= ( fracR&(1<<10) );

    result = signR + expR + fracR;

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
// convert sfp to bit
char* sfp2bits(sfp result){
    char* bitArr = (char*)malloc(sizeof(char)*17);

    int temp; int bIdx = 0;

    for(int W=15;W>=0;W--) {
        temp = result >> W;
        if(temp&1) {
            bitArr[bIdx++] = '1';
        } else {
            bitArr[bIdx++] = '0';
        }
    }
    bitArr[bIdx] = '\0';

    return bitArr;
}


char* sfp_comp(sfp a, sfp b){
    // find s, exp, frac of a,b using Shift(<<,>>)

    // find sign bit , size: first 1 bit
    unsigned short signA, signB;
    signA = a >> 15; 
    signB = b >> 15;
    // find exp , size: 5 bit
    unsigned short expA, expB;
    expA = a << 1; expA = expA >> 11;
    expB = b << 1; expB = expB >> 11;
    // find frac , size: 10 bit
    unsigned short fracA, fracB;
    fracA = a << 6; fracA = fracA >> 6;   
    fracB = b << 6; fracB = fracB >> 6;

    // when a,b has same value
    if(signA==signB && expA==expB && fracA==fracB)
        return "=";

    // change b's sign bit
    // to use subtraction by applying the addition function's feature
    signB = (~signB); 

    unsigned short signR, expR, fracR;

    // 특별 케이스 나중에 확인
    // sfp NaN = 31<<10 + 1; // +가 우선함
    // // 둘다 스페셜 케이스
    // //a,b 둘다 inf
    // if(expA==31 && expB==31) {
    //     // 둘 중에 하나라도 frac에 1이 존재한다면
    //     if(fracA || fracB) return NaN;
    //     // 두 부호가 다르다면,
    //     else if(signA != signB) return NaN;
    //     else return a; //b여도 상관 X
    // }
    // else if (expA==31 || expB==31) {
    //     // a만 inf
    //     if (expA == 31) return a;
    //     // b만 inf
    //     else return b;
    // }

    // In normalized case, M : implied leading 1
    if(expA!=0) fracA += (1<<10);
    if(expB!=0) fracB += (1<<10);
    
    // use 'two's complement
    short Shift = expA + (~expB) + 1; 
    // when (expA >= expB)
    if (Shift >=0 ) {
        fracB = fracB >> Shift;
        expR = expA; 
    }
    // when (expA < expB)
    else {
        fracA = fracA >> (~Shift) + 1; 
        expR = expB; 
    }

    // same sign; all negative or all positive
    if(signA == signB) {
        signR = signA;
        fracR = fracA + fracB;
    }
    // different sign (+ or -)
    else {
        if(fracA > fracB) {
            signR = signA;
            fracR = fracA + (~fracB) +1 ;
        }
        else {
            signR = signB;
            fracR = fracB + (~fracA) + 1;
        }
    }

    if(fracR & (1<<11)) {
        expR += 1; //increase the exp
        fracR = fracR >> 1;
    }
    else if(!(fracR & (1<<10)))
    {
        int key = 999;
        for(int i=1; i<=10; i++)
        {
            if(fracR & 1<<(10-i)) {
                key=i;
                break;
            }
        }
        if(key!=999) {
            expR -= key;
            fracR = fracR << key;
        } 
        else expR = 0;
    }
    // through previous process, the final signR determines return value
    // signR => '1' means a<b, '0' means a>b
    signR = signR << 15;

    if(signR==1) {
        return "<";
    } 
    else 
        return ">";
}
