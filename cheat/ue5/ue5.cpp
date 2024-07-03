#include "ue5.h"

#include "../driver/driver.h"

#include "../../../backend/encryption/SkCrypt.h"

#include "../aim/aim.h"

float ue5::distance_from_crosshair(Vector2 point)
{
	double d_x = point.x - (ue5::width / 2);
	double d_y = point.y - (ue5::height / 2);
	float distance = sqrtf(d_x * d_x + d_y * d_y);

	return distance;
}

bool ue5::visible(uintptr_t mesh)
{
	float LastSubmitTime = luminox->read<float>(mesh + Offsets.LastSubmitTime);
	float LastRenderTimeOnScreen = luminox->read<float>(mesh + Offsets.LastRenderTimeOnScreen);
	const float VisionTick = 0.06f;

	return LastRenderTimeOnScreen + VisionTick >= LastSubmitTime;
}

bool ue5::get_bone_array(entity* pawn)
{
	pawn->bone_manager.bone_array = luminox->read<uintptr_t>(pawn->mesh + 0x5B0 + (luminox->read<int>(pawn->mesh + 0x5F8) * 0x10));

	pawn->bone_manager.component_to_world = luminox->read<FTransform>(pawn->mesh + Offsets.ComponetToWorld);

	return true;
}

Vector3 ue5::get_bone(entity* pawn, int bone_id)
{
	FTransform bone = luminox->read<FTransform>(pawn->bone_manager.bone_array + bone_id * 0x60);

	D3DMATRIX matrix = MatrixMultiplication(bone.ToMatrixWithScale(), pawn->bone_manager.component_to_world.ToMatrixWithScale());

	return Vector3(matrix._41, matrix._42, matrix._43);
}

std::string ue5::get_platform(uintptr_t player_state)
{
	uintptr_t test_platform = luminox->read<uintptr_t>(player_state + 0x438);
	wchar_t platform[64] = { 0 };
	luminox->read_memory(reinterpret_cast<PVOID>(test_platform), reinterpret_cast<uint8_t*>(platform), sizeof(platform));
	std::wstring platform_wstr(platform);
	std::string platform_str(platform_wstr.begin(), platform_wstr.end());
	return platform_str;
}

Vector2 ue5::w2s(Vector3 WorldLocation)
{
	D3DMATRIX tempMatrix = Matrix(Rotation);

	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;
	float precalc = (((ue5::width / 2.0f) / tanf(FOV * 0.00872664625f))) / vTransformed.z;
	return Vector2((ue5::width / 2.0f) + vTransformed.x * precalc, (ue5::height / 2.0f) - vTransformed.y * precalc);
}

std::string ue5::get_weapon_name(uintptr_t actor)
{
	uintptr_t current_weapon = luminox->read<uintptr_t>(actor + 0xA68);

	uint64_t weapon_data = luminox->read<uint64_t>(current_weapon + 0x4f0);
	if (!weapon_data) return "";
	uint64_t fname_text = luminox->read<uint64_t>(weapon_data + 0x40);
	if (!fname_text) return "";
	uint32_t name_length = luminox->read<uint32_t>(fname_text + 0x30);
	if (!name_length) return "";
	wchar_t* WeaponName = new wchar_t[uint64_t(name_length) + 1];

	luminox->read_memory((PVOID)luminox->read<PVOID>(fname_text + 0x28), (WeaponName), name_length * sizeof(wchar_t));

	std::wstring wWeaponName(WeaponName);
	return std::string(wWeaponName.begin(), wWeaponName.end());
}

std::string ue5::get_player_name(__int64 PlayerState)
{
	auto fstring = (luminox->read<__int64>)(PlayerState + 0xaf0);
	auto length = (luminox->read<int>)(fstring + 16);

	auto v6 = (__int64)length;
	if (!v6) return std::string(E("AI/NONE"));

	auto ftext = (uintptr_t)(luminox->read<__int64>)(fstring + 8);

	wchar_t* buffer = new wchar_t[length];
	luminox->read_memory((PVOID)ftext, buffer, length * sizeof(wchar_t));

	char v21;
	int v22;
	int i;

	int v25;
	UINT16* v23;

	v21 = v6 - 1;
	if (!(UINT32)v6)
		v21 = 0;
	v22 = 0;
	v23 = (UINT16*)buffer;
	for (i = (v21) & 3; ; *v23++ += i & 7)
	{
		v25 = v6 - 1;
		if (!(UINT32)v6)
			v25 = 0;
		if (v22 >= v25)
			break;
		i += 3;
		++v22;
	}

	std::wstring username{ buffer };
	return std::string(username.begin(), username.end());
}

bool ue5::mouse(float x, float y)
{
	Vector3 center(ue5::width / 2, ue5::height / 2, 0);
	FVector target(0, 0, 0);

	if (x != 0) {
		if (x > center.x) {
			target.x = -(center.x - x);
			target.x /= (a1m::smoothing);
			if (target.x + center.x > center.x * 2)
				target.x = 0;
		}

		if (x < center.x) {
			target.x = x - center.x;
			target.x /= (a1m::smoothing);
			if (target.x + center.x < 0)
				target.x = 0;
		}
	}
	if (y != 0) {
		if (y > center.y) {
			target.y = -(center.y - y);
			target.y /= (a1m::smoothing);
			if (target.y + center.y > center.y * 2)
				target.y = 0;
		}

		if (y < center.y) {
			target.y = y - center.y;
			target.y /= (a1m::smoothing);
			if (target.y + center.y < 0)
				target.y = 0;
		}
	}

	luminox->move_mouse(target.x, target.y);

	return true;
}
bool ue5::is_shootable(Vector3 lur, Vector3 wl)
{
	if (lur.x >= wl.x - 20 && lur.x <= wl.x + 20 && lur.y >= wl.y - 20 && lur.y <= wl.y + 20 && lur.z >= wl.z - 30 && lur.z <= wl.z + 30)
		return true;
	else
		return false;

}