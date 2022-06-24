/*
1. Создайте потокобезопасную оболочку для объекта cout. Назовите ее pcout. Необходимо,
чтобы несколько потоков могли обращаться к pcout и информация выводилась в консоль.
Продемонстрируйте работу pcout.
2. Реализовать функцию, возвращающую i-ое простое число (например, миллионное простое
число равно 15485863). Вычисления реализовать во вторичном потоке. В консоли отображать
прогресс вычисления (в основном потоке).
3. Промоделировать следующую ситуацию. Есть два человека (2 потока): хозяин и вор. Хозяин
приносит домой вещи (функция добавляющая случайное число в вектор с периодичностью 1
раз в секунду). При этом у каждой вещи есть своя ценность. Вор забирает вещи (функция,
которая находит наибольшее число и удаляет из вектора с периодичностью 1 раз в 0.5
секунд), каждый раз забирает вещь с наибольшей ценностью.
*/

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <random>

#include "Timer.h"


using namespace std;

//Task #1
static mutex mtx_cout;

class pcout
{

    private:
        
        lock_guard<mutex> lg;
    public:
        pcout() : lg(lock_guard<mutex>(mtx_cout))
        {
        }
    template<typename T>
    pcout& operator<<(const T& data)
    {
        cout << data;
        return *this;
    }

    pcout& operator<<(ostream& (*fp)(ostream&))               
    {                                                                    
        cout << fp;                                                 
        return *this;                                                    
    }                                                                    
};

void DoThings(int number)
{
    pcout() << "Start thread: " << number << endl;
    pcout() << "Stop thread: " << number << endl;
}

void task1()
{
    thread th1(DoThings, 1);
    thread th2(DoThings, 2);
    thread th3(DoThings, 3);
    th1.join();
    th2.join();
    th3.join();
}
//Task #2
bool isSimple(size_t number)
{
    size_t limit = number / 2;

    if (number > 2)
    {
        for (size_t i = 2; i <= limit; ++i)
        {
            if (number% i == 0)
            {
                return false;
            }
        }
    }

    return true;
}

void task2()
{

    size_t maxValue = 1000000;
    size_t countThreads = 30;
    vector<size_t> answer;
    mutex m;

    Timer timer("With threads");

    for (size_t i = 0; i < maxValue;)
    {
        vector<thread> task2;

        for (size_t j = i + countThreads; i <= j; ++i)
        {
            task2.push_back(thread([=, &m, &answer]()
                {
                    if (isSimple(i))
                    {
                        lock_guard lg(m); //
                        answer.push_back(i);
                    }
                }));
        }

        for (auto& t : task2)
        {
            t.join();
        }
    }
    timer.print();

    answer.clear();
    answer.shrink_to_fit();

    timer.start("Without threads");

    for (size_t i = 0; i < maxValue; ++i)
    {
        if (isSimple(i))
        {
            answer.push_back(i);
        }
    }
    timer.print();
}
std::mutex m;
//Task #3
void generate_things(vector<int>& task3)
{
    random_device rd;
    mt19937 mersenne(rd());
    uniform_int_distribution<int> urd(0, 1000);

    lock_guard lg(m);

    generate(task3.begin(), task3.end(), [&]()
        {
            return urd(mersenne);
        });
}

void pop_thing(vector<int>& task3)
{
    lock_guard lg(m);

    cout << *max_element(task3.begin(), task3.end()) << endl;
}

void task3()
{
    vector<int> task3(100);
    size_t count = 100;

    while (count--)
    {
        thread owner([&]()
            {
                generate_things(task3);
            });
        thread thief([&]()
            {
                pop_thing(task3);
            });

        owner.join();
        thief.join();
    }
}
int main()
{
    cout << "Task #1" << endl;
    task1();
    cout << "Task #2" << endl;
    task2();
    cout << "Task #3" << endl;
    task3();

    return 0;
}
