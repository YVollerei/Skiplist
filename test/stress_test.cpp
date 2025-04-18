#include "../skiplist.h"
#include <iostream>
#include <string>
#include <chrono>
#include <cstdlib>
#include <pthread.h>
#include <functional>
#include <time.h>


#define NUM_THREADS 8
#define TEST_COUNT 100000
SkipList<int, std::string> skipList(32, 0.25);


void *insertElement(void* threadid) {
    long tid; 
    tid = (long)threadid;
    // std::cout << tid << std::endl;  
    int tmp = TEST_COUNT/NUM_THREADS; 
	for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
		skipList.insertNode(rand() % TEST_COUNT, "a"); 
	}
    pthread_exit(NULL);
}

void *getElement(void* threadid) {
    long tid; 
    tid = (long)threadid;
    // std::cout << tid << std::endl;  
    int tmp = TEST_COUNT/NUM_THREADS; 
	for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
		skipList.searchNode(rand() % TEST_COUNT); 
	}
    pthread_exit(NULL);
}

int main() {
    std::cout << "Stress Test: NUM_THREADS=" << NUM_THREADS << ", TEST_COUNT=" << TEST_COUNT << std::endl;

    srand (time(NULL));  
    {
        std::cout << "-----test insert-----" << std::endl;
        pthread_t threads[NUM_THREADS];
        int rc;
        int64_t i;

        auto start = std::chrono::high_resolution_clock::now();

        for( i = 0; i < NUM_THREADS; i++ ) {
            // std::cout << "main() : creating thread, " << i << std::endl;
            rc = pthread_create(&threads[i], NULL, insertElement, (void*)i);

            if (rc) {
                std::cout << "Error:unable to create thread," << rc << std::endl;
                exit(-1);
            }
        }

        void *ret;
        for( i = 0; i < NUM_THREADS; i++ ) {
            if (pthread_join(threads[i], &ret) !=0 )  {
                perror("pthread_create() error"); 
                exit(3);
            }
        }
        auto finish = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "insert elapsed:" << elapsed.count() << std::endl;
    }
    // skipList.displayList();

    {
        std::cout << "\n-----test search-----" << std::endl;
        pthread_t threads[NUM_THREADS];
        int rc;
        int64_t i;
        auto start = std::chrono::high_resolution_clock::now();

        for( i = 0; i < NUM_THREADS; i++ ) {
            // std::cout << "main() : creating thread, " << i << std::endl;
            rc = pthread_create(&threads[i], NULL, getElement, (void *)i);

            if (rc) {
                std::cout << "Error:unable to create thread," << rc << std::endl;
                exit(-1);
            }
        }

        void *ret;
        for( i = 0; i < NUM_THREADS; i++ ) {
            if (pthread_join(threads[i], &ret) !=0 )  {
                perror("pthread_create() error"); 
                exit(3);
            }
        }

        auto finish = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "get elapsed:" << elapsed.count() << std::endl;
    }

	pthread_exit(NULL);
    return 0;

}