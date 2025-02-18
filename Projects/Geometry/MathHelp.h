#pragma once
#include "Vec3.h"
#include "TrapManager.h"

/// <summary>
/// 線分と点の直線上の最近接点を取得
/// </summary>
/// <param name="start">線分の始点</param>
/// <param name="end">線分の終点</param>
/// <param name="point">点の座標</param>
/// <returns>線分上の最近接点</returns>
Vec3 GetNearestPtOnLine(const Vec3& start, const Vec3& end, const Vec3& point);

/// <summary>
/// 線分と線分のそれぞれの線分上の最近接点を取得
/// </summary>
/// <param name="centerA">線分Aの中心座標</param>
/// <param name="sizeA">線分Aの長さの半分</param>
/// <param name="centerB">線分Bの中心座標</param>
/// <param name="sizeB">線分Bの長さの半分</param>
/// <param name="resultAPos">線分A上の線分Bとの最近接点座標</param>
/// <param name="resultBPos">線分B上の線分Aとの最近接点座標</param>
void GetNearestPtOnLine(const Vec3& centerA, const Vec3& sizeA, const Vec3& centerB, const Vec3& sizeB,Vec3& resultAPos,Vec3& resultBPos);

Vec3 ClosestPointOnLineSegment(const Vec3& lineStart, const Vec3& lineEnd, const Vec3& point);

std::list<std::shared_ptr<TrapManager::Trap>> CheckHitBoundingBoxAndPoints(const Vec3& start, const Vec3& end, const std::list<std::shared_ptr<TrapManager::Trap>> trap);