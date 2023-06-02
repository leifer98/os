#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include "active_object.hpp"
#include "prime.cpp"
#include "safe_queue.hpp"

using namespace std;

std::shared_ptr<ActiveObject> AO1;
std::shared_ptr<ActiveObject> AO2;
std::shared_ptr<ActiveObject> AO3;
std::shared_ptr<ActiveObject> AO4;

int main(int argc, char *argv[])
{
    if (argc < 3 || argc > 3)
    {
        std::cerr << "Usage: " << argv[0] << " N [SEED]" << std::endl;
        return 1;
    }
    int seed = std::atoi(argv[2]);
    int repeats = std::atoi(argv[1]);
    int tuple[] = {seed, repeats};

    AO1 = std::make_shared<ActiveObject>();
    AO2 = std::make_shared<ActiveObject>();
    AO3 = std::make_shared<ActiveObject>();
    AO4 = std::make_shared<ActiveObject>();

    AO1->setTask([](void *data)
                 {
                    int repeats = *((int*)data + 1);
                    int seed = *(int*)data;

                    std::srand(seed);
                     for (int i = 0 ; i < repeats ; i++) {
                        int num = std::rand() % 900000 + 100000; // Generate 6-digit number
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        AO2->getQueue()->enqueue(new int(num));
                     } });

    AO2->setTask([](void *data)
                 {
        int num = *(int*)data;
        std::cout << num << std::endl << std::boolalpha << isPrime(num) << std::endl;
        num += 11;
        AO3->getQueue()->enqueue(new int(num));
        delete (int*)data; });

    AO3->setTask([](void *data)
                 {
        int num = *(int*)data;
        std::cout << num << std::endl << std::boolalpha << isPrime(num) << std::endl;
        num -= 13;
        AO4->getQueue()->enqueue(new int(num));
        delete (int*)data; });

    AO4->setTask([](void *data)
                 {
        int num = *(int*)data;
        std::cout << num << std::endl << std::boolalpha << isPrime(num) << std::endl;
        num += 2;
        std::cout << num << std::endl;
        delete (int*)data; });

    // Start the AOs
    AO1->getQueue()->enqueue(&tuple);
    AO1->start();
    AO2->start();
    AO3->start();
    AO4->start();

    // Ensure the main thread doesn't finish until the AOs are done
    // std::this_thread::sleep_for(std::chrono::seconds(5));

    AO1->stop();
    AO2->stop();
    AO3->stop();
    AO4->stop();

    return 0;
}
