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
	
}

sfp sfp_mul(sfp a, sfp b){
	
}

char* sfp2bits(sfp result){

}

char* sfp_comp(sfp a, sfp b){

}
