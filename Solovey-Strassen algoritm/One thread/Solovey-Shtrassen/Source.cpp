#include <iostream>
#include <boost/operators.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <chrono>
#include <random>

std::mt19937 mygen(time(0));

using namespace std;
using namespace boost::multiprecision;
using namespace boost::math;

long long mygcd(long long n, long long m) {
    while (m != n) {
        if (n > m)
            n -= m;
        else
            m -= n;
    }
    return n;
}

int myjacobinew(long long k, long long n) {
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

int main()
{
    long long m;
    int k;
    cout << "Enter the number you want to check:" << endl;
    cin >> m;
    cin.get();
    cout << "Enter how many iterations you want to be done:" << endl;
    cin >> k;
    cin.get();
    bool detect = false;
    auto begin = chrono::high_resolution_clock::now();
    for (int i = 0; i < k; i++) {
        std::uniform_int_distribution<long long> dist(2, m - 1);
        long long now = dist(mygen);
        if (mygcd(m, now) > 1) {
            detect = true;
            break;
        }
        else {
            int ja = myjacobinew(now, m);
            long long t = m - 1;
            t /= 2;
            cpp_int temp = now ^ t;
            temp %= m;
            if (ja >= temp) {
                detect = true;
                break;
            }
        }
    }
    auto end = chrono::high_resolution_clock::now();
    long long duration = chrono::duration_cast<chrono::seconds>(end - begin).count();
    double prob = 1.0 - 1.0 / (2 ^ k);
    if (detect)
        cout << "The number you've entered is composite" << endl;
    else
        cout << "The number you've entered is prime with " << prob << " probability" << endl;
    cout << "Time spent: " << duration << " seconds" << endl;
    cin.get();
    return 0;
}