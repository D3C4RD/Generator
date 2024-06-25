#include "stribog.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define INTERVALS 256

int get_rand(uint8_t* IV,int n)
{
    int FD=open("/dev/urandom",O_RDONLY);
    if(FD<0)
    {
        printf("FILE /dev/urandom didnt open\n");
        return -1;
    }
    if(read(FD,IV,n)<0)
    {
        printf("Can't read file\n");
        close(FD);
        return -2;
    }
    close(FD);
    return 0;
}

int chi_squared_test(uint8_t *data, int n) {
    int observed[INTERVALS] = {0}; // Массив для наблюдаемых частот
    double expected = (double)n / INTERVALS; // Ожидаемая частота для равномерного распределения
    double crit=293.2478350807012;
    // Подсчет наблюдаемых частот
    for (int i = 0; i < n; i++) {
        observed[data[i]]++;
    }

    // Вычисление хи-квадрат статистики
    double chi_squared = 0;
    for (int i = 0; i < INTERVALS; i++) {
        chi_squared += pow(observed[i] - expected, 2) / expected;
    }
    if(chi_squared>crit)
    {
        //printf("%f\n",chi_squared);
        return 0;
    }
    return 1;
}

void gen_block(uint8_t block[64])
{
    uint8_t seed[100];
    uint8_t h[64];
    
    double t;
    int i=1;
    do
    {
        get_rand(seed,100);
        get512(seed,100,h);
        //printf("try:%d\n",i);
        i++;
    }while(!chi_squared_test(h,64));
    copy(h,0,block,0,64);
}

void gen_m(uint8_t m[],int blocks)
{
    uint8_t block[64];
    int j=1;
    uint8_t temp[64*blocks];
    do
    {
        printf("try%d:\n",j);
        for(int i=0;i<blocks;i++)
        {
            gen_block(block);
            copy(block,0,temp,64*i,64);
        }
        j++;
    }while(!chi_squared_test(temp,64*blocks));
    copy(temp,0,m,0,64*blocks);
}
