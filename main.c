/*Быстрое преобразование Фурье (FFT) может быть реализовано на целых числах с помощью алгоритма Кули-Тьюки. Представляем пример реализации на языке с:```*/

#include <stdio.h>
#include <math.h>
#define N 8

void fft(int n, int x_real[], int x_imag[], int X_real[], int X_imag[])
 {    if (n == 1) 
{        X_real[0] = x_real[0];        X_imag[0] = x_imag[0];        return;   
 }        

int x_even_real[n/2], x_even_imag[n/2], x_odd_real[n/2], x_odd_imag[n/2];   

 int X_even_real[n/2], X_even_imag[n/2], X_odd_real[n/2], X_odd_imag[n/2];        

for (int i = 0; i < n/2; i++) {        x_even_real[i] = x_real[2*i];        x_even_imag[i] = x_imag[2*i];        x_odd_real[i] = x_real[2*i+1];        x_odd_imag[i] = x_imag[2*i+1];    }        

fft(n/2, x_even_real, x_even_imag, X_even_real, X_even_imag);   

 fft(n/2, x_odd_real, x_odd_imag, X_odd_real, X_odd_imag);       

 for (int k = 0; k < n; k++) {      
  int kth = -2 * M_PI * k / n * pow(2, ceil(log2(N)));        
int W_real = cos(kth);       
 int W_imag = sin(kth);       
 X_real[k] = X_even_real[k % (n/2)] + W_real * X_odd_real[k % (n/2)] - W_imag * X_odd_imag[k % (n/2)];       
 X_imag[k] = X_even_imag[k % (n/2)] + W_real * X_odd_imag[k % (n/2)] + W_imag * X_odd_real[k % (n/2)];    }}

int main() {    
int x_real[N] = {0, 1, 2, 3, 4, 5, 6, 7};    int x_imag[N] = {0, 0, 0, 0, 0, 0, 0, 0};    int X_real[N], X_imag[N];       

 fft(N, x_real, x_imag, X_real, X_imag);        

for (int i = 0; i < N; i++) {        printf(%d +%di\n, X_real[i], X_imag[i]);    }       
 return 0;
}