#include "BulletManager.h"
#include "Bullet.h"
#include "BulletComponent.h"
#include "../Engine/maths.h"
namespace Game::Manager {
	using namespace Game::Bullets;

	BulletManager::BulletManager(Engine::Core::Context& _ctx)
		:ctx(_ctx)
	{ }

	BulletManager::~BulletManager()
	{

	}

	//bulletIdx1用于角度散布，bulletIdx2用于速度渐变
	void BulletManager::SpawnSingleBullet(
		Game::GameObject::EnemyBulletShooter* props, 
		i32 bulletIdx1, i32 bulletIdx2, f32 angle)
	{
		f32 bullet_angle = 0.0f;
		Bullet* bullet = nullptr;
		i32 local_c = 0;
		f32 bullet_speed = 0.0f;

		bullet = &bullets[next_bullet_index];
		for (local_c = 0; local_c < MAX_BULLET_SIZE; ++local_c) {
			++next_bullet_index;
			//对象池循环使用
			if (next_bullet_index >= MAX_BULLET_SIZE) {
				next_bullet_index = 0;
			}
			//状态不为0(state表示是否被占用)
			if (bullet->GetBulletData()->state != 0) {
				++bullet;
				//只要指向0，表示满了，满了就循环使用
				if (next_bullet_index == 0) {
					bullet = &bullets[0];
				}
				continue;
			}
			break;
		}

		if (local_c >= MAX_BULLET_SIZE) {
			return;
		}

		bullet_angle = 0.0f;
		bullet_speed = props->speed1 - (props->speed1 - props->speed2)
			* bulletIdx2 / props->count2;
		switch (props->aim_mode) {
		case AimMode::FAN_AIMED:
		case AimMode::FAN:
		{
			if (props->count1 & 1) {
				bullet_angle += ((bulletIdx1 + 1) / 2) * props->angle2;
			}
			else {
				bullet_angle += (bulletIdx1 / 2) * props->angle2
					+ props->angle2 * 0.5f;
			}

			if (bulletIdx1 & 1) {
				bullet_angle *= -1.0f;
			}
			if (props->aim_mode == AimMode::FAN_AIMED) {
				bullet_angle += angle;
			}

			bullet_angle += props->angle1;
		}break;
		case AimMode::CIRCLE_AIMED:
			bullet_angle += angle;
		case AimMode::CIRCLE:
			bullet_angle += bulletIdx1 * _2PI / props->count1 
				+ bulletIdx2 * props->angle2 + props->angle1;
			break;
		case AimMode::OFFSET_CIRCLE_AIMED:
			bullet_angle += angle;
		case AimMode::OFFSET_CIRCLE:
			bullet_angle += _PI / props->count1
				+ bulletIdx1 * _2PI / props->count1
				+ props->angle1;
			break;
		case AimMode::RANDOM_ANGLE:
			bullet_angle = Engine::Maths::GetRandomFloat(0.0f, props->angle1 - props->angle2);
				+ props->angle2;
			break;
		case AimMode::RANDOM_SPEED:
			bullet_speed = Engine::Maths::GetRandomFloat(0.0f, props->speed1 - props->speed2);
				+ props->speed2;
			bullet_angle += bulletIdx1 * _2PI / props->count1
				+ bulletIdx2 * props->angle2 + props->angle1;
			break;
		case AimMode::RANDOM:
			bullet_angle = Engine::Maths::GetRandomFloat(0.0f, props->angle1 - props->angle2) + props->angle2;
			bullet_speed = Engine::Maths::GetRandomFloat(0.0f, props->speed1 - props->speed2)
				+ props->speed2;
		}
		auto bt = bullet->GetBulletData();
		bt->state = 1;
		bt->is_active = 1;
		bt->speed = bullet_speed;
		bt->angle = Engine::Maths::NormalizeAngle(bullet_angle, 0.0f);
		bt->position = props->position;
		bt->velocity = Engine::Maths::SpeedAngleToVec2(bullet_speed, bt->angle);
		bt->sprite_offset = props->sprite_offset;
		bt->ex_flags = props->flags;
		bt->sprites.anim = bullet_type_templates[props->sprite].anim;
		bt->sprites.spawn_effect_donut = bullet_type_templates[props->sprite].spawn_effect_donut;
		bt->sprites.bullet_width = bullet_type_templates[props->sprite].bullet_width;
		bt->sprites.bullet_height = bullet_type_templates[props->sprite].bullet_height;

		if (bt->ex_flags & 0x10) {
			if (props->exFloats[1] <= -999.0f) {
				bt->ex4_acceleration = Engine::Maths::SpeedAngleToVec2(bullet_angle, props->exFloats[0]);
			}
			else {
				bt->ex4_acceleration = Engine::Maths::SpeedAngleToVec2(props->exFloats[1], props->exFloats[0]);
			}

			if (props->exInts[0] > 0) {
				bt->ex5_lifespan = props->exInts[0];
			}
			else {
				bt->ex5_lifespan = 99999;
			}
		}
		else if (bt->ex_flags & 0x20) {
			bt->ex5_angular_velocity = props->exFloats[0];
			bt->ex5_acceleration_speed = props->exFloats[1];
			bt->ex5_lifespan = props->exInts[0];
		}

		if (bt->ex_flags & 0x1c0) {
			bt->dir_change_rotation = props->exFloats[0];
		
			if (props->exFloats[1] >= 0.0f) {
				bt->dir_change_speed = props->exFloats[1];
			}
			else {
				bt->dir_change_speed = bullet_speed;
			}

			bt->dir_change_interval = props->exInts[0];
			bt->dir_change_max_times = props->exInts[1];
			bt->dir_change_num_times = 0;
		}

		if (bt->ex_flags & 0xc00) {
			if (props->exFloats[0] >= 0.0f) {
				bt->dir_change_speed = props->exFloats[0];
			}
			else {
				bt->dir_change_speed = bullet_speed;
			}

			bt->dir_change_max_times = props->exInts[0];
			bt->dir_change_num_times = 0;
		}
	}


	void BulletManager::Update(float d_t) {
		for (int i = 0; i < MAX_BULLET_SIZE; i++) {
			if (bullets[i].GetBulletData()->state != 0) {
				bullets[i].GetGameObject()->Update(d_t, ctx);
			}
		}
	}

	void BulletManager::Render() {
		for (int i = 0; i < MAX_BULLET_SIZE; i++) {
			if (bullets[i].GetBulletData()->state != 0) {
				bullets[i].GetGameObject()->Render(ctx);
			}
		}

	}

}