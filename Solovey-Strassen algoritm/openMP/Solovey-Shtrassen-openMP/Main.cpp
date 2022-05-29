#include <iostream>
#include <stdlib.h>
#include <omp.h>
#include <chrono>
#include <random>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/operators.hpp>

std::mt19937 mygen(time(0));

using namespace boost::multiprecision;

int symjac(long long k, long long n) {
    int jac = 1;
    if (k < 0) {
        k = -k;
        if (n % 4 == 3)
            jac = -jac;
    }
    while (k != 0) {
        long long t = 0;
        while (k % 2 == 0) {
            t += 1;
            k /= 2;
        }
        if (t % 2 == 1) {
            if (n % 8 == 3 || n % 8 == 5)
                jac = -jac;
        }
        if (k % 4 == 3 && n % 4 == 3)
            jac = -jac;
        long long c = k;
        k = n % c;
        n = c;
    }
    return jac;
}

long long gcd(long long n, long long m) {
    while (m != n) {
        if (n > m)
            n -= m;
        else
            m -= n;
    }
    return n;
}

void init(long long& number, int &iterat)
{
    while (true) {
        std::cout << "Please enter the number to check: " << std::endl;
        std::cin >> number;
        std::cin.get();
        if (number <= 0)
            std::cout << "The number must be greater than 0!" << std::endl;
        else
            break;
    }
    while (true) {
        std::cout << "Please enter how many iterations you want to be done: " << std::endl;
        std::cin >> iterat;
        std::cin.get();
        if (iterat <= 0)
            std::cout << "The number of iterations must be greater than 0!" << std::endl;
        else
            break;
    }
}

int main(void) {
    long long number;
    int iterat;
    init(number, iterat);
    bool detect = false;
    auto begin = std::chrono::high_resolution_clock::now();

    
#pragma omp parallel for
    for (int i = 0; i < iterat; i++) {
        if (detect) continue;
        std::uniform_int_distribution<long long> dist(2, number - 1);
        long long now = dist(mygen);
        if (gcd(number, now) > 1) {
            detect = true;
            
        }
        else {
            int ja = symjac(now, number);
            long long t = number - 1;
            t /= 2;
            cpp_int temp = now ^ t;
            temp %= number;
            if (ja >= temp) {
                detect = true;
                
            }
        }
        
    }

    auto end = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    double probability = 1.0 - 1.0 / (2 ^ iterat);
    if (detect)
        std::cout << "The number is composite" << std::endl;
    else
        std::cout << "The number is probably prime with probability of " << probability << std::endl;
    std::cout << "Time spent: " << duration << " milliseconds" << std::endl;
    std::cin.get();
    return 0;
}