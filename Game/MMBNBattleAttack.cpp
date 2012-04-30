#include "MMBNBattleAttack.h"

#include "MMBNBattleActor.h"

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

const vector<Vector2i>& AttackInfo::GetRange() const
{
	return range;
}

const vector<GameSystem::PanelTeam>& AttackInfo::GetTeams() const
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

const vector<int>& AttackInfo::GetHitFrames() const
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
	name = "Unknown Projectile";
	velocity = 0;
	moving_type = GameSystem::NONE_PROJECTILE_MOVING_TYPE;
	damage = 0;
	owner = NULL;
}

BattleProjectile::BattleProjectile(const string& name, const string& anim, bool reverse, unsigned int velocity, GameSystem::ProjectileMovingType mt, const vector<unsigned int>& hit_frames, unsigned int trigger, unsigned int damage, MMBNBattleActor* owner)
{
	this->name = name;
	this->animation = Animation::Load(string("Battle/Animations/Projectiles/") + anim, reverse, true);
	this->reverse = reverse;
	this->velocity = velocity;
	this->moving_type = mt;
	for(unsigned int i = 0 ; i < hit_frames.size() ; i++)
		this->hitting_frames.push_back(hit_frames[i]);
		
	this->damage = damage;
	this->trigger = trigger;
	this->owner = owner;
	
}

BattleProjectile::~BattleProjectile()
{

}

BattleProjectilePtr BattleProjectile::Load(unsigned int proj_nb, const string& file, bool reverse, MMBNBattleActor* owner)
{
	#ifdef _DEBUG
		ostringstream _debug;
		_debug << "Init projectile from file " << file << " : proj nb is " << proj_nb;
		LOG(_debug.str())
	#endif
	
	ifstream in_info( file.c_str() , ifstream::in);

	if(!in_info.good())
	{
		LOG("Impossible de trouver le fichier " + file);
		oslQuit();
	}

	bool def_found = false;
	
	//proj info
	string name;
	string anim;
	unsigned int velocity;
	GameSystem::ProjectileMovingType mt;
	std::vector<unsigned int> hit_frames;
	unsigned int damage;
	unsigned int trigger;
	
	string line;
	while(getline(in_info, line))
	{
		//look for proj def
		if(!def_found)
		{
			ostringstream oss;
			oss << "PROJ " << proj_nb;
			if(line.find(oss.str()) == 0)
				def_found = true;
		}
		//read current proj attributes
		else
		{
			//if end of proj declaration, go out
			if(line.find("ENDPROJ") == 0) break;
			
			if(line.find("name") == 0)
			{
				vector<string> v = StringUtils::Split(line, " \r\n");
				name = v.at(1);
			}
			else if(line.find("anim") == 0)
			{
				vector<string> v = StringUtils::Split(line, " \r\n");
				anim = v.at(1);
				//anim = Animation::Load("Battle/Animations/Projectiles/" + v.at(1), reverse, true);
			}
			else if(line.find("moving") == 0)
			{
				vector<string> v = StringUtils::Split(line, " \r\n");
				mt = GameSystem::STRAIGHT_PROJECTILE_MOVING_TYPE; //a changer ici
			}
			else if(line.find("damage") == 0)
			{
				vector<string> v = StringUtils::Split(line, " \r\n");
				istringstream iss(v.at(1));
				iss >> damage;
			}
			else if(line.find("velocity") == 0)
			{
				vector<string> v = StringUtils::Split(line, " \r\n");
				istringstream iss(v.at(1));
				iss >> velocity;
			}
			else if(line.find("trigger") == 0)
			{
				vector<string> v = StringUtils::Split(line, " \r\n");
				istringstream iss(v.at(1));
				iss >> trigger;
			}
			else if(line.find("hit") == 0)
			{
				vector<string> v = StringUtils::Split(line, " \r\n");
				for(unsigned int i = 1 ; i < v.size() ; i++)
				{
					istringstream iss(v.at(i));
					unsigned int hit;
					iss >> hit;
					hit_frames.push_back(hit);
				}
			}
		}
		
	}

	in_info.close();
	
	BattleProjectilePtr bpp(new BattleProjectile(name, anim, reverse, velocity, mt, hit_frames, trigger, damage, owner));
	
	#ifdef _DEBUG
		ostringstream oss(ostringstream::out);
		oss << "name " << bpp->name << "\r\n";
		oss << "reverse " << bpp->reverse << "\r\n";
		oss << "velocity " << bpp->velocity << "\r\n";
		oss << "damage " << bpp->damage << "\r\n";
		oss << "trigger " << bpp->trigger << "\r\n";
		oss << "moving_type " << bpp->moving_type << "\r\n";
		oss << "hit_frames ";
		for(unsigned int i = 0 ; i < bpp->hitting_frames.size() ; i ++)
			oss << bpp->hitting_frames.at(i) << " ";
		oss << "\r\n";
		LOG(oss.str())
		LOG("Battle projectile loaded")
	#endif
	
	
	return bpp;
}

const string& BattleProjectile::GetName() const
{
	return name;
}

AnimationPtr BattleProjectile::GetAnimation() const
{
	return animation;
}

const unsigned int BattleProjectile::GetVelocity() const
{
	return velocity;
}

const GameSystem::ProjectileMovingType& BattleProjectile::GetMovingType() const
{
	return moving_type;
}

const vector<unsigned int> BattleProjectile::GetHitFrames() const
{
	return hitting_frames;
}

const unsigned int BattleProjectile::GetDamage() const
{
	return damage;
}

const unsigned int BattleProjectile::GetTrigger() const
{
	return trigger;
}

const MMBNBattleActor* BattleProjectile::GetOwner() const
{
	return owner;
}


void BattleProjectile::Display(float offX, float offY)
{
	animation->Update();
	animation->Display(offX, offY);
}

void BattleProjectile::Move(float x, float y)
{
	animation->SetPosition(animation->GetPosition().x + x, animation->GetPosition().y + y);
}

void BattleProjectile::SetPosition(float x, float y)
{
	animation->SetPosition(x, y);
}

const Vector2f& BattleProjectile::GetPosition() const
{
	return animation->GetPosition();
}
		
		
///////////////////////////////////////////////
// BATTLE ATTACK
///////////////////////////////////////////////
MMBNBattleAttack::MMBNBattleAttack()
{

}

MMBNBattleAttack::MMBNBattleAttack(const string& path, const string& file, bool reverse)
{
	#ifdef _DEBUG
		LOG("Init battle attack from file " + path + file)
	#endif
	
	ifstream in_info( (path + file).c_str() , ifstream::in);

	if(!in_info.good())
	{
		LOG("Impossible de trouver le fichier " + (path + file));
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
	
	AttackInfoPtr aip(new AttackInfo(path + "attack_info.txt"));
	this->attack_info.swap(aip);
	
	//load projectiles if needed
	if(this->use_projectile)
	{
		#ifdef _DEBUG
			LOG("Load projectiles")
		#endif
		for(unsigned int i = 0 ; i < this->projectiles_number ; ++i)
		{
			pair<unsigned int, BattleProjectilePtr> p;
			BattleProjectilePtr bpp = BattleProjectile::Load(i + 1, path + "projectiles.txt", reverse, NULL);
			this->projectiles_map[bpp->GetTrigger()].push_back(bpp);
		}
		#ifdef _DEBUG
			LOG("Projectiles loaded")
		#endif
	}
}

MMBNBattleAttack::~MMBNBattleAttack()
{

}


const string MMBNBattleAttack::GetName() const
{
	return name;
}

const unsigned int MMBNBattleAttack::GetPower() const
{
	return power;
}

const AttackInfoPtr& MMBNBattleAttack::GetAttackInfo() const
{
	return attack_info;
}

const bool MMBNBattleAttack::UseProjectile() const
{
	return use_projectile;
}

const unsigned int MMBNBattleAttack::GetProjectilesNumber() const
{
	return projectiles_number;
}

const map<unsigned int, vector<BattleProjectilePtr> > MMBNBattleAttack::GetProjectilesMap() const
{
	return projectiles_map;
}

AnimationPtr MMBNBattleAttack::GetAnimation() const
{
	return actor_animation;
}

const string MMBNBattleAttack::ToString() const
{
	ostringstream oss(ostringstream::out);
	oss << "name " << this->name << "\r\n";
	oss << "power " << this->power << "\r\n";
	oss << "use_projectile " << this->use_projectile << "\r\n";
	oss << "projectiles_number " << this->projectiles_number << "\r\n";
	
	return oss.str() + attack_info->ToString();
}
