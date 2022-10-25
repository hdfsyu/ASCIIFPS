#include <iostream>
#include <string>
#include <algorithm>
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
class ASCIIFPS : public olc::PixelGameEngine {
public:
	ASCIIFPS() {
		sAppName = "ASCIIFPS";
	}
private:
	float plx = 8.0f;//player x pos
	float ply = 8.0f;//player y pos
	float pla = 0.0f;//player angle
	int mh = 16;//map height
	int mw = 16;//map width
	float fov = 3.14159 / 4.0;
	float depth = 16.0f;
	float speed = 3.0f;
	std::string map;
	olc::Sprite *wall;
	olc::Sprite *lamp;
	olc::Sprite *fireball;
	float *db = nullptr;
	struct object {
		float x;
		float y;
		float vx;//velocity x
		float vy;//velocity y
		bool remove;//is the object removed check
		olc::Sprite *sprite;
	};
	std::list<object> lo; //list objects
protected:
	virtual bool ouc() {
		//16x16 map, replace a dot with a hashtag to make a block/wall there
		map += "################";
		map += "#..............#";
		map += "#..##........#.#";
		map += "#......#....#..#";
		map += "#..............#";
		map += "#....#####.....#";
		map += "#..............#";
		map += "#.####......#..#";
		map += "#.#..#.........#";
		map += "#.#..#.........#";
		map += "#.#..#......#..#";
		map += "#.#..#.........#";
		map += "#......###.....#";
		map += "#..##########..#";
		map += "#......##......#";
		map += "################";
		wall = new olc::Sprite("spr/fps_wall1.png");
		lamp = new olc::Sprite("spr/fps_lamp1.png");
		fireball = new olc::Sprite("spr/fps_fireball1.png");
		db = new float[sw()];
		lo = {
			{4.5f, 8.5f, 0.0f, 0.0f, false, lamp},
			{3.5f, 7.5f, 0.0f, 0.0f, false, lamp},
			{6.2f, 3.5f, 0.0f, 0.0f, false, lamp}
		};
		return true;
	}
	virtual bool oup(float fet) {
		if (GetKey(olc::Key::A).bHeld)
			pla -= (speed * 0.5f) * fet;
		if (GetKey(olc::Key::D).bHeld)
			pla += (speed * 0.5f) * fet;
		if (GetKey(olc::Key::W).bHeld)
		{
			plx += sinf(pla) * speed * fet;;
			ply += cosf(pla) * speed * fet;;
			if (map.c_str()[(int)plx * mw + (int)ply] == '#')
			{
				plx -= sinf(pla) * speed * fet;;
				ply -= cosf(pla) * speed * fet;;
			}
		}
		if (GetKey(olc::Key::S).bHeld)
		{
			plx -= sinf(pla) * speed * fet;;
			ply -= cosf(pla) * speed * fet;;
			if (map.c_str()[(int)plx * mw + (int)ply] == '#')
			{
				plx += sinf(pla) * speed * fet;;
				ply += cosf(pla) * speed * fet;;
			}
		}
		if (GetKey(olc::Key::E).bHeld)
		{
			plx += cosf(pla) * speed * fet;
			ply -= sinf(pla) * speed * fet;
			if (map.c_str()[(int)plx * mw + (int)ply] == '#')
			{
				plx -= cosf(pla) * speed * fet;
				ply += sinf(pla) * speed * fet;
			}
		}
		if (GetKey(olc::Key::Q).bHeld)
		{
			plx -= cosf(pla) * speed * fet;
			ply += sinf(pla) * speed * fet;
			if (map.c_str()[(int)plx * mw + (int)ply] == '#')
			{
				plx += cosf(pla) * speed * fet;
				ply -= sinf(pla) * speed * fet;
			}
		}
		if (GetKey(olc::Key::SPACE).bReleased) {
			object o;
			o.x = plx;
			o.y = ply;
			float movement = (((float)rand() / (float)RAND_MAX) - 0.5f) * 0.1f;
			o.vx = sinf(pla + movement) * 8.0f;
			o.vy = cosf(pla + movement) * 8.0f;
			o.sprite = fireball;
			o.remove = false;
			lo.push_back(o);
		}
			for (int x = 0; x < sw(); x++) {
				float ra = (pla - fov / 2.0f) + ((float)x / (float)sw())*fov;//ray angle
				float ss = 0.01f;//step size
				float dtw = 0;//distance to wall
				bool hw = false; //did player hit wall
				float ex = cosf(ra);//eye x
				float ey = sinf(ra);//eye y
				bool boundary = false;
				float sx = 0.0f;//sample x
				while (!hw && dtw < depth) {
					dtw += ss;
					int tex = (int)(plx + ex * dtw);//test x
					int tey = (int)(plx + ey * dtw);//test y
					if (tex < 0 || tex >= mw || tey < 0 || tey >= mh) {
						hw = true;
						dtw = depth;
					}
					else {
						if (map.c_str()[tex * mw + tey] == '#') {
							hw = true;
							float bmx = (float)tex + 0.5f;//block mid x
							float bmy = (float)tey + 0.5f;//block mid y
							float tpx = plx + ex * dtw;//test point x
							float tpy = ply + ey * dtw;//test point y
							float ta = atan2f((tpy - bmy), (tpx - bmx));
							if (ta >= -3.14159f * 0.25f && ta < 3.14159f * 0.25f)
								sx = tpy - (float)tey;
							if (ta >= 3.14159f * 0.25f && ta < 3.14159f * 0.75f)
								sx = tpx - (float)tex;
							if (ta < -3.14159f * 0.25f && ta >= -3.14159f * 0.75f)
								sx = tpx - (float)tex;
							if (ta >= 3.14159f * 0.75f || ta < -3.14159f * 0.75f)
								sx = tpy - (float)tey;
						}
					}
				}
				int c = (float)(sh() / 2.0) - sh() / ((float)dtw);//ceiling
				int f = sh() - c;//floor
				db[x] = dtw;
				for (int y = 0; y < sh(); y++) {
					if (y <= c) {
						Draw(x, y, L' ');
					}
					else if (y > c && y <= f) {
						if (dtw < depth)
						{
							float sy = ((float)y - (float)c) / ((float)f - (float)c);//sample y
							Draw(x, y, wall->Sample(sx, sy));
						}
						else {
							Draw(x, y, olc::BLACK);
						}
					}
					else {
						Draw(x, y, olc::DARK_GREEN);
					}
				}
			}
			for (auto &object : lo) {
				object.x += object.vx * fet;
				object.y += object.vy * fet;
				if (map.c_str()[(int)object.x*mw + (int)object.y] == '#') {
					object.remove = true;
				}
				float vx = object.x - plx;
				float vy = object.y - ply;
				float dfp = sqrtf(vx*vx + vy * vy);//distance from player
				float ex = sinf(pla);//eye x
				float ey = cosf(pla);//eye y
				float oa = atan2f(ey, ex) - atan2f(vy, vx);//object angle
				if (oa < -3.14159f) {
					oa += 2.0f * 3.14159f;
				}
				if (oa > 3.14159f) {
					oa -= 2.0f*3.14159f;
				}
				bool ipfov = fabs(oa) < fov / 2.0f;//in player fov
				if (ipfov && dfp >= 0.5f && dfp < depth) {
					float oc = (float)(sh() / 2.0) - sh() / ((float)dfp);//object ceiling
					float of = sh() - oc;//object floor
					float oh = of - oc;//object height
					float oar = (float)object.sprite->height / (float)object.sprite->width;//object aspect ratio
					float ow = oh / oar;//object width
					float moo = (0.5f*(oa / (fov / 2.0f)) + 0.5f)*(float)sw();//middle of object
					for (float lx = 0; lx < ow; lx++){
						for (float ly = 0; ly < oh; ly++){
						float sx = lx / ow;//sample x
						float sy = ly / oh;//sample y
						olc::Pixel c = object.sprite->Sample(sx, sy);
							int oco = (int)(moo + lx - (ow / 2.0f));//object column
							if (oco >= 0 && oco < sw()) {
								if (c != olc::BLACK && db[oco]>=dfp) {
									Draw(oco, oc + ly, c);
									db[oco] = dfp;
								}
							}
						}
					}
				}
			}
			lo.remove_if([](object &o) {return o.remove; });
			int ms = 2;//map scale
			for (int nx = 0; nx < mw; nx++) {
				for (int ny = 0; ny < mw; ny++) {
					FillRect((nx + 1)*ms, (ny + 1)*ms, ms, ms, map[ny*mw + nx] == '#' ? olc::WHITE : olc::VERY_DARK_GREY);
				}
			}
			FillRect((1 + (int)ply)*ms, (1 + (int)plx) * ms, ms, ms, olc::GREEN);
			return true;
	}
};
int main() {
	ASCIIFPS game;
	if(game.Construct(650, 480, 2, 2))
	game.Start();
	return 0;
}