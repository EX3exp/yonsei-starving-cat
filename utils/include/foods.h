#pragma once
#include <random>
#include <vector>

struct Food 
{
    u32string name = U""; // ȭ�鿡 ��µǴ� ������ �̸�
    int canEat; // 0 �̿��� ���ڴ� ���� ���� �� ����
    u32string message = U""; // �� ���Ŀ� �����ϴ� �޽���
    int minStage = 0; // 0~9, ���ʷ� ������ �����ϴ� ��������  -- ����� ���ĵ��� ��� ���� ���������� �Ĺݺη� ������ ��
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

    int randomNumber(int min, int max)  // min�� max ������ ���� int ����
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist(min, max);
        auto randNum = dist(mt);

        return randNum;
    }

    vector<Food> foods = {
        // ���ĵ��� ���⿡ �߰�
        Food{U"��", 1, U"", 0 },
        Food { U"���ݸ�", 0, U"" , 0 }
    };
};