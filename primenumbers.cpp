#include <iostream>
#include <set>
#include <pthread.h>
#include <cmath>

constexpr int THREADS {4};
constexpr int RANGE {1000000};
constexpr int OFFSET {1000000};

struct thread_args {
    int from;
    int to;
    std::set<int> primes;
};

bool is_prime(int number) {
	if (number < 2 || (number != 2 && number % 2 == 0)) {
		return false;
	}

	int root = std::floor(std::sqrt(number));
	for (int i = 2; i <= root; i++) {
		if (number % i == 0) {
			return false;
		}
	}
	return true;
}

void *on_start_thread(void *parameter) {
    thread_args *args = (thread_args *) parameter;

    for (int i = args->from + 1; i <= args->to; i++) {
    	if (is_prime(i)) {
    		args->primes.insert(i);
    	}
    }

    return nullptr;
}

int main() {
    pthread_t threads[THREADS];
    thread_args args[THREADS];
    float chunk_size = (float) RANGE / (float) THREADS;

    for (size_t i = 0; i < THREADS; i++) {
        args[i].from = OFFSET + std::floor(chunk_size) * i;
        args[i].to = args[i].from + std::ceil(chunk_size);
        int id = pthread_create(&threads[i], nullptr, on_start_thread, &args[i]);

        // For first index the first number won't get checked for prime
        // Dirty fix
        if (i == 0) {
        	--args[i].from;
        }

        if (id) {
            std::cout << "Failed to create thread " << i << std::endl;
            exit(1);
        }
    }

    for (size_t i = 0; i < THREADS; i++) {
        pthread_join(threads[i], nullptr);
    }

    int count = 0;
    for (int i = 0; i < THREADS; i++) {
    	for (auto prime : args[i].primes) {
    		std::cout << prime << std::endl;
    	}
    	count += args[i].primes.size();
    }
    std::cout << count << " primes found" << std::endl;

    return 0;
}
