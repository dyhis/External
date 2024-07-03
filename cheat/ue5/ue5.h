#pragma once

#include "../SDK.h"

#include "../loops/loop.h"

#include <unordered_map>

class ue5 
{
public:
	static bool is_shootable(Vector3 lur, Vector3 wl);

	static bool setup_camera();

	static float distance_from_crosshair(Vector2 point);

	static std::string get_platform(uintptr_t player_state);

	static std::string get_weapon_name(uintptr_t actor);

	static bool mouse(float x, float y);

	static bool get_bone_array(entity* pawn);

	static Vector3 get_bone(entity* pawn, int bone_id);

	static bool visible(uintptr_t mesh);

	static Vector2 w2s(Vector3 WorldLocation);

	static std::string get_player_name(__int64 PlayerState);

	static inline int
		width,
		height;
};
