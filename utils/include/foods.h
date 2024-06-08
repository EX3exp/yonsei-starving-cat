﻿#pragma once
#include <random>
#include <vector>
#include <string>
#include <unordered_set> // 중복 검사를 위한 헤더
#include <codecvt>
using namespace std;

class Food
{
public:
    Food() {
        name = U"";
        canEat = 0;
        message = U"";
        minStage = 0;
    }

    Food(u32string name, int canEat, u32string message, int minStage)
        : name(name), canEat(canEat), message(message), minStage(minStage)
    {

    }

    Food(const Food& f) : name(f.name), canEat(f.canEat), message(f.message), minStage(f.minStage) { // 음식 교체 
    }


    void change(const Food& f) { // 음식 교체 
        name = f.name;
        canEat = f.canEat;
        message = f.message;
        minStage = f.minStage;
    }


    bool isCanEat() {
        return (canEat != 0 ? true : false);
    }

    u32string getName() {
        return name;
    }

    string PrintName() {
        return converter.to_bytes(name);
    }

    u32string getMessage() {
        return message;
    }

    int getMinStage() {
        return minStage;
    }

private:
    u32string name; // 화면에 출력되는 음식의 이름
    int canEat; // 0 이외의 숫자는 전부 먹을 수 있음
    u32string message; // 각 음식에 대응하는 메시지
    int minStage; // 0~9, 최초로 음식이 등장하는 스테이지  -- 어려운 음식들의 경우 등장 스테이지를 후반부로 잡으면 됨
    wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
};


class FoodManager
{
public:
    FoodManager() {}

    void selectRandom(int stage, Food& food)
    {
        int randomNo;
        bool isSelected = false;
        while (!isSelected) {
            
            randomNo = randomNumber(0, foods.size() - 1);
            if (!(foods[randomNo].getMinStage() > stage) && usedFoods.find(foods[randomNo].getName()) == usedFoods.end() && ((isFirstTarget && foods[randomNo].isCanEat() != prevFoodCanEat)) || !isFirstTarget) {
                isSelected = true;
            }
                
        }
        usedFoods.insert(foods[randomNo].getName());
        prevFoodCanEat = foods[randomNo].isCanEat();
        isFirstTarget = !isFirstTarget;
        food.change(foods[randomNo]);
    }

    void reset()
    {
        usedFoods.clear();
    }

private:
    int randomNumber(int min, int max)  // min과 max 사이의 랜덤 int 생성
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist(min, max);
        return dist(mt);
    }

    vector<Food> foods = {
        Food(U"츄르", 1, U"츄르는 고양이들이 좋아하는 최고의 간식입니다. \n참치 맛을 포함한 다양한 맛이 있으며, \n제품이 스틱 형태로 낱개 포장되어 있는 것이 특징입니다. \n하지만, 원료 자체의 염분이 있기 때문에 염분 과다 섭취의 위험이 있으므로, \n하루 2스틱 이상을 고양이에게 주지 않도록 해주세요!", 0),
        Food(U"통조림", 1, U"고양이 통조림입니다. \n사람이 먹는 통조림은 고양이가 먹으면 안됩니다!", 0),
        Food(U"사료", 1, U"고양이 사료에는 여러 제품들이 있습니다. \n영양성분 등을 확인하여 양질의 사료를 급여해주세요!", 0),
        Food(U"멸치", 1, U"멸치는 생선 중에서도 고단백, 저지방, 저칼로리 식품이에요. \n육식 동물인 고양이에게 좋은 간식이죠. \n고양이에게 필요한 인, 마그네슘 같은 미네랄과 \n비타민A, B12, D를 섭취 할 수 있어요. \n하지만 과잉 섭취하면 요로결석이 올 수 있으니 주의하세요.", 0),
        Food(U"사과", 1, U"사과는 섬유질을 풍부하게 함유하고 있어 \n고양이의 소화에 도움을 줄 수 있습니다. \n다만, 씨앗은 제거해야 합니다.", 0),
        Food(U"연어", 1, U"연어는 슈퍼푸드로 선정된 식품 중 유일한 생선으로, \n단백질과 오메가-3 지방산의 훌륭한 공급원입니다.", 5),
        Food(U"수박", 1, U"수박은 수분이 풍부하고, 고양이가 좋아하는 식감이라 기호성이 높습니다. \n껍질과 씨앗은 제거하고 과육만 적당량씩 급여해야 합니다.", 0),
        Food(U"당근", 1, U"식이섬유가 많은 당근은 장내 청소에 도움을 줄 수 있습니다. \n변비에도 효과를 기대할 수 있습니다. \n하지만 과다 섭취 시 설사를 할 수 있으니 주의해야 합니다.", 5),
        Food(U"브로콜리", 1, U"브로콜리는 고양이가 먹어도 되는 음식 중 하나입니다! \n허나, 브로콜리는 사람에게도 호불호가 심하게 갈리는 식품입니다. \n더군다나 육식동물인 고양이는 브로콜리를 먹는 고양이보다 \n먹지 않는 고양이가 훨씬 더 많을 거예요.", 5),
        Food(U"육류", 1, U"닭고기, 소고기 등의 고기는 양질의 단백질 공급원으로, \n모두 고양이의 식단을 위한 훌륭한 선택입니다.", 3),
        Food(U"초콜릿", 0, U"초콜릿의 주 원료인 카카오에는 \n쓴맛을 내는 테오브로민이라는 성분이 들어 있는데, \n이 성분은 고양이가 소량을 섭취해도 경련, 구토, 발작 등의 증상을 보일 수 있으며 \n심한 경우에는 목숨을 앗아갈 수 있어요!", 0),
        Food(U"양파", 0, U"양파에는 알릴 프로필 다이설파이드라는 성분이 있습니다. \n이것은 고양이의 적혈구를 파괴하여 용혈성 빈혈을 일으킵니다.\n 절대 먹이지 마세요!", 0),
        Food(U"치즈", 0, U"치즈는 높은 지방 함량을 가지고 있어요. \n고양이는 기름진 음식을 소화하기 어렵기 때문에 \n지나친 지방 섭취는 비만 및 소화 문제를 일으킬 수 있어요.", 0),
        Food(U"요거트", 0, U"고양이는 선천적으로 유당불내증으로, \n사람이 먹는 우유나 치즈, 요거트를 소화하지 못해요. \n고양이 체내엔 유당을 분해하는 락타아제(lactase) 효소가 없기 때문입니다. \n고양이는 육식동물이기 때문에 요거트를 먹을 필요가 없을 뿐더러, \n복통이나 구토, 설사를 유발할 수 있기 때문에 급여해선 안 됩니다.", 0),
        Food(U"포도", 0, U"포도와 건포도에는 고양이의 신부전을 유발하는 독소가 있습니다. \n신장에 손상을 입게 되어 설사와 구토는 물론, \n심한 경우는 급성 신부전증에 걸릴 수 있어 주의해야 합니다. \n포도 껍질이나 건포도 역시 신부전을 일으킬 수 있다고 하니 \n주의해야 합니다.", 5),
        Food(U"호두", 0, U"견과류엔 칼로리가 높아 고양이 비만으로 이어지기 쉽습니다. \n게다가, 지방 함량이 높아 고양이가 잘 소화하지 못하고 \n구토나 설사 등 소화 장애를 유발할 수 있어요. \n또한, 견과류엔 마그네슘 함량이 높은데, 지속적으로 섭취할 경우 당뇨와 신장병을 유발할 수 있습니다.", 3),
        Food(U"땅콩", 0, U"땅콩은 지방 함량이 높아 고양이가 잘 소화하지 못하고 \n구토나 설사 등 소화 장애를 유발할 수 있어요. \n또한, 견과류엔 마그네슘 함량이 높은데, 지속적으로 섭취할 경우 당뇨와 신장병을 유발할 수 있습니다.", 3),
        Food(U"오징어", 0, U"고양이가 생 오징어나 새우, 회 같은 날음식을 많이 먹게 되면 \n비타민 B1 결핍이 올 수도 있어요. 생선에 티아민 성분을 없애버리는 \n티아미나아제(thiaminase) 효소가 들어있기 때문입니다. \n만약 고양이가 비타민B1 결핍에 걸리게 된다면, \n신경학적 문제가 일어나 경련을 일으키거나 호흡 곤란으로 사망할 수도 있습니다.", 5),
        Food(U"문어", 0, U"문어에 포함된 티아미나아제 효소는 비타민 B1을 분해하고 흡수를 방해합니다. \n비타민 B1이 부족할 경우 식욕저하와 구토, 경련, 보행 이상등이 나타날 수 있으니 \n익힌다 하더라도 아예 주지 않는 것이 바람직합니다", 5),
        Food(U"계란", 0, U"달걀은 식중독균의 일종인 살모넬라균을 감염시킬 수 있고 \n기생충도 옮길 수 있으므로 충분히 익혀야 합니다.\n 특히 달걀흰자에 들어 있는 아비딘(avidin)이라는 단백질은 소화흡수를 방해하므로 \n굳이 달걀을 줄 때는 삶은 노른자만 소량 주어야 합니다.", 5),
        Food(U"마늘", 0, U"마늘을 고양이가 먹게 될 경우 적혈구 손상으로 인한 빈혈로 이어집니다.\n 분말로 된 양파와 마늘도 마찬가지니 절대 주지 마세요.", 0)
    };

    bool isFirstTarget = false; // 못 먹는 음식만 두 개 나오는 경우 방지
    bool prevFoodCanEat;
    unordered_set<u32string> usedFoods; // 이미 사용된 음식들을 저장
};