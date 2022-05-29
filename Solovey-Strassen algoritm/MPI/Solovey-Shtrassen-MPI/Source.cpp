#include <iostream>
#include <mpi.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/operators.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <string.h>
#include <random>

#pragma warning(disable: 4996)

using namespace boost::multiprecision;
using namespace std;

int ProcNum;
int ProcCurRank;

std::mt19937 gen(time(0));

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

long long gcd(long long a, long long b) {
    while (a != b) {
        if (a > b)
            a -= b;
        else
            b -= a;
    }
    return a;
}

void processInit(long long& number, int& iterat, int& localCount) {
    int* procIterCount = new int[ProcNum];
    if (ProcCurRank == 0) {
        do {
            printf("\nPlease enter the number you want to check: ");
            std::cin >> number;
            std::cin.get();
            if (number <= 0) {
                printf("\nThe number must be greater than 0!");
            }

        } while (number <= 0);
        do {
            printf("\nPlease enter how many iterations you want to be done: ");
            std::cin >> iterat;
            std::cin.get();
            if (iterat < ProcNum) {
                printf("The number of iterations must be greater than number of processes!\n ");
            }
        } while (iterat < ProcNum);
        int procIterNum = iterat / ProcNum;
        int leftIterats = iterat - (procIterNum * ProcNum);
        for (int i = 0; i < ProcNum; i++)
            procIterCount[i] = procIterNum;
        for (int i = 0; i < leftIterats; i++)
            procIterCount[i] += 1;
    }
    MPI_Bcast(&number, sizeof(number), MPI_LONG_LONG, 0, MPI_COMM_WORLD);
    MPI_Scatter(procIterCount, 1, MPI_INT, &localCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
    delete[] procIterCount;
}



void calculate(long long number, int localCount) {
    std::uniform_int_distribution<long long> distr(2, number - 1);
    for (int i = 0; i < localCount; i++) {
        long long now = distr(gen);
        if (gcd(number, now) > 1) {
            std::cout << "Your number is composite";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        else {
            int ja = symjac(now, number);
            long long temp = number - 1;
            temp /= 2;
            cpp_int ours = now ^ temp;
            ours %= number;
            if (ja >= ours) {
                std::cout << "Your number is composite";
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    setbuf(stdout, NULL);
    long long number;
    std::string bigNum;
    int iterat;
    int procIterNum;
    int localCount;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcCurRank);
    if (ProcCurRank == 0) {
        printf("Parallel Solovey-Shtrassen algorithm\n");
    }
    processInit(number, iterat, localCount);
    double begin = MPI_Wtime();
    calculate(number, localCount);
    double end = MPI_Wtime();
    double duration = end - begin;
    if (ProcCurRank == 0) {
        double prob = 1.0 - 1.0 / (2 ^ iterat);
        std::cout << "The number you've entered is prime with " + std::to_string(prob) + " probability\n";
        std::cout << "Time spent: " + std::to_string(duration) + " seconds\n";
    }
    MPI_Finalize();
    return 0;
}