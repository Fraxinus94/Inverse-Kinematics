#include <raylib.h>
#include <stdio.h>
#include <math.h>
#include <vector>

struct Bone
{
public:
	Bone(float ax, float ay, float nLength, float nAngle = 0)
	{
		a = { ax, ay };
		length = nLength;
		angle = nAngle;
		b = { a.x + length * (float)cos(angle), a.y + length * (float)sin(angle) };
	}

	Bone(Bone* nParent, float nLength)
	{
		parent = nParent;
		a = parent->b;
		b = { 0.0f, 0.0f };
		length = nLength;
		b = { a.x + length * (float)cos(angle), a.y + length * (float)sin(angle) };
	}

public:
	void updateChild(float nLength)
	{
		b = { child->a };

		updateParent(b, nLength);
	}

	void updateParent(Vector2 mousepos, float nLength)
	{
		length = nLength;

		Vector2 dir = { mousepos.x - a.x, mousepos.y - a.y };

		angle = (float)atan2(dir.y, dir.x);

		b = { mousepos };

		a = { b.x - length * (float)cos(angle), b.y - length * (float)sin(angle) };

	}

	void updateChild(Bone* nChild) { child = nChild; }

	Vector2 getPointA() { return a; }
	Vector2 getPointB() { return b; }

private:
	Bone* parent = nullptr;
	Bone* child = nullptr;
	Vector2 a;
	Vector2 b;
	float length;
	float angle;
};

std::vector<Bone*> generateBones(Vector2 pos, float boneLength, int boneCount, std::vector<Bone*> bones)
{
	bones.clear();

	Bone* parent = new Bone(pos.x * 0.5f, pos.y * 0.5f, boneLength);
	for (int i = 0; i < boneCount; i++)
	{
		Bone* child = new Bone(parent, boneLength);
		parent->updateChild(child);
		parent = child;
		bones.push_back(parent);
	}

	return bones;
}

int main()
{
	const int WIDTH = 720;
	const int HEIGHT = 480;

	InitWindow(WIDTH, HEIGHT, "IK");

	SetTargetFPS(120);

	Vector2 ballPosition = { WIDTH * 0.5f, HEIGHT * 0.5f };
	Vector2 ballSpeed = { 1.0f, 1.0f };
	int ballRadius = 20;

	int boneCount = 1;
	float boneLength = 20.0f;

	Bone* bone = nullptr;
	std::vector<Bone*> bones;
	bones = generateBones(Vector2{ WIDTH * 0.5, HEIGHT * 0.5f }, boneLength, boneCount, bones);
		
	while (!WindowShouldClose())
	{
		if (IsKeyPressed(KEY_UP))
		{
			Bone* parent = bones[bones.size() - 1];
			Bone* child = new Bone(bones[bones.size() - 1], boneLength);
			parent->updateChild(child);
			parent = child;
			bones.push_back(parent);
			boneCount++;

		}

		if (IsKeyPressed(KEY_DOWN))
		{
			if (boneCount > 1)
			{
				bones.pop_back();
				boneCount--;
			}
		}

		if (IsKeyPressed(KEY_LEFT))
			boneLength--;

		if (IsKeyPressed(KEY_RIGHT))
			boneLength++;

		ballPosition.x += ballSpeed.x;
		ballPosition.y += ballSpeed.y;

		if ((ballPosition.x >= (GetScreenWidth() - ballRadius)) || (ballPosition.x <= ballRadius)) ballSpeed.x *= -1.0f;
		if ((ballPosition.y >= (GetScreenHeight() - ballRadius)) || (ballPosition.y <= ballRadius)) ballSpeed.y *= -1.0f;

		bone = bones[bones.size() - 1];
		bone->updateParent(/*GetMousePosition()*/ballPosition , boneLength);

		for (int i = 0; i < bones.size() - 1; i++)
			bones[i]->updateChild(boneLength);

		BeginDrawing();

		ClearBackground(BLACK);

		DrawLineEx(bone->getPointA(), bone->getPointB(), 1.0f, WHITE);

		for (int i = 0; i < bones.size() - 1; i++)
			DrawLineEx(bones[i]->getPointA(), bones[i]->getPointB(), 1.0f, WHITE);

		DrawCircleV(ballPosition, (float)ballRadius, MAROON);

		EndDrawing();
		
	}

	CloseWindow();

	return 0;
}
