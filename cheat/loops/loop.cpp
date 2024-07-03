#include "loop.h"

#include "../driver/driver.h"

#include "../ue5/ue5.h"

#include "../SDK.h"

#include "../../../backend/encryption/Importer.h"
#include "../../../backend/encryption/SkCrypt.h"

#include <unordered_map>

#include "../aim/aim.h"
double closest_distance;
entity closest_mesh;
Vector3 Velocity;
float projectileSpeed = 0;
float projectileGravityScale = 0;
bool has_clicked;
std::chrono::steady_clock::time_point tb_begin;
std::chrono::steady_clock::time_point tb_end;
int tb_time_since;
bool setup_camera = false;


void loop::camera_update()
{
	
		if (!globals.pointers.view_state)
			return;

		FMatrix projection = luminox->read<FMatrix>(globals.pointers.view_state + 0x900);

		Rotation.x = asin(projection.ZPlane.W) * (180.0 / M_PI);
		Rotation.y = atan2(projection.YPlane.W, projection.XPlane.W) * (180.0 / M_PI);
		Rotation.z = 0.0;

		Location.x = projection.m[3][0];
		Location.y = projection.m[3][1];
		Location.z = projection.m[3][2];

		double fov = atanf(1 / luminox->read<double>(globals.pointers.view_state + 0x700)) * 2;

		FOV = (fov) * (180.f / M_PI);
	
}

void loop::cache_pointers()
{
	while (1)
	{
		temporary_entity_list.clear();

		globals.pointers.gworld = luminox->read<uintptr_t>(luminox->base + Offsets.UWorld);

		if (!globals.pointers.gworld)
			continue;

		globals.pointers.game_instance = luminox->read<uintptr_t>(globals.pointers.gworld + Offsets.GameInstance);

		globals.pointers.game_state = luminox->read<uintptr_t>(globals.pointers.gworld + Offsets.GameState);

		if (!(globals.pointers.game_instance or globals.pointers.game_state))
			continue;

		globals.pointers.local_player = luminox->read<uintptr_t>(luminox->read<uintptr_t>(globals.pointers.game_instance + Offsets.LocalPlayers));

		if (setup_camera == false)
		{
			auto view_matrix = luminox->read<uintptr_t>(globals.pointers.local_player + 0xd0);

			globals.pointers.view_state = luminox->read<uintptr_t>(view_matrix + 0x8);

			setup_camera = true;
		}
		
		globals.pointers.player_controller = luminox->read<uintptr_t>(globals.pointers.local_player + Offsets.PlayerController);

		globals.pointers.local_pawn = luminox->read<uintptr_t>(globals.pointers.player_controller + Offsets.LocalPawn);

		globals.pointers.player_state = luminox->read<uintptr_t>(globals.pointers.local_pawn + Offsets.PlayerState);

		globals.pointers.teamindex = luminox->read<int>(globals.pointers.player_state + Offsets.TeamIndex);

		globals.pointers.root_component = luminox->read<uintptr_t>(globals.pointers.local_pawn + Offsets.RootComponent);

		std::this_thread::sleep_for(std::chrono::seconds(3));
	}

}

void loop::cache_entities()
{
	while (1) 
	{
		globals.pointers.player_array = luminox->read<uintptr_t>(globals.pointers.game_state + Offsets.PlayerArray);

		globals.pointers.player_count = luminox->read<int>(globals.pointers.game_state + (Offsets.PlayerArray + sizeof(uintptr_t)));

		for (int i = 0; i < globals.pointers.player_count; ++i)
		{
			entity cached_actor;

			cached_actor.player_state = luminox->read<uintptr_t>(globals.pointers.player_array + (i * sizeof(uintptr_t)));
			if (!cached_actor.player_state) continue;

			cached_actor.team_index = luminox->read<int>(cached_actor.player_state + Offsets.TeamIndex);

			if (!cached_actor.name_read) cached_actor.name = ue5::get_player_name(cached_actor.player_state);
			
			if (cached_actor.name != "") cached_actor.name_read = true;

			if (!cached_actor.platform_read) cached_actor.platform = ue5::get_platform(cached_actor.player_state);

			if (cached_actor.platform != "") cached_actor.platform_read = true;

			cached_actor.entity = luminox->read<uintptr_t>(cached_actor.player_state + Offsets.PawnPrivate);

			if (cached_actor.entity == globals.pointers.local_pawn)
				continue;

			if (cached_actor.entity && cached_actor.entity != globals.pointers.local_pawn)
			{
				cached_actor.mesh = luminox->read<uint64_t>(cached_actor.entity + Offsets.ActorMesh);

				if (!cached_actor.mesh) continue;

				temporary_entity_list.push_back(cached_actor);
			}
		}
		entity_list.clear();
		entity_list = temporary_entity_list;
		std::this_thread::sleep_for(std::chrono::seconds(3));
	}
}

void box(int x, int y, int width, int height, int thickness, ImColor color) 
{
	ImVec2 topLeft = ImVec2(x, y);
	ImVec2 bottomRight = ImVec2(x + width, y + height);

	_render::Backround_drawlist->AddRectFilled(topLeft, ImVec2(bottomRight.x, topLeft.y + thickness), color);
	_render::Backround_drawlist->AddRectFilled(topLeft, ImVec2(topLeft.x + thickness, bottomRight.y), color);
	_render::Backround_drawlist->AddRectFilled(ImVec2(bottomRight.x - thickness, topLeft.y), bottomRight, color);
	_render::Backround_drawlist->AddRectFilled(ImVec2(topLeft.x, bottomRight.y - thickness), bottomRight, color);
}

void draw_cornered_box(int x, int y, int w, int h, bool visible, int thickness)
{
	if (!visible) 
	{
		_render::Backround_drawlist->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 6)), ImColor(220, 20, 60), thickness);
		_render::Backround_drawlist->AddLine(ImVec2(x, y), ImVec2(x + (w / 6), y), ImColor(220, 20, 60), thickness);
		_render::Backround_drawlist->AddLine(ImVec2(x + w - (w / 6), y), ImVec2(x + w, y), ImColor(220, 20, 60), thickness);
		_render::Backround_drawlist->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 6)), ImColor(220, 20, 60), thickness);
		_render::Backround_drawlist->AddLine(ImVec2(x, y + h - (h / 6)), ImVec2(x, y + h), ImColor(220, 20, 60), thickness);
		_render::Backround_drawlist->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 6), y + h), ImColor(220, 20, 60), thickness);
		_render::Backround_drawlist->AddLine(ImVec2(x + w - (w / 6), y + h), ImVec2(x + w, y + h), ImColor(220, 20, 60), thickness);
		_render::Backround_drawlist->AddLine(ImVec2(x + w, y + h - (h / 6)), ImVec2(x + w, y + h), ImColor(220, 20, 60), thickness);
	}
	else if (visible)
	{
		_render::Backround_drawlist->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 6)), ImColor(0, 255, 255), thickness);
		_render::Backround_drawlist->AddLine(ImVec2(x, y), ImVec2(x + (w / 6), y), ImColor(0, 255, 255), thickness);
		_render::Backround_drawlist->AddLine(ImVec2(x + w - (w / 6), y), ImVec2(x + w, y), ImColor(0, 255, 255), thickness);
		_render::Backround_drawlist->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 6)), ImColor(0, 255, 255), thickness);
		_render::Backround_drawlist->AddLine(ImVec2(x, y + h - (h / 6)), ImVec2(x, y + h), ImColor(0, 255, 255), thickness);
		_render::Backround_drawlist->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 6), y + h), ImColor(0, 255, 255), thickness);
		_render::Backround_drawlist->AddLine(ImVec2(x + w - (w / 6), y + h), ImVec2(x + w, y + h), ImColor(0, 255, 255), thickness);
		_render::Backround_drawlist->AddLine(ImVec2(x + w, y + h - (h / 6)), ImVec2(x + w, y + h), ImColor(0, 255, 255), thickness);

		_render::Backround_drawlist->AddRectFilled(ImVec2(x + w, y + h - (h / 6)), ImVec2(x + w, y + h), ImColor(0, 255, 255, 80), thickness);
	}
}

void loop::local_weapon() 
{
	while (true)
	{
		std::string weapon = ue5::get_weapon_name(globals.pointers.local_pawn);

		if (!weapon.empty())
			globals.pointers.local_weapon = weapon;

		if (exploit::Magic)
		{
			uintptr_t current_weapon = luminox->read<uintptr_t>(globals.pointers.local_pawn + Offsets.CurrentWeapon);
			if (current_weapon) luminox->write<bool>(current_weapon + 0xdb5, true);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
}

void loop::actor_loop()
{
	loop::camera_update();

	closest_distance = DBL_MAX;
	closest_mesh.reset();

	std::string local_player_weapon = globals.pointers.local_weapon;

	if (esp::render_fov)
	{
		ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(ue5::width / 2, ue5::height / 2), a1m::fov, ImColor(255, 255, 255), 64, 2);
	}
	else 
	{

	}
	for (auto& cached : entity_list)
	{
		if (cached.team_index == globals.pointers.teamindex && esp::team_check) continue;

		ue5::get_bone_array(&cached);

		Vector3 root_location = ue5::get_bone(&cached, Bones::HumanBase);

		Vector2 root_screen = ue5::w2s(root_location);

		auto head_location = ue5::get_bone(&cached, Bones::HumanHead);
		auto head_screen = ue5::w2s(ue5::get_bone(&cached, Bones::HumanHead));

		Vector2 head_box = ue5::w2s(Vector3(head_location.x, head_location.y, head_location.z + 15));
		Vector2 head_2d = ue5::w2s(Vector3(head_location.x, head_location.y, head_location.z + 10));

		float box_height = abs(head_screen.y - root_screen.y);
		float box_width = box_height * 0.50f;
		float distance = Location.Distance(root_location) / 100;

		ImColor esp_color = ImColor(127, 255, 0);

		bool visible = ue5::visible(cached.mesh);

		if (!visible)
		{
			esp_color = ImColor(186, 85, 211);
		}

		if (esp::box)
		{
			float CornerHeight = abs(head_box.y - root_screen.y);
			float CornerWidth = box_height * 0.55f;
			
			box(head_box.x - (CornerWidth / 2), head_box.y, CornerWidth, CornerHeight, 2, esp_color);
		}
		if (esp::skeleton) 
		{
			Vector2 pelvis = ue5::w2s(ue5::get_bone(&cached, 2));
			Vector2 chest = ue5::w2s(ue5::get_bone(&cached, 66));
			Vector2 left_shoulder = ue5::w2s(ue5::get_bone(&cached, 105));
			Vector2 left_elbow = ue5::w2s(ue5::get_bone(&cached, 10));
			Vector2 left_hand = ue5::w2s(ue5::get_bone(&cached, 11));
			Vector2 right_shoulder = ue5::w2s(ue5::get_bone(&cached, 9));
			Vector2 neck = ue5::w2s(ue5::get_bone(&cached, 67));

			Vector2 left_hip = ue5::w2s(ue5::get_bone(&cached, 71));
			Vector2 right_hip = ue5::w2s(ue5::get_bone(&cached, 78));

			Vector2 right_elbow = ue5::w2s(ue5::get_bone(&cached, 39));
			Vector2 right_hand = ue5::w2s(ue5::get_bone(&cached, 40));
			Vector2 left_knee = ue5::w2s(ue5::get_bone(&cached, 72));
			Vector2 left_foot = ue5::w2s(ue5::get_bone(&cached, 73));
			Vector2 right_knee = ue5::w2s(ue5::get_bone(&cached, 79));
			Vector2 right_foot = ue5::w2s(ue5::get_bone(&cached, 82));
			//btw shoulders are opposite oopsies
			
			_render::Backround_drawlist->AddLine(ImVec2(neck.x, neck.y), ImVec2(chest.x, chest.y), esp_color, 2.0f);
			_render::Backround_drawlist->AddLine(ImVec2(chest.x, chest.y), ImVec2(pelvis.x, pelvis.y), esp_color, 2.0f);
			_render::Backround_drawlist->AddLine(ImVec2(left_shoulder.x, left_shoulder.y), ImVec2(right_shoulder.x, right_shoulder.y), esp_color, 2.0f);
			//_render::Backround_drawlist->AddLine(ImVec2(chest.x, chest.y), ImVec2(left_shoulder.x, left_shoulder.y), esp_color, 2.0f);
			_render::Backround_drawlist->AddLine(ImVec2(left_elbow.x, left_elbow.y), ImVec2(left_hand.x, left_hand.y), esp_color, 2.0f);
			_render::Backround_drawlist->AddLine(ImVec2(left_shoulder.x, left_shoulder.y), ImVec2(right_elbow.x, right_elbow.y), esp_color, 2.0f);
			_render::Backround_drawlist->AddLine(ImVec2(right_shoulder.x, right_shoulder.y), ImVec2(left_elbow.x, left_elbow.y), esp_color, 2.0f);

			_render::Backround_drawlist->AddLine(ImVec2(right_elbow.x, right_elbow.y), ImVec2(right_hand.x, right_hand.y), esp_color, 2.0f);
			_render::Backround_drawlist->AddLine(ImVec2(pelvis.x, pelvis.y), ImVec2(left_hip.x, left_hip.y), esp_color, 2.0f);
			_render::Backround_drawlist->AddLine(ImVec2(left_hip.x, left_hip.y), ImVec2(left_knee.x, left_knee.y), esp_color, 2.0f);
			_render::Backround_drawlist->AddLine(ImVec2(left_knee.x, left_knee.y), ImVec2(left_foot.x, left_foot.y), esp_color, 2.0f);
			_render::Backround_drawlist->AddLine(ImVec2(pelvis.x, pelvis.y), ImVec2(right_hip.x, right_hip.y), esp_color, 2.0f);
			_render::Backround_drawlist->AddLine(ImVec2(right_hip.x, right_hip.y), ImVec2(right_knee.x, right_knee.y), esp_color, 2.0f);

			_render::Backround_drawlist->AddLine(ImVec2(right_knee.x, right_knee.y), ImVec2(right_foot.x, right_foot.y), esp_color, 2.0f);
		}
		
		if (esp::distance) 
		{
			std::string dist = "[" + std::to_string(static_cast<int>(distance)) + "M]";

			ImVec2 TextSize = ImGui::CalcTextSize(dist.c_str());

			_render::Backround_drawlist->AddText(ImVec2((root_screen.x) - (TextSize.x / 2), (root_screen.y + 5)), esp_color, dist.c_str());
		}

		if (esp::platform)
		{
			ImVec2 TextSize = ImGui::CalcTextSize(cached.platform.c_str());

			_render::Backround_drawlist->AddText(ImVec2((head_2d.x) - (TextSize.x / 2), (head_2d.y - 45)), esp_color, cached.platform.c_str());
		}

		if (esp::name)
		{
			ImVec2 TextSize = ImGui::CalcTextSize(cached.name.c_str());

			_render::Backround_drawlist->AddText(ImVec2((head_2d.x) - (TextSize.x / 2), (head_2d.y - 30)), esp_color, cached.name.c_str());
		}

		if (triggerbot::enabled)
		{
			if (triggerbot::shotgun_only && strstr(local_player_weapon.c_str(), " Shotgun"))
			{
				if (triggerbot::always_on) 
				{
					Vector3 location_under_reticle = luminox->read<Vector3>(globals.pointers.player_controller + Offsets.location_under_reticle);

					if (ue5::is_shootable(location_under_reticle, head_location))
					{
						if (distance <= triggerbot::maximum_distance) 
						{
							if (has_clicked) {
								tb_begin = std::chrono::steady_clock::now();
								has_clicked = 0;
							}
							tb_end = std::chrono::steady_clock::now();
							tb_time_since = std::chrono::duration_cast<std::chrono::milliseconds>(tb_end - tb_begin).count();
							if (tb_time_since >= triggerbot::delay) 
							{
								aim::click();
								has_clicked = 1;
							}
						}
					}
				}
				else
				{
					if (GetAsyncKeyState(triggerbot::triggerbot_key)) 
					{
						Vector3 location_under_reticle = luminox->read<Vector3>(globals.pointers.player_controller + Offsets.location_under_reticle);
						if (ue5::is_shootable(location_under_reticle, head_location))
						{
							if (distance <= triggerbot::maximum_distance)
							{
								if (has_clicked) {
									tb_begin = std::chrono::steady_clock::now();
									has_clicked = 0;
								}
								tb_end = std::chrono::steady_clock::now();
								tb_time_since = std::chrono::duration_cast<std::chrono::milliseconds>(tb_end - tb_begin).count();
								if (tb_time_since >= triggerbot::delay)
								{
									aim::click();
									has_clicked = 1;
								}
							}
						}
					}
				}
			}
		}

		auto d_x = head_screen.x - (ue5::width / 2);
		auto d_y = head_screen.y - (ue5::height / 2);
		auto square_root = sqrtf(d_x * d_x + d_y * d_y);

		if (square_root < a1m::fov && square_root < closest_distance)
		{
			if (!visible) continue;

			closest_distance = square_root;
			closest_mesh = cached;

			uintptr_t root_componet = luminox->read<uintptr_t>(cached.entity + Offsets.RootComponent);

			Velocity = luminox->read<Vector3>(root_componet + Offsets.Velocity);
		}
	}
	if (!(a1m::aimbot && closest_mesh.mesh && globals.pointers.local_pawn)) return;
	
	if (closest_mesh.team_index == globals.pointers.teamindex && esp::team_check) return;

	ue5::get_bone_array(&closest_mesh);

	Vector3 target;

	switch (a1m::hitbox)
	{
	case 0:
		target = ue5::get_bone(&closest_mesh, 110);
		break;
	case 1:
		target = ue5::get_bone(&closest_mesh, 66);
		break;
	case 2:
		target = ue5::get_bone(&closest_mesh, 7);
		break;
	case 3:
		target = ue5::get_bone(&closest_mesh, 2);
		break;
	case 4:
		Vector3 target1 = ue5::get_bone(&closest_mesh, 110);
		Vector2 targetw2s1 = ue5::w2s(target1);
		float distance1 = ue5::distance_from_crosshair(targetw2s1);

		Vector3 target2 = ue5::get_bone(&closest_mesh, 66);
		Vector2 targetw2s2 = ue5::w2s(target2);
		float distance2 = ue5::distance_from_crosshair(targetw2s2);

		Vector3 target3 = ue5::get_bone(&closest_mesh, 7);
		Vector2 targetw2s3 = ue5::w2s(target3);
		float distance3 = ue5::distance_from_crosshair(targetw2s3);

		Vector3 target4 = ue5::get_bone(&closest_mesh, 2);
		Vector2 targetw2s4 = ue5::w2s(target4);
		float distance4 = ue5::distance_from_crosshair(targetw2s4);

		if (distance1 < distance2 && distance3 && distance4)
			target = target1;
		else if (distance2 < distance1 && distance3 && distance4)
			target = target2;
		else if (distance3 < distance1 && distance2 && distance4)
			target = target3;
		else if (distance4 < distance1 && distance2 && distance3)
			target = target4;

		break;
	}

	float distance = Location.Distance(target);

	//prediction values
	{
		if (strstr(local_player_weapon.c_str(), (E("Warforged Assault Rifle"))))
		{
			projectileSpeed = 80000.f;
			projectileGravityScale = 3.5f;
		}
		else if (strstr(local_player_weapon.c_str(), (E("Ares' Warforged Assault Rifle"))))
		{
			projectileSpeed = 80000.f;
			projectileGravityScale = 3.5f;
		}
		else if (strstr(local_player_weapon.c_str(), (E("Huntress DMR"))))
		{
			projectileSpeed = 96000.f;
			projectileGravityScale = 2.5f;
		}
		else if (strstr(local_player_weapon.c_str(), (E("Zeus' Huntress DMR")))) 
		{
			projectileSpeed = 96000.f;
			projectileGravityScale = 2.5f;
		}
		else if (strstr(local_player_weapon.c_str(), (E("Nemesis AR"))))
		{
			projectileSpeed = 80000;
			projectileGravityScale = 3.5;
		}
		else if (strstr(local_player_weapon.c_str(), (E("Reaper Sniper Rifle"))))
		{
			projectileSpeed = 50000;
			projectileGravityScale = 3.3;
		}
		else if (strstr(local_player_weapon.c_str(), (E("Ranger Pistol"))))
		{
			projectileSpeed = 60000;
			projectileGravityScale = 2;
		}
		else if (strstr(local_player_weapon.c_str(), (E("Hand Cannon"))))
		{
			projectileSpeed = 58000;
			projectileGravityScale = 2;
		}
		else if (strstr(local_player_weapon.c_str(), (E("Thunder Burst SMG"))))
		{
			projectileSpeed = 70000;
			projectileGravityScale = 3;
		}
		else if (strstr(local_player_weapon.c_str(), (E("Harbinger SMG"))))
		{
			projectileSpeed = 70000;
			projectileGravityScale = 3;
		}
		else if (strstr(local_player_weapon.c_str(), (E("Drum Gun"))))
		{
			projectileSpeed = 75000;
			projectileGravityScale = 3;
		}
		else if (strstr(local_player_weapon.c_str(), (E("Hades' Harbinger SMG"))))
		{
			projectileSpeed = 70000;
			projectileGravityScale = 3;
		}
		else {
			projectileSpeed = 0;
			projectileGravityScale = 0;
		}
	}

	if (projectileSpeed != 0)
	{
		target = predict_location(target, Velocity, distance, projectileSpeed, projectileGravityScale);
	}

	auto target_w2s = ue5::w2s(target);

	bool aimkey = GetAsyncKeyState(a1m::aim_key);

	if (aimkey && target_w2s.x != 0)
	{
		ue5::mouse(target_w2s.x, target_w2s.y);
	}
}
