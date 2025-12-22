#pragma once
using f64 = double;
using i64 = long long;
using u64 = unsigned long long;
using i32 = int;
using u32 = unsigned int;
using f32 = float;
using i16 = short;
using u16 = unsigned short;
using i8 = char;
using u8 = unsigned char;

constexpr i32 MAX_BULLET_SIZE = 1024;
constexpr i32 MAX_LASER_SIZE = 64;
constexpr i32 SCREEN_WIDTH = 1280;
constexpr i32 SCREEN_HEIGHT = 960;
constexpr f32 SCREEN_WIDTH_F32 = 1280.0f;
constexpr f32 SCREEN_HEIGHT_F32 = 960.0f;
constexpr i32 FPS = 60;
constexpr i32 ID_SMALL_POWER = 0;
constexpr i32 ID_BIG_POWER = 1;
constexpr i32 ID_POINT = 2;
constexpr i32 ID_LIFE_FRAG = 3;
constexpr i32 ID_FULL_LIFE = 4;
constexpr i32 ID_BIG_DROP = 5;
constexpr i32 ID_SCORE_RAIN = 6;
struct Data {
	u8 id;
	u8 life;
	u8 difficulty;
	i64 high_score;
	i64 score;
	i64 graze;
	f32 power;
};
struct ScoreEntry {
	i64 score;
	char name[16];
	char date[10];
	char stage[10];
};
struct Config {
	i32 bgm_sound;
	i32 sfx_sound;
};
inline const Config default_config{ 100, 80 };
inline Config config = default_config;
inline Data gamedata{ 0 };

inline void ResetData() {
	gamedata.id = 0;
	gamedata.life = 0;
	gamedata.difficulty = 0;
	gamedata.high_score = 0;
	gamedata.score = 0;
	gamedata.graze = 0;
	gamedata.power = 0.0f;
}