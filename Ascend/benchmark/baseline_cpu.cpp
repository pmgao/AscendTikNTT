#include <algorithm>
#include <chrono>
#include <gmpxx.h>
#include <iostream>
using namespace std;

const int rangeSzie = 1<<20; //模数为p，数组长度限制为N
const mpz_class prime= 4179340454199820289;
const mpz_class root = 1394649864822396625;
const mpz_class iroot = 159035048546431000;

mpz_class * bitReverseCopy(const mpz_class * src){
    mpz_class * dst = new mpz_class[rangeSzie];

    for(int i = 0; i < rangeSzie; i++){
      int rev_i=0;
      for(int j = 0; j < 20; j++){
        rev_i<<=1;
        rev_i |= (i>>j)&1;
      }       
      dst[i] = src[rev_i];
    }
    return dst;
}

mpz_class modexp(mpz_class a, mpz_class b) {

    mpz_class res = 1;
    a = a % prime;  // 处理 a >= m 的情况
    while (b > 0) {
        if (b % 2 == 1) {
          res = (res * a) % prime;
        }
        a = (a * a) % prime;
        b /= 2;
    }
    return res;
}

mpz_class * ntt(mpz_class *data) {
  data = bitReverseCopy(data);
  mpz_class gn = root;
  for(int i = 1; i <= 20; i++){
    int m=1<<i;
    mpz_class gm = modexp(gn,rangeSzie/m);
    for(int j = 0; j < rangeSzie; j+=m){
      mpz_class g = 1;
      for(int k=0; k < m/2; k++){
        mpz_class u = data[k+j]%prime;
        mpz_class t = (data[k+j+m/2]*g)%prime;
        data[k+j] = (u+t) %prime;
        data[k+j+m/2] = (u-t);
        while(data[k+j+m/2]<0)
        {
          data[k+j+m/2] += prime;
        }
        data[k+j+m/2] = data[k+j+m/2] %prime;

        g = g*gm % prime;
      } 
    }
  }

  return data;
}

mpz_class * intt(mpz_class *data) {
  data = bitReverseCopy(data);
  mpz_class gn = iroot;
  for(int i = 1; i <= 20; i++){
    int m=1<<i;
    mpz_class gm = modexp(gn,rangeSzie/m);
    for(int j = 0; j < rangeSzie; j+=m){
      mpz_class g = 1;
      for(int k=0; k < m/2; k++){
        mpz_class u = data[k+j]%prime;
        mpz_class t = (data[k+j+m/2]*g)%prime;
        data[k+j] = (u+t)%prime;
        data[k+j+m/2] = (u-t);
        while(data[k+j+m/2]<0)
        {
          data[k+j+m/2] += prime;
        }
        data[k+j+m/2] = data[k+j+m/2] %prime;
        g = g*gm % prime;
      }
    }
  }
  mpz_class inv_n = modexp(rangeSzie, prime - 2);
  for (int i = 0; i < rangeSzie; i++) {
    data[i] = (data[i] * inv_n) % prime;
  }
  return data;
}

int main() {

  mpz_class * data_a = new mpz_class[rangeSzie];
  mpz_class * data_b = new mpz_class[rangeSzie];
  for (int i = 0; i < rangeSzie/2; i++) {
     data_a[i] = 1;
     data_b[i] = 1;
  }

  auto start = chrono::high_resolution_clock::now();
  data_a = ntt(data_a);
  auto end = chrono::high_resolution_clock::now();
  cout<<"data_a: "<<"[";
  for(int i = 0;i<10;i++){
    cout<<data_a[i]<<", ";
  }
  chrono::duration<double, nano> elapsed = end - start;

  cout << "baseline_cpu " << elapsed.count() / 1000000
       << " ms \n"; // 计算两次NTT的总时间，单位为纳秒 
  
  data_b = ntt(data_b);

  mpz_class * data_c = new mpz_class[rangeSzie];
  for (int i = 0; i < rangeSzie; i++) {
     data_c[i] =(data_a[i]*data_b[i])%prime;
  }
  data_c = intt(data_c);

  cout<<"data_c: "<<"[";
  for(int i = 0;i<10;i++){
    cout<<data_c[i]<<", ";
  }
  cout<<"]"<<endl; 

  return 0;
}
