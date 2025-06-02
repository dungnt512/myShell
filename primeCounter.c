#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>

struct Interval {
  long long left, right, count;
};

bool checkPrime(long long n) {
  if (n < 2) {
    return false;
  }
  if (n == 2 || n == 3) {
    return true;
  }
  if (n % 2 == 0 || n % 3 == 0) {
    return false;
  }

  for (long long i = 5; i * i <= n; i += 6) {
    if (n % i == 0 || n % (i + 2) == 0) {
      return false;
    }
  }

  return true;
}

void countPrime(struct Interval *interval) {
  int range = interval->right - interval->left + 1;
  int gap = range / 100;
  for (long long i = interval->left; i <= interval->right; i++) {
    if (i % gap == 0) {
      printf("Thread %d is processing number %lld, progress: %d%%\n", GetCurrentThreadId(), i, (i - interval->left) * 100 / range);
    }
    if (checkPrime(i)) {
      interval->count++;  
    }
  }
  printf("Thread %d finished\n", GetCurrentThreadId());
}

int OneThreads() {
  struct Interval It1 = {2,       2000000,   0};
  HANDLE hHandles;
  DWORD Id;
  hHandles = CreateThread(NULL, 512, (LPTHREAD_START_ROUTINE)countPrime, &It1, 0, &Id);
  printf("Thread 1-1 created with ID: %d\n", Id);

  WaitForSingleObject(hHandles, INFINITE);
  // printf("Number of prime numbers in the interval [2, 2000000] is %d\n", It1.count);
  return 0;
}

int TwoThreads() {
  struct Interval It1 = {2,       1300000,  0},
                  It2 = {1300001, 2000000,  0};
  
  HANDLE hHandles[2];
  DWORD Id;
  hHandles[0] = CreateThread(NULL, 512, (LPTHREAD_START_ROUTINE)countPrime, &It1, 0, &Id);
  printf("Thread 2-1 created with ID: %d\n", Id);
  hHandles[1] = CreateThread(NULL, 512, (LPTHREAD_START_ROUTINE)countPrime, &It2, 0, &Id);
  printf("Thread 2-2 created with ID: %d\n", Id);

  WaitForMultipleObjects(2, hHandles, TRUE, INFINITE);
  // printf("Number of prime numbers in the interval [2, 2000000] is %d\n", It1.count + It2.count);
  return 0;
}

int ThreeThreads() {
  struct Interval It1 = {2,       1000000,  0},
                  It2 = {1000001, 1500000,  0},
                  It3 = {1500001, 2000000,  0};
  
  HANDLE hHandles[3];
  DWORD Id;
  hHandles[0] = CreateThread(NULL, 512, (LPTHREAD_START_ROUTINE)countPrime, &It1, 0, &Id);
  printf("Thread 3-1 created with ID: %d\n", Id);
  hHandles[1] = CreateThread(NULL, 512, (LPTHREAD_START_ROUTINE)countPrime, &It2, 0, &Id);
  printf("Thread 3-2 created with ID: %d\n", Id);
  hHandles[2] = CreateThread(NULL, 512, (LPTHREAD_START_ROUTINE)countPrime, &It3, 0, &Id);
  printf("Thread 3-3 created with ID: %d\n", Id);

  WaitForMultipleObjects(3, hHandles, TRUE, INFINITE);
  // printf("Number of prime numbers in the interval [2, 2000000] is %d\n", It1.count + It2.count + It3.count);
  return 0;
}

int FourThreads() {
  struct Interval It1 = {2,       900000,   0},
                  It2 = {900001,  1400000,  0},
                  It3 = {1400001, 1730000,  0},
                  It4 = {1730001, 20000000000,  0};
  
  HANDLE hHandles[4];
  DWORD Id;
  hHandles[0] = CreateThread(NULL, 512, (LPTHREAD_START_ROUTINE)countPrime, &It1, 0, &Id);
  printf("Thread 4-1 created with ID: %d\n", Id);
  hHandles[1] = CreateThread(NULL, 512, (LPTHREAD_START_ROUTINE)countPrime, &It2, 0, &Id);
  printf("Thread 4-2 created with ID: %d\n", Id);
  hHandles[2] = CreateThread(NULL, 512, (LPTHREAD_START_ROUTINE)countPrime, &It3, 0, &Id);
  printf("Thread 4-3 created with ID: %d\n", Id);
  hHandles[3] = CreateThread(NULL, 512, (LPTHREAD_START_ROUTINE)countPrime, &It4, 0, &Id);
  printf("Thread 4-4 created with ID: %d\n", Id);
  WaitForMultipleObjects(4, hHandles, TRUE, INFINITE);
  // printf("Number of prime numbers in the interval [2, 2000000] is %d\n", It1.count + It2.count + It3.count + It4.count);
  return 0;
}

int main() {
  OneThreads();
  TwoThreads();
  ThreeThreads();
  FourThreads();
  return 0;
}