#pragma once
#include "DxLib.h"

template <class Type>
class Singleton
{

protected:

	Singleton() = default;

	Singleton(const Singleton& s) = delete;
	Singleton& operator = (const Singleton& s) = delete;

	~Singleton() = default;

public:
	static Type* GetInstance()
	{
		static Type* instance = new Type();
		return instance;
	}
};