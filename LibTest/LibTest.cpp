
#ifndef _DEBUG
#pragma comment(lib, "MyNetworkLib\\Release\\MyNetworkLib.lib")

#else
#pragma comment(lib, "MyNetworkLib\\Debug\\MyNetworkLib.lib")

#endif // !_DEBUG

#include "CorePch.h"

#include <iostream>
#include <cmath>

#include "flatbuffers/flatbuffers.h"
#include "flatbuffers/util.h"

#include "fbs/Test_generated.h"
#include "fbs/Test2_generated.h"


using namespace std;

using fbb = flatbuffers::FlatBufferBuilder;

static string ColorToString(Test::Color color)
{
	switch (color)
	{
	case Test::Color_Red: return "Red";
	case Test::Color_Green: return "Green";
	case Test::Color_Blue: return "Blue";
	default: return "None";
	}
}

int main()
{
	// flat buffer test
	{
		fbb builder;

		std::string str = "Slime";
		auto name_offset = builder.CreateString(str);

		Test::Vec3 pos(1.0f, 2.0f, 3.0f);
		auto drops = builder.CreateVector(std::vector<uint8_t>({ 1, 2, 3 }));
		auto o_creature = Test::CreateCreature(builder, false, &pos, 1500, name_offset, drops, Test::Color_Blue);
		builder.Finish(o_creature);

		auto p_monster = builder.GetBufferPointer();
		auto p_monster_size = builder.GetSize();

		const Test::Creature *monster = Test::GetCreature(p_monster);

		auto _name = monster->name()->c_str();
		auto _pos = monster->pos();
		auto _hp = monster->hp();
		auto _friendly = monster->friendly();
		auto _color = monster->color();
		auto _drops = monster->drops();

		cout << "Monster Name: " << _name << endl;
		cout << "Monster Position: (" << _pos->x() << ", "
			<< _pos->y() << ", " << _pos->z() << ")" << endl;
		cout << "Monster Hp: " << monster->hp() << endl;
		cout << "Friendly? : " << monster->friendly() << endl;
		cout << "Monster Color: " << ColorToString(monster->color()) << endl;
		for (int i = 0; i < _drops->size(); ++i)
		{
			cout << (int)(_drops->Get(i)) << endl;
		}
	}

	{
		{
			fbb builder;

			Test::Vec3 pos(1.0f, 1.0f, 2.0f);
			auto magician_name = builder.CreateString("Magician");
			auto o_magician = Test::CreateMagician(builder, 10);
			auto o_player = Test::CreatePlayer(builder, magician_name, &pos, 
				Test::Jobs_Magician, o_magician.Union());
			builder.Finish(o_player);

			auto p_magician = builder.GetBufferPointer();
			auto p_magician_size = builder.GetSize();

			const Test::Player* player = Test::GetPlayer(p_magician);
			auto magician = player->job_as_Magician();

			auto _name = player->name()->c_str();
			auto _pos = player->pos();
			auto _magic = magician->magic();

			cout << "Player name: " << _name << endl;
			cout << "Player Position: (" << _pos->x() << ", "
				<< _pos->y() << ", " << _pos->z() << ")" << endl;
			cout << "Player magic: " << _magic << endl;
		}

	}

	return 0;
}
