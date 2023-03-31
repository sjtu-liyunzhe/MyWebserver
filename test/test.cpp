#include "../threadpool/threadpoll.h"
#include <iostream>
#include <unistd.h>

int i = 0;
void task()
{
    int cnt = 0;
    {
        std::cout << "from this thread: " << std::this_thread::get_id() << std::endl;
        // std::cout << i++ << std::endl;
        sleep(1);
    }
}

int main()
{
    Threadpoll pool(8);
    for(int i = 0; i < 100; ++i)
    {
        pool.addTask(task);
    }
    return 0;
}