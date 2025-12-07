#pragma once
#include "Vector2D.h"
#include <vector>

//オブジェクトのタイプ
enum eObjectType
{
	eNone,
	ePlayer,
	eEnemy,
	eTable,
	eSord
};


class Collision
{
public:
	bool is_blocking;
	Vector2D box_size;
	Vector2D pivot;
	eObjectType object_type;
	std::vector<eObjectType> hit_object_type;

	bool IsCheckHitTarget(eObjectType object_type) const;
	bool IsCheckHitCollision(Collision target1, Collision target2);
};