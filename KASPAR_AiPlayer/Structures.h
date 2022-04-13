#pragma once
struct Object
{
	Object() : id(0), x(0.0f), y(0.0f) {}

	unsigned int id;
	float x, y;
};

struct AiDesicion
{
	char x, y, hit, target;
};
