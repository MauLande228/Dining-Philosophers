#include <array>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <iomanip>
#include <string_view>
#include <Windows.h>

#include "Random.h"

std::mutex g_lockprint;
constexpr  int no_of_philosophers = 5;

enum Color
{
    GREEN = 10,
    BLUE,
    RED,
    PURPLE,
    YELLOW,
    WHITE
};

struct fork
{
    std::mutex mutex;
};

struct table
{
    std::array<fork, no_of_philosophers> forks;
    std::atomic<bool> ready{ false };
};

class Philosopher
{
public:
    Philosopher(std::string_view n, table const& t, fork& l, fork& r, Color printColor) :
        name(n), dinnertable(t), left_fork(l), right_fork(r),
        meals(1), color(printColor),
        lifethread(&Philosopher::dine, this)
    {
    }

    ~Philosopher()
    {
        lifethread.join();
    }

    void dine()
    {
        while (!dinnertable.ready);

        while (dinnertable.ready)
        {
            think();
            eat();
            meals++;
        }
    }

    void print(std::string_view text, Color printColor)
    {
        std::lock_guard<std::mutex> cout_lock(g_lockprint);
        
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, printColor);

        std::cout << "Philosopher "
            << std::left << std::setw(10) << std::setfill(' ')
             << name << text << std::endl;
    }

    void print(std::string_view text, uint16_t nMeals, Color printColor)
    {
        std::lock_guard<std::mutex> cout_lock(g_lockprint);

        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, printColor);

        std::cout << "Philosopher "
            << std::left << std::setw(10) << std::setfill(' ')
             << name << text << nMeals << std::endl;
    }

    void eat()
    {
        std::lock(left_fork.mutex, right_fork.mutex);

        std::lock_guard<std::mutex> left_lock(left_fork.mutex, std::adopt_lock);
        std::lock_guard<std::mutex> right_lock(right_fork.mutex, std::adopt_lock);

        print(" started eating meal: ", meals, color);

        auto time = Random::RandInt(1, 2);
        std::this_thread::sleep_for(std::chrono::seconds(time));

        print(" finished eating meal: ", meals, color);
    }

    void think()
    {

        auto time = Random::RandInt(1, 2);
        std::this_thread::sleep_for(std::chrono::seconds(time));

        print(" is thinking ", color);
    }

private:
    std::thread lifethread;
    fork& left_fork;
    fork& right_fork;
    table const& dinnertable;
    std::string const name;
    Color color;

public:
    uint16_t meals;
};

void dine()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Dinner started!" << std::endl;

    {
        table table;
        std::array<Philosopher, no_of_philosophers> philosophers
        {
           {
              { "1", table, table.forks[0], table.forks[1], Color::GREEN },
              { "2", table, table.forks[1], table.forks[2], Color::BLUE },
              { "3", table, table.forks[2], table.forks[3], Color::RED },
              { "4", table, table.forks[3], table.forks[4], Color::PURPLE },
              { "5", table, table.forks[4], table.forks[0], Color::YELLOW },
           }
        };

        table.ready = true;

        while (true)
        {
            if (philosophers[0].meals >= 6 &&
                philosophers[1].meals >= 6 &&
                philosophers[2].meals >= 6 &&
                philosophers[3].meals >= 6 &&
                philosophers[4].meals >= 6)
            {
                table.ready = false;
                break;
            }
        }

        table.ready = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    }

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, Color::WHITE);

    std::cout << "Dinner done!" << std::endl;
}

int main()
{
    Random::Init();

    dine();

    return 0;
}