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
  long long range = interval->right - interval->left + 1;
  long long  gap = range / 100;
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
  struct Interval It1 = {2,       200000000,   0};
  HANDLE hHandles;
  DWORD Id;
  hHandles = CreateThread(NULL, 512, (LPTHREAD_START_ROUTINE)countPrime, &It1, 0, &Id);
  printf("Thread 1-1 created with ID: %d\n", Id);

  WaitForSingleObject(hHandles, INFINITE);
  // printf("Number of prime numbers in the interval [2, 2000000] is %d\n", It1.count);
  return 0;
}

int TwoThreads() {
  struct Interval It1 = {2,       130000000,  0},
                  It2 = {130000001, 200000000,  0};
  
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
  struct Interval It1 = {2,       100000000,  0},
                  It2 = {100000001, 150000000,  0},
                  It3 = {150000001, 200000000,  0};
  
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
  struct Interval It1 = {2,       90000000,   0},
                  It2 = {90000001,  140000000,  0},
                  It3 = {140000001, 173000000,  0},
                  It4 = {173000001, 200000000,  0};
  
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