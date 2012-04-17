#include "MMBNBattleAttack.h"

using namespace std;

//////////////////////////////////
// ATTACK INFO
//////////////////////////////////
AttackInfo::AttackInfo()
{

}

AttackInfo::AttackInfo(string file)
{
	#ifdef _DEBUG
		LOG("Init attack info from file " + file)
	#endif
	
	ifstream in_info( file.c_str() , ifstream::in);

	if(!in_info.good())
	{
		LOG("Impossible de trouver le fichier " + file)
		oslQuit();
	}
	
	string line;
	while(getline(in_info, line))
	{
		//target type
		if(line.find("target_type") == 0)
		{	
			vector<string> v = StringUtils::Split(line, " \r\n");
			this->target_type = GameSystem::GetTargetOfString(v.at(1));
		}
		//range
		else if(line.find("range") == 0)
		{	
			this->range.clear();
			vector<string> v = StringUtils::Split(line, " \r\n");
			for(unsigned int i = 1 ; i < v.size() ; ++i)
			{
				vector<string> r = StringUtils::Split(v[i], ",");
				istringstream iss(r.at(0) + " " + r.at(1));
				int x_range;
				int y_range;
				iss >> x_range;
				iss >> y_range;
				this->range.push_back(Vector2i(x_range,y_range));
			}
			
		}
		//target teams
		else if(line.find("target_teams") == 0)
		{	
			vector<string> v = StringUtils::Split(line, " \r\n");
			for(unsigned int i = 1 ; i < v.size() ; ++i)
				this->target_teams.push_back(GameSystem::GetPanelTeamOfString(v.at(i)));
			
		}
		//piercing attack
		else if(line.find("pierce") == 0)
		{
			vector<string> v = StringUtils::Split(line, " \r\n");
			if(v.at(1).compare("true") == 0)
				this->pierce_attack = true;
			else
				this->pierce_attack = false;
		}
		//stagger enemy
		else if(line.find("stagger") == 0)
		{
			vector<string> v = StringUtils::Split(line, " \r\n");
			if(v.at(1).compare("true") == 0)
				this->stagger_enemy = true;
			else
				this->stagger_enemy = false;
		}
		//frames
		else if(line.find("hit_frames") == 0)
		{
			vector<string> v = StringUtils::Split(line, " \r\n");
			for(unsigned int i = 1 ; i < v.size() ; ++i)
			{
				istringstream iss(v.at(i));
				unsigned int frame;
				iss >> frame;
				this->hit_frames.push_back(frame);
			}
		}
	}

	in_info.close();
	
	#ifdef _DEBUG
		ostringstream oss(ostringstream::out);
		oss << "target_type " << this->target_type << "\r\n";
		
		for(unsigned int i = 0 ; i < this->range.size() ; ++i)
			oss << "range " << this->range[i].x << " " << this->range[i].y << "\r\n";
			
		for(unsigned int i = 0 ; i < this->target_teams.size() ; ++i)
			oss << "target_teams " << this->target_teams[i] << "\r\n";
			
		oss << "pierce_attack " << this->pierce_attack << "\r\n";
		oss << "stagger_enemy " << this->stagger_enemy << "\r\n";
		
		for(unsigned int i = 0 ; i < this->hit_frames.size() ; ++i)
			oss << "hit_frame " << this->hit_frames[i] << "\r\n";
			
		LOG(oss.str())
		LOG("Attack info loaded")
	#endif
}

AttackInfo::~AttackInfo()
{

}

void AttackInfo::SetTargetType(GameSystem::TargetType target)
{
	target_type = target;
}

void AttackInfo::SetRange(vector<Vector2i> range)
{
	this->range.clear();
	for(unsigned int i = 0 ; i < range.size() ; ++i)
		this->range.push_back(range[i]);
}

void AttackInfo::SetTeams(vector<GameSystem::PanelTeam> teams)
{
	this->target_teams.clear();
	for(unsigned int i = 0 ; i < teams.size() ; ++i)
		this->target_teams.push_back(teams[i]);
}

void AttackInfo::SetPierceAttack(bool pierce)
{
	pierce_attack = pierce;
}

void AttackInfo::SetStaggerEnemy(bool stagger)
{
	stagger_enemy = stagger;
}

void AttackInfo::SetHitFrames(vector<int> frames)
{
	this->hit_frames.clear();
	for(unsigned int i = 0 ; i < frames.size() ; ++i)
		this->hit_frames.push_back(frames[i]);
}

GameSystem::TargetType AttackInfo::GetTargetType() const
{
	return target_type;
}

vector<Vector2i> AttackInfo::GetRange() const
{
	return range;
}

vector<GameSystem::PanelTeam> AttackInfo::GetTeams() const
{
	return target_teams;
}

bool AttackInfo::IsPierceAttack()
{
	return pierce_attack;
}

bool AttackInfo::IsStaggerAttack()
{
	return stagger_enemy;
}

vector<int>& AttackInfo::GetHitFrames()
{
	return hit_frames;
}

string AttackInfo::ToString()
{
	ostringstream oss(ostringstream::out);
	oss << "target_type " << this->target_type << "\r\n";
	
	for(unsigned int i = 0 ; i < this->range.size() ; ++i)
		oss << "range " << this->range[i].x << " " << this->range[i].y << "\r\n";
		
	for(unsigned int i = 0 ; i < this->target_teams.size() ; ++i)
		oss << "target_teams " << this->target_teams[i] << "\r\n";
		
	oss << "pierce_attack " << this->pierce_attack << "\r\n";
	oss << "stagger_enemy " << this->stagger_enemy << "\r\n";
	
	for(unsigned int i = 0 ; i < this->hit_frames.size() ; ++i)
		oss << "hit_frame " << this->hit_frames[i] << "\r\n";
		
	return oss.str();
}




////////////////////////////////////////////////
// BATTLE PROJECTILE
////////////////////////////////////////////////
BattleProjectile::BattleProjectile()
{

}

BattleProjectile::~BattleProjectile()
{

}




///////////////////////////////////////////////
// BATTLE ATTACK
///////////////////////////////////////////////
MMBNBattleAttack::MMBNBattleAttack()
{

}

MMBNBattleAttack::MMBNBattleAttack(string file, string attack_info_file)
{
	#ifdef _DEBUG
		LOG("Init battle attack from file " + file)
	#endif
	
	ifstream in_info( file.c_str() , ifstream::in);

	if(!in_info.good())
	{
		LOG("Impossible de trouver le fichier " + file);
		oslQuit();
	}

	string line;
	while(getline(in_info, line))
	{
		//name
		if(line.find("name") == 0)
		{	
			vector<string> v = StringUtils::Split(line, " \r\n");
			this->name = v.at(1);
			
		}
		//power
		if(line.find("power") == 0)
		{	
			vector<string> v = StringUtils::Split(line, " \r\n");
			istringstream iss(v.at(1));
			iss >> this->power;
			
		}
		
		//use projectile
		if(line.find("use_projectile") == 0)
		{
			vector<string> v = StringUtils::Split(line, " \r\n");
			if(v.at(1).compare("true") == 0)
				this->use_projectile = true;
			else
				this->use_projectile = false;
		}
		
		//projectiles number
		if(line.find("projectiles_number") == 0)
		{	
			vector<string> v = StringUtils::Split(line, " \r\n");
			istringstream iss(v.at(1));
			iss >> this->projectiles_number;
			
		}
		//animation name
		if(line.find("animation") == 0)
		{	
			vector<string> v = StringUtils::Split(line, " \r\n");
			//ba->actor_animation_name = string("Battle/Animations/Attacks/") + v.at(1);
			AnimationPtr ap(Animation::Load( string("Battle/Animations/Attacks/") + v.at(1), false, false ));
			this->actor_animation.swap(ap);
		}
	}

	in_info.close();
	
	#ifdef _DEBUG
		ostringstream oss(ostringstream::out);
		oss << "name " << this->name << "\r\n";
		oss << "power " << this->power << "\r\n";
		oss << "use_projectile " << this->use_projectile << "\r\n";
		oss << "projectiles_number " << this->projectiles_number << "\r\n";
		LOG(oss.str())
		LOG("Battle Attack loaded")
	#endif
	
	AttackInfoPtr aip(new AttackInfo(attack_info_file));
	this->attack_info.swap(aip);
}

MMBNBattleAttack::~MMBNBattleAttack()
{

}


string MMBNBattleAttack::GetName()
{
	return name;
}

unsigned int MMBNBattleAttack::GetPower()
{
	return power;
}

AttackInfoPtr& MMBNBattleAttack::GetAttackInfo()
{
	return attack_info;
}

bool MMBNBattleAttack::UseProjectile()
{
	return use_projectile;
}

unsigned int MMBNBattleAttack::GetProjectilesNumber()
{
	return projectiles_number;
}

map<unsigned int, vector<BattleProjectile> > MMBNBattleAttack::GetProjectilesMap() const
{
	return projectiles_map;
}

AnimationPtr MMBNBattleAttack::GetAnimation()
{
	return actor_animation;
}

string MMBNBattleAttack::ToString()
{
	ostringstream oss(ostringstream::out);
	oss << "name " << this->name << "\r\n";
	oss << "power " << this->power << "\r\n";
	oss << "use_projectile " << this->use_projectile << "\r\n";
	oss << "projectiles_number " << this->projectiles_number << "\r\n";
	
	return oss.str() + attack_info->ToString();
}
