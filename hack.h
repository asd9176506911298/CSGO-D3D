#pragma once

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

#define ABS(x) ((x < 0) ? (-x) : (x))

#define TORAD(x) ((x) * 0.01745329252)

#define W2S(x,y) hack->WorldToScreen(x, y)

struct Vec2 {
	float x, y;
};

struct Vec3 {
	float x, y, z;
	Vec3 operator+(Vec3 d)
	{
		return { x + d.x, y + d.y, z + d.z };
	}
	Vec3 operator*(float d)
	{
		return { x * d, y * d,z * d };
	}
};
struct Vec4 {
	float x, y, z, w;
};

class Ent{
public:
	union
	{
		//              Type     Name    Offset
		DEFINE_MEMBER_N(bool, m_bDormant, 0xED);
		DEFINE_MEMBER_N(int, m_iHealth, 0x100);
		DEFINE_MEMBER_N(Vec3, m_vecOrigin, 0x138);
		DEFINE_MEMBER_N(int, m_iTeamNum, 0xF4);
		DEFINE_MEMBER_N(int, m_dwBoneMatrix, 0x26A8);
		DEFINE_MEMBER_N(int, m_ArmorValue, 0x117CC);
		DEFINE_MEMBER_N(Vec3, m_aimPunchAngle, 0x303C);
		DEFINE_MEMBER_N(float, m_angEyeAnglesX, 0x117D0);
		DEFINE_MEMBER_N(float, m_angEyeAnglesY, 0x117D4);
		DEFINE_MEMBER_N(Vec3, m_vecVelocity, 0x114);
		DEFINE_MEMBER_N(bool, m_bHasHelmet, 0x117C0);
	};
};

class EntListObj {
public:
	Ent* ent;
	char padding[12];
};

class EntList {
public:
	EntListObj ents[32];
};

class Hack {
public:
	uintptr_t dwEntityList = 0x4DDB8FC;
	uintptr_t dwViewMatrix = 0x4DCD214;

	uintptr_t engine;
	uintptr_t client;
	Ent* localEnt;
	EntList* entList;
	float viewMatrix[16];

	ID3DXLine* LineL;
	ID3DXFont* FontF;

	Vec2 crosshair2D;
	int corsshairSize = 4;

	~Hack();

	void Init();
	void Update();

	void CheckButtons();

	bool CheckValidEnt(Ent* ent);
	bool WorldToScreen(Vec3 pos, Vec2& screen);
	Vec3 GetBonePos(Ent* ent, int bone);
	Vec3 TransformVec(Vec3 src, Vec3 ang, float d);

	struct Settings {
		bool showMenu = false;
		bool showTeamates = false;
		bool snaplines = false;
		bool box2D = false;
		bool status2D = false;
		bool box3D = false;
		bool velEsp = false;
		bool headlineEsp = false;
		bool statusText = false;
		bool rcsCrosshair = false;
	}settings;

	struct Buttons
	{
		DWORD showMenuBtn =		VK_INSERT;
		DWORD showTeamatesBtn = VK_F1;
		DWORD snaplinesBtn =	VK_F2;
		DWORD box2DBtn =		VK_F3;
		DWORD status2DBtn =		VK_F4;
		DWORD statusTextBtn =	VK_F5;
		DWORD box3DBtn =		VK_F6;
		DWORD velEspBtn =		VK_F7;
		DWORD headlineEspBtn =	VK_F8;
		DWORD rcsCrosshairBtn = VK_F9;
	}button;

	struct Colors
	{
		D3DCOLOR health = D3DCOLOR_ARGB(255, 46, 139, 87);
		D3DCOLOR armor = D3DCOLOR_ARGB(255, 30, 144, 255);
		D3DCOLOR crosshair = D3DCOLOR_ARGB(255, 255, 255, 255);
		struct Team 
		{
			D3DCOLOR esp = D3DCOLOR_ARGB(255, 0, 255, 0);
			D3DCOLOR snapline = D3DCOLOR_ARGB(255, 0, 255, 0);
			D3DCOLOR velocity = D3DCOLOR_ARGB(255, 0, 0, 255);
			D3DCOLOR headline = D3DCOLOR_ARGB(255, 0, 0, 255);
		}team;
		struct Enemy
		{
			D3DCOLOR esp = D3DCOLOR_ARGB(255, 255, 0, 0);
			D3DCOLOR snapline = D3DCOLOR_ARGB(255, 255, 0, 0);
			D3DCOLOR velocity = D3DCOLOR_ARGB(255, 0, 255, 255);
			D3DCOLOR headline = D3DCOLOR_ARGB(255, 0, 255, 255);
		}enemy;
	}color;
};