#include "Vector2D.h"
#include <math.h>

const Vector2D Vector2D::operator/(const float& scalar) const
{
	// 0除算の考慮
	if (fabsf(scalar) < 1.0e-6f)
	{
		return Vector2D(0.0f);
	}

	return Vector2D(this->x / scalar, this->y / scalar);
}

const Vector2D Vector2D::operator/(const Vector2D& v) const
{
	// 0除算の考慮
	if (fabsf(v.x) < 1.0e-6f || fabsf(v.y) < 1.0e-6f)
	{
		return Vector2D(0.0f);
	}

	return Vector2D(this->x / v.x, this->y / v.y);
}

Vector2D& Vector2D::operator/=(const float& scalar)
{
	// 0除算の考慮
	if (fabsf(scalar) < 1.0e-6f)
	{
		this->x = 0.0f;
		this->y = 0.0f;
	}
	else
	{
		this->x /= scalar;
		this->y /= scalar;
	}

	return *this;
}

Vector2D& Vector2D::operator/=(const Vector2D& v)
{
	// 0除算の考慮
	if (fabsf(v.x) < 1.0e-6f || fabsf(v.y) < 1.0e-6f)
	{
		this->x = 0.0f;
		this->y = 0.0f;
	}
	else
	{
		this->x /= v.x;
		this->y /= v.y;
	}

	return *this;
}

//float Vector2D::SqrLength() const
//{
//	return Vector2D::Dot(*this);
//}
//
//float Vector2D::Length() const
//{
//	return sqrtf(SqrLength());
//}
//
//Vector2D Vector2D::Normalize() const
//{
//	return *this / Length();
//}
//
//float Vector2D::Dot(const Vector2D& a)
//{
//	return ((a.x * a.x) + (a.y * a.y));
//}
//
//float Vector2D::Dot(const Vector2D& a, const Vector2D& b)
//{
//	return ((a.x * b.x) + (a.y * b.y));
//}
//
//float Vector2D::Cross(const Vector2D& a, const Vector2D& b)
//{
//	return (a.x * b.y - a.y * b.x);
//}
//
//Vector2D Vector2D::Lerp(const Vector2D& a, const Vector2D& b, const float t)
//{
//	return a + ((b - a) * t);
//}
//
//float Vector2D::Distance(const Vector2D& a, const Vector2D& b)
//{
//	return Vector2D(a - b).SqrLength();
//}

float Vector2D::SqrLength() const
{
	return Vector2D::Dot(*this);
}

float Vector2D::Length() const
{
	return sqrtf(SqrLength());
}

Vector2D Vector2D::Normalize() const
{
	return *this / Length();
}

float Vector2D::Dot(const Vector2D& a)
{
	return ((a.x * a.x) + (a.y * a.y));
}

float Vector2D::Dot(const Vector2D& a, const Vector2D& b)
{
	return ((a.x * b.x) + (a.y * b.y));
}

float Vector2D::Cross(const Vector2D& a, const Vector2D& b)
{
	return (a.x * b.y - a.y * b.x);
}

Vector2D Vector2D::Lerp(const Vector2D& a, const Vector2D& b, const float t)
{
	return a + ((b - a) * t);
}

float Vector2D::Distance(const Vector2D& a, const Vector2D& b)
{
	return Vector2D(a - b).SqrLength();
}

// 摩擦の影響を考慮して速度を更新する関数
//Vector2D Vector2D::applyFriction(Vector2D velocity, float normalForce, float mu_static, float mu_kinetic, float deltaTime) {
//	float speed = velocity.Length(); // 速度の大きさ
//
//	if (speed == 0)
//	{
//		return velocity;   // 速度が0なら摩擦は作用しない
//	}
//
//	Vector2D frictionForce;
//	Vector2D direction = velocity.Normalize(); // 速度の逆向きに摩擦が働く
//
//	// 静止摩擦を適用
//	float staticFriction = mu_static * normalForce;
//
//	if (speed < staticFriction * deltaTime) {
//		// 摩擦が十分強くて物体が止まる場合
//		return velocity -= 0.1f;
//		//{ 0.0f, 0.0f };
//	}
//
//	// 動摩擦を適用
//	float kineticFriction = mu_kinetic * normalForce;
//	frictionForce = direction * (-kineticFriction); // 摩擦力は速度の逆向き
//
//	// 摩擦の影響を速度に反映
//	return velocity + (frictionForce * deltaTime);
//}
