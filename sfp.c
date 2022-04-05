#include "sfp.h"
#include <stdlib.h>

// 1~4 대충 바꾸긴 함, 변수명 바꾸기, special case 처리하기!
sfp int2sfp(int input){
	int bits[16];
    int indB = 0;

    if(input>=0)
        bits[indB] = 0;
    else {
        bits[indB] = 1;
        input = -input;
    }
    indB++;

    int M[17]; // 16이면 안되낭? 굳이?
    int indM = 0;

    // 완전 동일
    while(input>0 && indM<17)
    {
        M[indM++] = input%2;
        input /= 2;
        // indM++;
    }
    indM--; // 마지막 증가와, 리딩자리 정수 자리 1 빼주기

    int exp;
    exp = indM + 15; // ㅇㅈ E = exp - 15, E = 계산한 m의 index 자리수와 같음

    //요기 내가 수정함 !! 함 바꿔봄 비트말고!! 안되네 왲;? 된당 ㅜㅜ
    for (int i=5; i>0; i--) {
        if(exp !=0) {
            bits[i] = exp%2;
            exp /= 2;
        }
        else {
            bits[i] = 0;
        }
        indB++; // 한자리 채운거니까
    }

    //
    if(exp!=31) {
        indM--; // 마지막 자리 leading 1 이니까 제외
        // 2진수 변환 하여 배열에 담아둔 값 거꾸로 입력
        while(indM>=0 && indB<16) 
            bits[indB++] = M[indM--];

        for(;indB<16;indB++)
            bits[indB] = 0;
    }
    // 최대, EXP 다 1, frac 다 0 <- QQ 0이라고 장담할 수 있나 어차피 INF 아님?
    else {
        for(; indB<16; indB++)
        {
            bits[indB]=0;
        }
    }
    
    sfp result = 0; 
    for(int i=0;i<16;i++) {
        result += bits[i] << (15-i);
    } // 숫자화 이과정 이해가 잘 안되긴 함 똑같이 가져가기

    return result;
}

int sfp2int(sfp input){
    int bits[16]; // 16자리 배열

    for(int i=15; i>=0; i--) {
        if(input!=0) {
            bits[i] = input%2;
            input /= 2;
        }
        else {
            bits[i] = 0;
        }
    }
    
    int exp=0; // 인덱스별로 가중치를 부여하여 exp 연산하기 (2)w - idx 관계가 5차이
    int W_value = 0; // 가중치 부여한 각 자리 값
    
    for(int expi=1; expi<6; expi++) {
        W_value = bits[expi] << (5-expi);
        exp += W_value;
    }
    // 다 1, 뒤에 다 0 사실 뒤 값은 상관 없는듯. 암튼 31 이 파트는 특이 케이스! 조건에 따라 따로 처리
    if(exp == 31)
    {
        for(int fraci=6; fraci<16; fraci++)
        {
            if(bits[fraci]) return -214783648; //NaN 값 임의 -> TMin
        }
        if(!bits[0]) return 2147483647; // + 무한대, TMax : 2^(w-1)-1
        else return -2147483647; // -무한대 , TMin : -2^(w-1)
    }
    
    // 값비교 함수 가서 확인해보깅
    // if(exp == 31)
    // {
    //     for(int fraci=6; fraci<16; fraci++)
    //     {
    //         if(bits[fraci]) return -32768; //NaN 값 임의 -> TMin
    //     }
    //     if(!bits[0]) return 32767; // + 무한대, TMax : 2^(w-1)-1 w가 비트 수라면 32767
    //     else return -32768; // -무한대 , TMin : -2^(w-1) -32768
    // }

    int E =exp-15;
    if(E<0) return 0;

    // int v=0;
    // v+= 1<< E; // v = (-1)^s * M * 2^E --> 2의 e제곱 형태로 바꿔주기
    // int c = E-1; // E는 (M - 1) 정수자리 한 자리 제외.
    // for(int fraci = 6; fraci<6+E && fraci <16; fraci++) {
    //     v += bits[fraci] << c;
    //     c--;
    // } // 더 고민해보기

    // 바꾸긴 했음
    int v = 0;
    int temp = E-1;
    int W= 1<<E;
    for(int fraci = 6; fraci < 16 && fraci < 6+E; fraci++) {
        W += bits[fraci] << temp;
        temp--;
    }
    v = W << E >> E;

    if(bits[0]) v=-v; //부호체크

    return v;
}

sfp float2sfp(float input){
    int bits[16];
    int indB = 0;

    // sign bit 설정
    if(input>=0)
        bits[indB] = 0;
    else {
        bits[indB] = 1;
        input = -input;
    }
    indB++;

    int ipart = (int) input; // 정수부분
    float fpart = input - ipart; // 소수부분

    int integer[17]; // 정수 -> 이진수화
    int indI = 0;

    // 정수부분 배열에 담기 -- 앞에 정수변환과 동일
    while(ipart>0 && indI<17) {
        integer[indI++] = ipart%2;
        ipart/=2;// ipart 0 이면 다 0 으로 넣기
    }
    indI--;

    // 소수부분 배열에 담기 -- 
    int fractional[15]; // 소수부분 -> 이진수화
    int indF = 0; // 배열 인덱스
    int posF = 999; // 1 찾기 LeadNum

    // 소수부분 존재
    while(fpart && indF<15) {
        fpart*=2;
        fractional[indF++] = (int) fpart;
        fpart -= (int) fpart;

        if(indI<0) {
            for(int i=0; i<15; i++) {
                if(fractional[i]) {
                    posF = i;
                    break;
                }
            }
        }
    }

    int exp;
    int E;

    // 정수부분이 존재하는 경우
    if(indI>=0) {
        E = indI;
        exp = E+15;
    } 
    // 정수 존재 X
    else {
        if(posF!=999) {
            E = -(posF+1);
            exp= E + 15;
        } //소수부분에 1이 존재하는 경우
        else 
            exp=0; // 0인 경우
    }
    
    // 내가 수정함 int2Sfp 때랑 같은 맥락~ 거꾸로 담기!
    for (int i=5; i>0; i--) {
        if(exp !=0) {
            bits[i] = exp%2;
            exp /= 2;
        }
        else {
            bits[i] = 0;
        }
        indB++; // 한자리 채운거니까
    }

    // 예외 케이스 --> 조건에 맞추기
    if(exp == 31)
    {
        for(;indB<16;indB++)
        {
            bits[indB] = 0;
        }
    }

    else {
        indI--; // M 앞에 리딩1 빼고
        while (indI>=0 && indB<16)
            bits[indB++] = integer[indI--];

        int fraci = 0; // 차례로 담겨있음 순서대로 담기

        if(posF!=999) 
            fraci = posF+1; // 소수부만 있는 경우, posF가 M의 리딩 1 이 되니까 그 다음부터!
        
        for(;fraci<indF && indB<16; fraci++)
            bits[indB++] = fractional[fraci];
        
        for (;indB<16;indB++)
            bits[indB] = 0;
    }

    sfp result = 0;
    for(int i=0; i<16; i++)
        result += bits[i] << (15-i);
    // 그냥 갖고 가기

    return result;
}


float sfp2float(sfp input){
    
    int bits[16];
    // 내가 수정 위에 함수들과 같은 맥락
    for(int i=15; i>=0; i--) {
        if(input!=0) {
            bits[i] = input%2;
            input /= 2;
        }
        else 
            bits[i] = 0;
    }

    int exp=0; // 가중치 부여하며 exp 연산하기
    int W_value = 0; // 가중치 부여한 각 자리 값

    for(int expi=1; expi<6; expi++) {
        W_value = bits[expi] << (5-expi);
        exp += W_value;
    }

    int E = exp - 15; // 공식 적용하여 E 구하기

    float v = 0; // float v = 0f;
    int c,fraci = 6;
    if(exp==0) // when exp ==0, denormarlized 특별 케이스 따로 처리
        c=-E;
    else if(E<0) // 소수부분만 있는 경우
    {
        c=-E;
        // v+= (float) 1 / (float) (1 <<c); // 2^E 부분 처리
         v+= 1 / (float) (1 <<c);
    }
    else{ // 정수부분 존재
        // 정수부분 처리
        c = E-1;
        v+= 1<< E;
        for(;fraci<6+E && fraci<16; fraci++) {
            v += bits[fraci] <<c ;
            c--;
        }
        c++; // c 0으로 만들기
    }
    c++; // c 1로 만들기 , 소수부분 처리하려고
    for(;fraci<16;fraci++) {
        if(bits[fraci]) v+= 1 / (float) (bits[fraci]<<c);
        c++;
    }

    if(bits[0]) v = -v;

    return v;
}

sfp sfp_add(sfp a, sfp b){

    // 15칸 이동시키면 부호비트만 남게됨
    unsigned short signA, signB;
    signA = a >> 15; 
    signB = b >> 15;

    unsigned short expA, expB, mntA, mntB;
    expA = a << 1; expA = expA >> 11; // 5개만 남기기
    expB = b << 1; expB = expB >> 11;

    mntA = a << 6; mntA = mntA >> 6;    //m만 남기기
    mntB = b << 6; mntB = mntB >> 6;

    sfp result = 0;
    unsigned short sR, expR, mntR;

    // 특별 케이스 나중에 확인
    sfp NaN = 31<<10 + 1; // +가 우선함
    // 둘다 스페셜 케이스
    //a,b 둘다 inf
    if(expA==31 && expB==31) {
        // 둘 중에 하나라도 frac에 1이 존재한다면
        if(mntA || mntB) return NaN;
        // 두 부호가 다르다면,
        else if(signA != signB) return NaN;
        else return a; //b여도 상관 X
    }
    else if (expA==31 || expB==31) {
        // a만 inf
        if (expA == 31) return a;
        // b만 inf
        else return b;
    }

    // M : leading 1이니까!
    if(expA!=0) mntA += (1<<10);
    if(expB!=0) mntB += (1<<10);
    
    // 이동 칸
    short shftP = expA + (~expB) + 1; // 빼깅 1의 보수 활용
    // a가 더 클 때
    if (shftP >=0 ) {
        mntB = mntB >> shftP;
        expR = expA; // a지수로
    }
    // b가 더 클 때
    else {
        mntA = mntA >> (~shftP)+1; //개중요라인
        expR = expB; // b지수로
    }

    if(signA == signB) {
        sR = signA;
        mntR = mntA + mntB;
    }
    else {
        // A가 더 클 때
        if(mntA > mntB) {
            sR = signA;
            mntR = mntA + (~mntB) +1 ;
        }
        // B가 더 클 때
        else {
            sR = signB;
            mntR = mntB + (~mntA) + 1;
        }
    }

    // 해당 자리 값이 같냐 뒤에서부터 12번째자리 mntR 1이냐
    if(mntR & (1<<11)) {
        expR += 1; // 올림수
        // if((mntR&1) && ((mntR-1)%4)) mntR += 1; round-to-even ? 뭐라냐
        mntR = mntR >> 1;
    }
    // 해당 자리 값이 같냐 뒤에서부터 11번째자리 mntR 1이냐
    else if(!(mntR & (1<<10)))
    {
        int k = 999;
        for(int i=1; i<=10; i++)
        {
            if(mntR & 1<<(10-i)) {
                k=i;
                break;
            }
        }
        if(k!=999) {
            expR -= k;
            mntR = mntR << k;
        } 
        else expR = 0;
    }
    sR = sR << 15;
    expR = expR << 10;
    mntR -= ( mntR&(1<<10) );

    result = sR + expR + mntR;

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


char* sfp_comp(sfp a, sfp b){
   unsigned short signA, signB;
    signA = a >> 15; 
    
    signB = b >> 15;


    unsigned short expA, expB, mntA, mntB;
    expA = a << 1; expA = expA >> 11; // 5개만 남기기
    expB = b << 1; expB = expB >> 11;

    mntA = a << 6; mntA = mntA >> 6;    //m만 남기기
    mntB = b << 6; mntB = mntB >> 6;

    if(signA==signB && expA==expB && mntA==mntB)
        return "=";

    signB = (~signB); // b 부호 바꾸기

    unsigned short sR, expR, mntR;

    // 특별 케이스 나중에 확인
    // sfp NaN = 31<<10 + 1; // +가 우선함
    // // 둘다 스페셜 케이스
    // //a,b 둘다 inf
    // if(expA==31 && expB==31) {
    //     // 둘 중에 하나라도 frac에 1이 존재한다면
    //     if(mntA || mntB) return NaN;
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

    // M : leading 1이니까!
    if(expA!=0) mntA += (1<<10);
    if(expB!=0) mntB += (1<<10);
    
    // 이동 칸
    short shftP = expA + (~expB) + 1; // 빼깅 1의 보수 활용
    // a가 더 클 때
    if (shftP >=0 ) {
        mntB = mntB >> shftP;
        expR = expA; // a지수로
    }
    // b가 더 클 때
    else {
        mntA = mntA >> (~shftP)+1; //개중요라인
        expR = expB; // b지수로
    }

    if(signA == signB) {
        sR = signA;
        mntR = mntA + mntB;
    }
    else {
        // A가 더 클 때
        if(mntA > mntB) {
            sR = signA;
            mntR = mntA + (~mntB) +1 ;
        }
        // B가 더 클 때
        else {
            sR = signB;
            mntR = mntB + (~mntA) + 1;
        }
    }

    // 해당 자리 값이 같냐 뒤에서부터 12번째자리 mntR 1이냐
    if(mntR & (1<<11)) {
        expR += 1; // 올림수
        // if((mntR&1) && ((mntR-1)%4)) mntR += 1; round-to-even ? 뭐라냐
        mntR = mntR >> 1;
    }
    // 해당 자리 값이 같냐 뒤에서부터 11번째자리 mntR 1이냐
    else if(!(mntR & (1<<10)))
    {
        int k = 999;
        for(int i=1; i<=10; i++)
        {
            if(mntR & 1<<(10-i)) {
                k=i;
                break;
            }
        }
        if(k!=999) {
            expR -= k;
            mntR = mntR << k;
        } 
        else expR = 0;
    }
    sR = sR << 15;

    if(sR==1) {
        return "<";
    } 
    else 
        return ">";
}
