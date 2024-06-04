#pragma once
#include <random>
#include <vector>

struct Food 
{
    u32string name = U""; // 화면에 출력되는 음식의 이름
    int canEat; // 0 이외의 숫자는 전부 먹을 수 있음
    u32string message = U""; // 각 음식에 대응하는 메시지
    int minStage = 0; // 0~9, 최초로 음식이 등장하는 스테이지  -- 어려운 음식들의 경우 등장 스테이지를 후반부로 잡으면 됨
};




class FoodManager 
{
public:
    FoodManager() {
    }
    void selectRandom(int stage = 0) 
    {
        int randomNo;
        bool isSeleted = false;
        while (!isSeleted) {
            randomNo = randomNumber(0, foods.size())
            if (!(foods[randomNo].minStage > stage))
        }
    }

private:

    int randomNumber(int min, int max)  // min과 max 사이의 랜덤 int 생성
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist(min, max);
        auto randNum = dist(mt);

        return randNum;
    }

    vector<Food> foods = {
        // 음식들은 여기에 추가
        Food{U"츄르", 1, U"", 0 },
        Food { U"초콜릿", 0, U"" , 0 }
    };
};