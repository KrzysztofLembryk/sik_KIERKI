#include <iostream>
#include <semaphore>
#include <map>

class A
{
public:
    A(): sem1(1), sem2(0) {} 
    ~A() = default;
    void foo()
    {
        std::cout << "foo" << std::endl;
    }
    void acquire()
    {
        sem1.acquire();
        sem1.acquire();
    }
    private:
    std::binary_semaphore sem1;
    std::binary_semaphore sem2;
};


int main()
{
    A a;
    a.acquire();
    return 0;
}


