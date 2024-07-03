#pragma once
#include "../render/render.h"
#include <thread>
#include <chrono>
#include "d3dx9math.h"
#include <mutex>
#include <condition_variable>
#include <fstream>
static uint64_t base_id;
#define M_PI       3.14159265358979323846   // pi

class Vector2
{
public:
	Vector2() : x(0.f), y(0.f)
	{

	}

	Vector2(double _x, double _y) : x(_x), y(_y)
	{

	}
	~Vector2()
	{

	}

	Vector2 operator+(Vector2 v)
	{
		return Vector2(x + v.x, y + v.y);
	}

	Vector2 operator-(Vector2 v)
	{
		return Vector2(x - v.x, y - v.y);
	}

	Vector2 operator/(Vector2 v)
	{
		return Vector2(x / v.x, y / v.y);
	}

	Vector2 operator*(Vector2 v)
	{
		return Vector2(x * v.x, y * v.y);
	}


	double x;
	double y;
};
class FVector2d
{
public:
	double x;
	double y;

	inline bool is_Zero() {
		return (x == 0) && (y == 0);
	}
};

class FVector
{
public:
	double x, y, z;
	FVector()
	{
		x = y = z = 0.f;
	}

	FVector(double _x, double _y, double _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	FVector operator+(const FVector& v) {
		return FVector{ x + v.x, y + v.y, z + v.z };
	}

	FVector operator-(const FVector& v) {
		return FVector{ x - v.x, y - v.y, z - v.z };
	}

	FVector operator*(const double v) {
		return FVector{ x * v, y * v, z * v };
	}

	FVector operator/(const double fl) const {
		return FVector(x / fl, y / fl, z / fl);
	}


	inline double length() {
		return sqrt(x * x + y * y + z * z);
	}

	inline double DistanceFrom(const FVector Other) {
		double dx = (this->x - Other.x);
		double dy = (this->y - Other.y);
		double dz = (this->z - Other.z);

		return sqrt((dx * dx) + (dy * dy) + (dz * dz));
	}
	inline double Distance(FVector v)
	{
		return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	__forceinline double Size() const {
		return (double)sqrtf((float)x * (float)x + (float)y * (float)y + (float)z * (float)z);
	}

	double Dot(const FVector& vOther) const {
		const FVector& a = *this;

		return (a.x * vOther.x + a.y * vOther.y + a.z * vOther.z);
	}
};
class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z)
	{

	}

	~Vector3()
	{

	}

	double x;
	double y;
	double z;

	float Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	float Distance(Vector3 v)
	{
		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}
	void addScaled(const Vector3& v, float scale)
	{
		x += v.x * scale;
		y += v.y * scale;
		z += v.z * scale;
	}
	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(float number) const {
		return Vector3(x * number, y * number, z * number);
	}

	// Conversion operator to convert Vector3 to Vector2
	operator Vector2() const {
		return Vector2(x, y);
	}
};

struct Fquat
{
	double x;
	double y;
	double z;
	double w;
};
struct FRotator
{
	double Pitch;
	double Yaw;
	double Roll;
};

struct FTransform
{
	Fquat rot;
	Vector3 translation;
	char pad[4];
	Vector3 scale;
	char pad1[4];

	void clear() {
		rot = { 0.0f, 0.0f, 0.0f, 1.0f };
		translation = { 0.0f, 0.0f, 0.0f };
		scale = { 1.0f, 1.0f, 1.0f };
	}

	D3DMATRIX ToMatrixWithScale()
	{
		D3DMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};
static D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}

static D3DMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}


class Frotator
{
public:
	Frotator() : Pitch(0.f), Yaw(0.f), Roll(0.f)
	{

	}

	Frotator(double _Pitch, double _Yaw, double _Roll) : Pitch(_Pitch), Yaw(_Yaw), Roll(_Roll)
	{

	}
	~Frotator()
	{

	}

	double Pitch;
	double Yaw;
	double Roll;

	inline Frotator get()
	{
		return Frotator(Pitch, Yaw, Roll);
	}

	inline void set(double _Pitch, double _Yaw, double _Roll)
	{
		Pitch = _Pitch;
		Yaw = _Yaw;
		Roll = _Roll;
	}

	inline Frotator Clamp()
	{
		Frotator result = get();
		if (result.Pitch > 180)
			result.Pitch -= 360;
		else if (result.Pitch < -180)
			result.Pitch += 360;
		if (result.Yaw > 180)
			result.Yaw -= 360;
		else if (result.Yaw < -180)
			result.Yaw += 360;
		if (result.Pitch < -89)
			result.Pitch = -89;
		if (result.Pitch > 89)
			result.Pitch = 89;
		while (result.Yaw < -180.0f)
			result.Yaw += 360.0f;
		while (result.Yaw > 180.0f)
			result.Yaw -= 360.0f;

		result.Roll = 0;
		return result;

	}

	inline double Length()
	{
		return sqrt(Pitch * Pitch + Yaw * Yaw + Roll * Roll);
	}

	Frotator operator+(Frotator angB) { return Frotator(Pitch + angB.Pitch, Yaw + angB.Yaw, Roll + angB.Roll); }
	Frotator operator-(Frotator angB) { return Frotator(Pitch - angB.Pitch, Yaw - angB.Yaw, Roll - angB.Roll); }
	Frotator operator/(float flNum) { return Frotator(Pitch / flNum, Yaw / flNum, Roll / flNum); }
	Frotator operator*(float flNum) { return Frotator(Pitch * flNum, Yaw * flNum, Roll * flNum); }
	bool operator==(Frotator angB) { return Pitch == angB.Pitch && Yaw == angB.Yaw && Yaw == angB.Yaw; }
	bool operator!=(Frotator angB) { return Pitch != angB.Pitch || Yaw != angB.Yaw || Yaw != angB.Yaw; }

};

inline double __fastcall Atan2(double a1, double a2)
{
	double result;

	result = 0.0;
	if (a2 != 0.0 || a1 != 0.0)
		return atan2(a1, a2);
	return result;
}

inline double __fastcall FMod(double a1, double a2)
{
	if (fabs(a2) > 0.00000001)
		return fmod(a1, a2);
	else
		return 0.0;
}

inline double ClampAxis(double Angle)
{
	Angle = FMod(Angle, (double)360.0);

	if (Angle < (double)0.0)
	{
		Angle += (double)360.0;
	}

	return Angle;
}

inline double NormalizeAxis(double Angle)
{
	Angle = ClampAxis(Angle);

	if (Angle > (double)180.0)
	{
		Angle -= (double)360.0;
	}

	return Angle;
}

inline Frotator Rotator(Fquat* F)
{
	const double SingularityTest = F->z * F->x - F->w * F->y;
	const double YawY = 2.f * (F->w * F->z + F->x * F->y);
	const double YawX = (1.f - 2.f * ((F->y * F->y) + (F->z * F->z)));

	const double SINGULARITY_THRESHOLD = 0.4999995f;
	const double RAD_TO_DEG = 57.295776;
	double Pitch, Yaw, Roll;

	if (SingularityTest < -SINGULARITY_THRESHOLD)
	{
		Pitch = -90.f;
		Yaw = (Atan2(YawY, YawX) * RAD_TO_DEG);
		Roll = NormalizeAxis(-Yaw - (2.f * Atan2(F->x, F->w) * RAD_TO_DEG));
	}
	else if (SingularityTest > SINGULARITY_THRESHOLD)
	{
		Pitch = 90.f;
		Yaw = (Atan2(YawY, YawX) * RAD_TO_DEG);
		Roll = NormalizeAxis(Yaw - (2.f * Atan2(F->x, F->w) * RAD_TO_DEG));
	}
	else
	{
		Pitch = (asin(2.f * SingularityTest) * RAD_TO_DEG);
		Yaw = (Atan2(YawY, YawX) * RAD_TO_DEG);
		Roll = (Atan2(-2.f * (F->w * F->x + F->y * F->z), (1.f - 2.f * ((F->x * F->x) + (F->y * F->y)))) * RAD_TO_DEG);
	}

	Frotator RotatorFromQuat = Frotator{ Pitch, Yaw, Roll };
	return RotatorFromQuat;
}

typedef          __int64 ll;
typedef unsigned __int64 ull;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

typedef          char   int8;
typedef   signed char   sint8;
typedef unsigned char   uint8;
typedef          short  int16;
typedef   signed short  sint16;
typedef unsigned short  uint16;
typedef          int    int32;
typedef   signed int    sint32;
typedef unsigned int    uint32;
typedef ll              int64;
typedef ll              sint64;
typedef ull             uint64;


#define _BYTE  uint8
#define _WORD  uint16
#define _DWORD uint32
#define _QWORD uint64


struct
{
	bool render_menu = true;

	struct
	{
		uintptr_t
			player_state,
			view_state,
			gworld,
			location_pointer,
			rotation_pointer,
			game_state,
			game_instance,
			root_component,
			local_pawn,
			player_controller,
			local_player,
			player_array;
		Vector3
			relative_location;
		int
			player_count,
			teamindex;

		std::string local_weapon;
	}pointers;


}globals;


namespace exploit
{
	inline bool fov_changer = false;
	inline bool Magic = false;
	inline float fov_value = 110.0f;
}
namespace a1m
{
	inline bool team_check = false;
	inline bool aimbot = true;
	inline bool prediction = false;
	inline float fov = 180.0f;
	inline float smoothing = 3.0f;
	inline bool visible_check = false;
	inline int aim_key = VK_F1;
	inline float max_distance = 270.f;
	inline int hitbox = 0;
}

namespace esp
{
	inline bool snap_line = false;
	inline bool team_check = false;
	inline bool current_weapon = false;
	inline bool platform = false;
	inline bool target_line = false;
	inline bool box = false;
	inline bool box_filled = false;
	inline bool render_fov = false;
	inline bool distance = false;
	inline bool skeleton = false;
	inline bool name = false;
}
namespace misc
{
	inline bool lobby_esp = true;
}
namespace triggerbot 
{
	inline int triggerbot_key = VK_RBUTTON;
	inline bool enabled = false;;
	inline bool shotgun_only = false;
	inline bool always_on = false;
	inline int maximum_distance = 50;
	inline int delay = 100;
}
namespace aimkey
{
	inline static int keystatus = 0;

	inline void ChangeKey(void* blank)
	{
		keystatus = 1;
		while (true)
		{
			for (int i = 0; i < 0x87; i++)
			{
				if (GetAsyncKeyState(i) & 0x8000)
				{
					a1m::aim_key = i;
					keystatus = 0;
					return;
				}
			}
		}
	}

	static const char* keyNames[] =
	{
		"Keybind",
		"Left Mouse",
		"Right Mouse",
		"Cancel",
		"Middle Mouse",
		"Mouse 5",
		"Mouse 4",
		"",
		"Backspace",
		"Tab",
		"",
		"",
		"Clear",
		"Enter",
		"",
		"",
		"Shift",
		"Control",
		"Alt",
		"Pause",
		"Caps",
		"",
		"",
		"",
		"",
		"",
		"",
		"Escape",
		"",
		"",
		"",
		"",
		"Space",
		"Page Up",
		"Page Down",
		"End",
		"Home",
		"Left",
		"Up",
		"Right",
		"Down",
		"",
		"",
		"",
		"Print",
		"Insert",
		"Delete",
		"",
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"A",
		"B",
		"C",
		"D",
		"E",
		"F",
		"G",
		"H",
		"I",
		"J",
		"K",
		"L",
		"M",
		"N",
		"O",
		"P",
		"Q",
		"R",
		"S",
		"T",
		"U",
		"V",
		"W",
		"X",
		"Y",
		"Z",
		"",
		"",
		"",
		"",
		"",
		"Numpad 0",
		"Numpad 1",
		"Numpad 2",
		"Numpad 3",
		"Numpad 4",
		"Numpad 5",
		"Numpad 6",
		"Numpad 7",
		"Numpad 8",
		"Numpad 9",
		"Multiply",
		"Add",
		"",
		"Subtract",
		"Decimal",
		"Divide",
		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"F10",
		"F11",
		"F12",
	};

	inline static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
	{
		const char* const* items = (const char* const*)data;
		if (out_text)
			*out_text = items[idx];
		return true;
	}
	inline void HotkeyButton()
	{
		const char* preview_value = NULL;
		if (a1m::aim_key >= 0 && a1m::aim_key < IM_ARRAYSIZE(keyNames))
			Items_ArrayGetter(keyNames, a1m::aim_key, &preview_value);
		std::string aimkeys;
		if (preview_value == NULL)
			aimkeys = (("Select Key"));
		else
			aimkeys = preview_value;

		if (keystatus == 1)
		{
			aimkeys = (("Press the Key"));
		}
		if (ImGui::Button(aimkeys.c_str(), ImVec2(155, 40)))
		{
			if (keystatus == 0)
			{
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ChangeKey, nullptr, 0, nullptr);
			}
		}
	}
   
}

namespace triggerkey
{
	inline static int dkeystatus = 0;

	inline void ChangeKeys()
	{
		dkeystatus = 1;
		while (true)
		{
			for (int i = 0; i < 0x87; i++)
			{
				if (GetAsyncKeyState(i) & 0x8000)
				{
					triggerbot::triggerbot_key = i;
					dkeystatus = 0;
					return;
				}
			}
		}
	}

	static const char* keyNadmes[] =
	{
		"Keybind",
		"Left Mouse",
		"Right Mouse",
		"Cancel",
		"Middle Mouse",
		"Mouse 5",
		"Mouse 4",
		"",
		"Backspace",
		"Tab",
		"",
		"",
		"Clear",
		"Enter",
		"",
		"",
		"Shift",
		"Control",
		"Alt",
		"Pause",
		"Caps",
		"",
		"",
		"",
		"",
		"",
		"",
		"Escape",
		"",
		"",
		"",
		"",
		"Space",
		"Page Up",
		"Page Down",
		"End",
		"Home",
		"Left",
		"Up",
		"Right",
		"Down",
		"",
		"",
		"",
		"Print",
		"Insert",
		"Delete",
		"",
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"A",
		"B",
		"C",
		"D",
		"E",
		"F",
		"G",
		"H",
		"I",
		"J",
		"K",
		"L",
		"M",
		"N",
		"O",
		"P",
		"Q",
		"R",
		"S",
		"T",
		"U",
		"V",
		"W",
		"X",
		"Y",
		"Z",
		"",
		"",
		"",
		"",
		"",
		"Numpad 0",
		"Numpad 1",
		"Numpad 2",
		"Numpad 3",
		"Numpad 4",
		"Numpad 5",
		"Numpad 6",
		"Numpad 7",
		"Numpad 8",
		"Numpad 9",
		"Multiply",
		"Add",
		"",
		"Subtract",
		"Decimal",
		"Divide",
		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"F10",
		"F11",
		"F12",
	};

	inline static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
	{
		const char* const* items = (const char* const*)data;
		if (out_text)
			*out_text = items[idx];
		return true;
	}
	inline void HotkeyButton()
	{
		const char* preview_value = NULL;
		if (triggerbot::triggerbot_key >= 0 && triggerbot::triggerbot_key < IM_ARRAYSIZE(keyNadmes))
			Items_ArrayGetter(keyNadmes, triggerbot::triggerbot_key, &preview_value);
		std::string triggerkeys;
		if (preview_value == NULL)
			triggerkeys = (("Select Key"));
		else
			triggerkeys = preview_value;

		if (dkeystatus == 1)
		{
			triggerkeys = (("Press the Key"));
		}
		if (ImGui::Button(triggerkeys.c_str(), ImVec2(155, 40)))
		{
			if (dkeystatus == 0)
			{
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ChangeKeys, nullptr, 0, nullptr);
			}
		}
	}

}
class entity
{
public:
	struct
	{
		uintptr_t bone_array;

		FTransform component_to_world;

	}bone_manager;

	bool name_read = false;
	bool platform_read = false;

	uintptr_t entity;
	uintptr_t mesh;
	std::string name;
	std::string platform;

	uintptr_t root_component;
	uintptr_t player_state;
	int team_index;

	void reset()
	{
		bone_manager.bone_array = 0;
		bone_manager.component_to_world.clear();
		entity = 0;
		mesh = 0;
		name = "";
		name_read = false;
		root_component = 0;
		player_state = 0;
		team_index = 0;
	}
};



static std::vector<entity> entity_list;
static std::vector<entity> temporary_entity_list;

class Weapon
{
public:

	int32_t ammo_count;
	int32_t max_ammo;

	int held_weapon_type;

	BYTE tier;
	std::string weapon_name;
	std::string buildplan;

};
static std::string LocalPlayerWeapon;
static std::vector<Weapon> Temp_Weapon;
static std::vector<Weapon> Weapon_Act;


struct
{
	uintptr_t UWorld = 0x1216B7B8;
	uintptr_t LocalPawn = 0x338;
	uintptr_t PlayerController = 0x30;
	uintptr_t LocalPlayers = 0x38;
	uintptr_t PlayerState = 0x2b0;
	uintptr_t GameState = 0x160;
	uintptr_t GameInstance = 0x1D8;
	uintptr_t PlayerArray = 0x2A8;
	uintptr_t RootComponent = 0x198;
	uintptr_t CachedActor = 0x308;
	uintptr_t ActorCount = 0xA8;
	uintptr_t TeamIndex = 0x1211;
	uintptr_t BoneArray = 0x5B0;
	uintptr_t BoneArrayCached = 0x5E0;
	uintptr_t Actor = 0xA0;
	uintptr_t ActorMesh = 0x318;
	uintptr_t PersistentLevel = 0x30;
	uintptr_t RelativeLocation = 0x128;
	uintptr_t DisplayName = 0x90;
	uintptr_t ComponetToWorld = 0x1c0;
	uintptr_t PawnPrivate = 0x308;

	uintptr_t LastSubmitTime = 0x2E8;
	uintptr_t LastRenderTimeOnScreen = 0x2F0;
	uintptr_t Velocity = 0x168;
	uintptr_t DefaultFOV = 0x170;

	uintptr_t CurrentWeapon = 0xa68;
	uintptr_t bIsReloadingWeapon = 0x368;
	uintptr_t bIsEquippingWeapon = 0x350;
	uintptr_t bIsChargingWeapon = 0x369;
	uintptr_t WeaponData = 0x4d0;
	uintptr_t AmmoCount = 0xe1c;
	uintptr_t bIsTargeting = 0x581;
	uintptr_t GunName = 0x98;
	uintptr_t Tier = 0x11b;
	uintptr_t location_under_reticle = 0x26b0;
	uintptr_t targeted_fort_pawn = 0x1888;

}Offsets;

inline Vector3 Location;
inline Vector3 Rotation;
inline float   FOV;

enum Bones : int
{
	HumanBase = 0,
	HumanPelvis = 2,
	HumanLThigh1 = 71,
	HumanLThigh2 = 77,
	HumanLThigh3 = 72,
	HumanLCalf = 74,
	HumanLFoot2 = 73,
	HumanLFoot = 86,
	HumanLToe = 76,
	HumanRThigh1 = 78,
	HumanRThigh2 = 84,
	HumanRThigh3 = 79,
	HumanRCalf = 81,
	HumanRFoot2 = 82,
	HumanRFoot = 87,
	HumanRToe = 83,
	HumanSpine1 = 7,
	HumanSpine2 = 5,
	HumanSpine3 = 2,
	HumanLCollarbone = 9,
	HumanLUpperarm = 35,
	HumanLForearm1 = 36,
	HumanLForearm23 = 10,
	HumanLForearm2 = 34,
	HumanLForearm3 = 33,
	HumanLPalm = 32,
	HumanLHand = 11,
	HumanRCollarbone = 98,
	HumanRUpperarm = 64,
	HumanRForearm1 = 65,
	HumanRForearm23 = 39,
	HumanRForearm2 = 63,
	HumanRForearm3 = 62,
	HumanRHand = 40,
	HumanRPalm = 58,
	HumanNeck = 67,
	HumanHead = 110,
	HumanLowerHead = 106,
	HumanChest = 66
};

struct FPlane : Vector3
{
	double W = 0;

	Vector3 ToVector3()
	{
		Vector3 value;
		value.x = this->x;
		value.y = this->y;
		value.z = this->y;

		return value;
	}
};
struct alignas(16) MatrixElements {
	double m11, m12, m13, m14;
	double m21, m22, m23, m24;
	double m31, m32, m33, m34;
	double m41, m42, m43, m44;

	MatrixElements() : m11(0), m12(0), m13(0), m14(0),
		m21(0), m22(0), m23(0), m24(0),
		m31(0), m32(0), m33(0), m34(0),
		m41(0), m42(0), m43(0), m44(0) {}
};

struct alignas(16) dbl_matrix {
	union {
		MatrixElements elements;
		double m[4][4];
	};

	dbl_matrix() : elements() {}

	double& operator()(size_t row, size_t col) { return m[row][col]; }
	const double& operator()(size_t row, size_t col) const { return m[row][col]; }
};

struct alignas(16) FMatrix : public dbl_matrix {
	FPlane XPlane;
	FPlane YPlane;
	FPlane ZPlane;
	FPlane WPlane;

	FMatrix() : dbl_matrix(), XPlane(), YPlane(), ZPlane(), WPlane() {}
};
inline Vector3 predict_location(const Vector3& target_initial, const Vector3& velocity, float distance, float projectile_speed, float projectile_gravity)
{
	float pred_wconfig_t;

	if (distance >= 150)
	{
		pred_wconfig_t = 1.03;
	}
	else if (distance <= 150)
	{
		pred_wconfig_t = 1.08;
	}
	else
	{
		pred_wconfig_t = 1.00;
	}

	auto target = target_initial;
	auto projectile_speeds = projectile_speed / pred_wconfig_t;
	auto time = distance / projectile_speeds;

	target.addScaled(velocity, time);

	auto gravity_effect = std::fabs(-980.0f * projectile_gravity) * 0.5f * time * time;
	target.z += gravity_effect;

	return target;
}
//inline Vector3 predict_location(Vector3 target, Vector3 velocity, float projectile_speed, float gravity_scale, float distance)
//{
//    float time = distance / projectile_speed;
//
//    target.addScaled(velocity, time);
//
//    float gravity = std::fabs(-980.0f * gravity_scale) * 0.5f * time * time;
//    target.z += gravity;
//
//    return target;
//}