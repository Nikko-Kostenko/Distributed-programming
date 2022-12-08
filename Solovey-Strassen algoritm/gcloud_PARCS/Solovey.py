from Pyro4 import expose
import random

class Solver:
    def __init__(self, workers=None, input_file_name=None, output_file_name=None):
        self.input_file_name = input_file_name
        self.output_file_name = output_file_name
        self.workers = workers
        print("Inited")

    def solve(self):
        print("Job Started")
        print("Workers %d" % len(self.workers))

        (n, k, c) = self.read_input()
        a = 1 << n
        b = 1 << (n + 1)
        iter = c
        step_n = (b - a) / len(self.workers)
        step_k = k / len(self.workers)

        # map
        mapped = []
        for i in xrange(0, len(self.workers)):
            print("map %d" % i)
            mapped.append(self.workers[i].mymap(str(a + i * step_n), str(a + (i + 1) * step_n), step_k, iter))

        # reduce
        primes = self.myreduce(mapped)

        # output
        self.write_output(primes)

        print("Job Finished")

    @staticmethod
    @expose
    def mymap(a, b, count, iter):
        print(a)
        print(b)
        print(count)
        print(iter)
        a = int(a)
        b = int(b)
        iter = int(iter)
        primes = []

        if a % 2 == 0:
            a += 1

        while len(primes) < count and a < b:
            if Solver.solovoyStrassen(a, iter):
                primes.append(str(a))
            a += 2

        return primes

    def read_input(self):
        f = open(self.input_file_name, 'r')
        n = int(f.readline())
        k = int(f.readline())
        c = int(f.readline())
        f.close()
        return n, k, c

    def write_output(self, output):
        f = open(self.output_file_name, 'w')
        f.write(', '.join(output))
        f.write('\n')
        f.close()
        print("output done")

    @staticmethod
    @expose
    def myreduce(mapped):
        print("reduce")
        output = []

        for primes in mapped:
            print("reduce loop")
            output = output + primes.value
        print("reduce done")
        return output

    @staticmethod
    @expose
    def solovoyStrassen(p, iterations):
        if (p < 2):
            return False;
        if (p != 2 and p % 2 == 0):
            return False;

        def calculateJacobian(a, n):
            if (a == 0):
                return 0;  # (0/n) = 0

            ans = 1;
            if (a < 0):

                # (a/n) = (-a/n)*(-1/n)
                a = -a;
                if (n % 4 == 3):
                    # (-1/n) = -1 if n = 3 (mod 4)
                    ans = -ans;

            if (a == 1):
                return ans;  # (1/n) = 1

            while (a):
                if (a < 0):

                    # (a/n) = (-a/n)*(-1/n)
                    a = -a;
                    if (n % 4 == 3):
                        # (-1/n) = -1 if n = 3 (mod 4)
                        ans = -ans;

                while (a % 2 == 0):
                    a = a // 2;
                    if (n % 8 == 3 or n % 8 == 5):
                        ans = -ans;

                # swap
                a, n = n, a;

                if (a % 4 == 3 and n % 4 == 3):
                    ans = -ans;
                a = a % n;

                if (a > n // 2):
                    a = a - n;

            if (n == 1):
                return ans;

            return 0;

        def modulo(base, exponent, mod):
            x = 1;
            y = base;
            while (exponent > 0):
                if (exponent % 2 == 1):
                    x = (x * y) % mod;

                y = (y * y) % mod;
                exponent = exponent // 2;

            return x % mod;

        for i in range(iterations):

            # Generate a random number a
            a = random.randrange(p - 1) + 1;
            jacobian = (p + calculateJacobian(a, p)) % p;
            mod = modulo(a, (p - 1) / 2, p);

            if (jacobian == 0 or mod != jacobian):
                return False;

        return True;
