#include "DrawUI.h"
#include <algorithm>

DrawUI* DrawUI::m_instance = nullptr;

DrawUI::~DrawUI()
{
}

void DrawUI::RegisterDrawRequest(const std::function<void()>& drawFunction, int zOrder)
{
	requests.emplace_back(drawFunction, zOrder);
}

void DrawUI::Draw()
{
    // zOrderでソート
    std::sort(requests.begin(), requests.end(), [](const DrawRequest& a, const DrawRequest& b) {
        return a.zOrder < b.zOrder;
    });

    // 描画を実行
    for (const auto& request : requests) {
        request.drawFunction();
    }

    // リクエストをクリア
    requests.clear();
}