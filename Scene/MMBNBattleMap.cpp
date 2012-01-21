#include "MMBNBattleMap.h"
#include "GameBattle.h"

using namespace std;

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// MMBNPANELGRID
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
MMBNPanelGrid::MMBNPanelGrid()
{
	#ifdef _DEBUG
		LOG("Create PanelGrid")
	#endif
	
	m_width = 10;
	m_height = 5;
	m_x_inc = 40;
	m_y_inc = 24;

	m_x_map = m_x_inc;
	m_y_map = (272 / 3) + m_y_inc;

	boost::array<int, 2> shape = {{ m_width, m_height }};

	PanelsPtr pp( new Panels(shape) );
	m_panels.swap(pp);

	PanelsTeamPtr ptp( new PanelsTeam(shape) );
	m_panels_team.swap(ptp);

	// Initialize the panels
	// Each team has the half of the panels
	for(unsigned int i = 0 ; i < (m_width / 2) ; i++)
		for(unsigned int j = 0 ; j < m_height ; j++)
			(*m_panels_team)[i][j] = GameSystem::PLAYER;
		
	for(unsigned int i = (m_width / 2) ; i < m_width ; i++)
		for(unsigned int j = 0 ; j < m_height ; j++)
			(*m_panels_team)[i][j] = GameSystem::ENEMY;
		
	//all the panels are normal
	for(unsigned int i = 0 ; i < m_width ; i++)
		for(unsigned int j = 0 ; j < m_height ; j++)
			(*m_panels)[i][j] = GameSystem::NORMAL;
			
	//load the pictures for each team
	m_panel_pictures[GameSystem::PLAYER] = ImgManager::GetImage("System/Animation/Battle/Panels/player_panel.png");
	m_panel_pictures[GameSystem::ENEMY]  = ImgManager::GetImage("System/Animation/Battle/Panels/enemy_panel.png");
	
	//load the animations for each type of panels
	m_panel_animations[GameSystem::NORMAL] 	= Animation::Load("System/Animation/Battle/Panels/Normal", false, true);
	/*m_panel_animations[EMPTY] 	= NULL;
	m_panel_animations[BROKEN] 	= NULL;
	m_panel_animations[CRACKED] = NULL;
	m_panel_animations[FIRE] 	= NULL;
	m_panel_animations[ICE] 	= NULL;
	m_panel_animations[GRASS] 	= NULL;
	m_panel_animations[POISON] 	= NULL;
	m_panel_animations[WATER] 	= NULL;*/
	
	//m_attack_impact = Animation::Load("System/Animation/Battle/AttackImpact", false, false);
	Animation::Load("System/Animation/Battle/AttackImpact", false, false); //just to load it instead of inside the render loop
	
	//actor and enemies
	GameBattle::BattleInfo battle_info = GameBattle::GetBattleInfo();
	
	#ifdef _DEBUG
	LOG("Load battle actor")
	#endif
	m_actor = MMBNBattleActor::Load(battle_info.actor.name, false, true, false);
	PutActorOnPanel(m_actor, battle_info.actor.position.x, battle_info.actor.position.y);
	#ifdef _DEBUG
	LOG("Battle actor loaded")
	LOG("Load battle enemies")
	#endif
	
	vector<GameBattle::BattleCharacter>::iterator it;
	for(it = battle_info.enemies.begin() ; it != battle_info.enemies.end() ; ++it)
	{
		MMBNBattleActor* a = MMBNBattleActor::Load(it->name, true, false, true);
		a->SetDirection(MMBNBattleActor::LEFT);
		a->SetState(MMBNBattleActor::BATTLE_STANDING);
		m_enemies.push_back( a );
		PutActorOnPanel(m_enemies.back(), it->position.x, it->position.y);
		m_ia.push_back(new MMBNBattleIA(this, m_enemies.back(), GameSystem::ENEMY));
	}
	
	#ifdef _DEBUG
	LOG("Battle enemies loaded")
	#endif
	
	m_actor_is_dead = false;
	m_can_attack = false;
	
	m_display_attack_impact = false;
	
}

MMBNPanelGrid::~MMBNPanelGrid()
{
	#ifdef _DEBUG
		LOG("Destroy PanelGrid")
	#endif
	
	//actor
	if(m_actor) delete m_actor;

	//enemies
	vector<MMBNBattleActor*>::iterator it;
	for(it = m_enemies.begin() ; it != m_enemies.end() ; it++)
		delete (*it);

	//panel animations
	//for(unsigned int i = 0 ; i < PANEL_TYPES_NB ; i++)
	//	if(m_panel_animations[i]) delete m_panel_animations[i];
			
	//ia
	vector<MMBNBattleIA*>::iterator it_ia;
	for(it_ia = m_ia.begin() ; it_ia != m_ia.end() ; it_ia++)
		if(*it_ia) delete (*it_ia);
		
}

void MMBNPanelGrid::Display()
{

	//======================================
	// PANELS
	//======================================
	
	for(unsigned int i = 0 ; i < m_width ; i++)
		for(unsigned int j = 0 ; j < m_height ; j++)
		{
			int x = (i * m_x_inc) + m_x_map;
			int y = (j * m_y_inc) + m_y_map;

			//--------------------------------------
			// TEAM
			//--------------------------------------
			if( m_panel_pictures[(*m_panels_team)[i][j]] )
				oslDrawImageXY( m_panel_pictures[(*m_panels_team)[i][j]], x, y );

			if( m_panel_animations[(*m_panels)[i][j]] )
			{
				m_panel_animations[(*m_panels)[i][j]]->Update();
				m_panel_animations[(*m_panels)[i][j]]->SetPosition(x, y);
				m_panel_animations[(*m_panels)[i][j]]->Display();
			}
			
			
		}


	//======================================
	// ACTORS
	//======================================
	vector<MMBNBattleActor*>::iterator it;
	
	for(unsigned int i = 0 ; i < m_width ; i++)
		for(unsigned int j = 0 ; j < m_height ; j++)
		{
			Vector2i v = GetActorPanel(m_actor);
			if(v.x == i && v.y == j) m_actor->Display();
			
			for(it = m_enemies.begin() ; it != m_enemies.end() ; it++)
			{
				v = GetActorPanel(*it);
				if(v.x == i && v.y == j) (*it)->Display();
			}
		}
		
	//display life of enemies
	for(it = m_enemies.begin() ; it != m_enemies.end() ; it++)
	{
	
		//if enemy dead, don't display life
		if((*it)->IsDead()) continue;
		
		MMBNString life_s;
		life_s.SetFont(GameSystem::GetEnemyLifeFont());

		ostringstream life_oss(ostringstream::out);
		life_oss << (*it)->GetHP();
		life_s = life_oss.str();

		int w = life_s.GetStringWidth();

		life_s.SetPosition( (*it)->GetPosition().x - (w / 2), (*it)->GetPosition().y );
		life_s.Display();
	}
	
	//attack impact
	/*if(m_display_attack_impact)
	{
		m_attack_impact->Update();
		m_attack_impact->Display();
		
		if(m_attack_impact->IsOver())
		{
			m_attack_impact->Stop();
			m_display_attack_impact = false;
		}
	}*/
	
	for(unsigned int i = 0 ; i < m_attack_impact.size() ; ++i)
	{
		m_attack_impact[i]->Update();
		m_attack_impact[i]->Display();
	}
		
}

void MMBNPanelGrid::PutActorOnPanel(MMBNBattleActor* actor, unsigned int x, unsigned int y)
{
	actor->SetPosition( (x * m_x_inc) + m_x_map + m_x_inc / 2, (y * m_y_inc) + m_y_map + m_y_inc / 2);
}

Vector2i MMBNPanelGrid::GetActorPanel(MMBNBattleActor* actor)
{
	Vector2i v;
	v.x = (actor->GetPosition().x - m_x_map) / m_x_inc;
	v.y = (actor->GetPosition().y - m_y_map) / m_y_inc;
	return v;
}

void MMBNPanelGrid::MoveActor(MMBNBattleActor* actor, int offX, int offY)
{
	PutActorOnPanel(actor, GetActorPanel(actor).x + offX, GetActorPanel(actor).y + offY);
}

inline GameSystem::PanelType MMBNPanelGrid::GetPanelType(unsigned int x, unsigned int y)
{
	return (*m_panels)[x][y];
}

inline GameSystem::PanelTeam MMBNPanelGrid::GetPanelTeam(unsigned int x, unsigned int y)
{
	return (*m_panels_team)[x][y];
}

inline bool MMBNPanelGrid::IsInsideGrid(int x, int y)
{
	if( (x >= 0) && (x < m_width) && (y >= 0) && (y < m_height) ) return true;

	return false;
}

bool MMBNPanelGrid::IsWalkable(unsigned int x, unsigned int y)
{
	bool walkable = true;
	
	if(!IsInsideGrid(x,y)) return false;
	
	Vector2i v = GetActorPanel(m_actor);
	
	if( (v.x == x) && (v.y == y) ) return false;
	
	vector<MMBNBattleActor*>::iterator it;
	for(it = m_enemies.begin() ; walkable && it != m_enemies.end() ; it++)
	{
		v = GetActorPanel(*it);
		if( (v.x == x) && (v.y == y) ) walkable = false;
		
	}
	
	return walkable;
	
}

inline MMBNBattleActor* MMBNPanelGrid::GetActor()
{
	return m_actor;
}

inline vector<MMBNBattleActor*>& MMBNPanelGrid::GetEnemies()
{
	return m_enemies;
}

void MMBNPanelGrid::ActorHandle(OSL_CONTROLLER* k)
{

	Vector2i v = GetActorPanel(m_actor);

	//MOVING
	if(m_actor->GetState() == MMBNBattleActor::BATTLE_STANDING) //move only if in standing phase
	{
		if(k->pressed.left)
			v.x = v.x - 1;
		else if(k->pressed.right)
			v.x = v.x + 1;
		else if(k->pressed.up)
			v.y = v.y - 1;
		else if(k->pressed.down)
			v.y = v.y + 1;
		
		if( IsInsideGrid(v.x, v.y) && ( GetPanelTeam(v.x, v.y) == GameSystem::PLAYER ) )
			PutActorOnPanel(m_actor, v.x, v.y);
	}
	
	//ATTACKING
	if( k->pressed.cross )
	{
		//play attack anim
		if(m_actor->GetState() == MMBNBattleActor::BATTLE_STANDING) //attack only if in standing phase
		{
			m_actor->SetState(MMBNBattleActor::BATTLE_ATTACK);
			m_can_attack = true;
		}
		
	}
	
	if( m_can_attack && (m_actor->GetCurrentAnim()->GetCurrentFrame() == m_actor->GetInfo()->attack_frame) )
	{
		AttackEnemies();
		m_can_attack = false;
	}
	
	
}

void MMBNPanelGrid::Update(OSL_CONTROLLER* k)
{
	
	//state management
	
	//actor
	{
		//dead ?
		if(m_actor->IsDead()) m_actor->SetState(MMBNBattleActor::BATTLE_DEAD);
		
		//battle over ?
		if(BattleIsOver()) m_actor->SetState(MMBNBattleActor::BATTLE_WIN);
		
		//else
		switch(m_actor->GetState())
		{
			case MMBNBattleActor::BATTLE_ATTACK:
			case MMBNBattleActor::BATTLE_DAMAGED:
				if(m_actor->GetCurrentAnim()->IsOver()) m_actor->SetState(MMBNBattleActor::BATTLE_STANDING);
				break;
				
			case MMBNBattleActor::BATTLE_DEAD:
				if(m_actor->GetCurrentAnim()->IsOver()) m_actor_is_dead = true;
				break;
				
			case MMBNBattleActor::BATTLE_STANDING:
			case MMBNBattleActor::BATTLE_WIN:
			default:
				break;
			
		}
	}
	
	//---enemies---
	for(unsigned int i(0) ; i < m_enemies.size() ; i++)
	{
		//dead ?
		if(m_enemies[i]->IsDead()) m_enemies[i]->SetState(MMBNBattleActor::BATTLE_DEAD);
		
		switch(m_enemies[i]->GetState())
		{
			case MMBNBattleActor::BATTLE_ATTACK:
			case MMBNBattleActor::BATTLE_DAMAGED:
				if(m_enemies[i]->GetCurrentAnim()->IsOver()) m_enemies[i]->SetState(MMBNBattleActor::BATTLE_STANDING);
				break;
				
			case MMBNBattleActor::BATTLE_DEAD:
				if(m_enemies[i]->GetCurrentAnim()->IsOver())
				{
					m_enemies.erase(m_enemies.begin() + i);
					m_ia.erase(m_ia.begin() + i);
					i--;
				}
				break;
				
			case MMBNBattleActor::BATTLE_STANDING:
			default:
				break;
			
		}
	}
	
	//impact anims
	for(unsigned int i = 0 ; i < m_attack_impact.size() ; ++i)
	{
		if(m_attack_impact[i]->IsOver())
		{
			m_attack_impact.erase(m_attack_impact.begin() + i);
			i--;
		}
	}
	
	if(BattleIsOver()) return;
	
	//Update IA
	vector<MMBNBattleIA*>::iterator it;
	
	for(it = m_ia.begin() ; it != m_ia.end() ; it++)
		(*it)->Update();
	
	//actor
	ActorHandle(k);
}

inline bool MMBNPanelGrid::BattleIsOver()
{
	return m_enemies.size() == 0;
}

inline bool MMBNPanelGrid::ActorIsDead()
{
	return m_actor_is_dead;
}

inline unsigned int MMBNPanelGrid::GetWidth() const
{
	return m_width;
}

inline unsigned int MMBNPanelGrid::GetHeight() const
{
	return m_height;
}

vector<Vector2i> MMBNPanelGrid::GetTargetedPanels(MMBNBattleActor* launcher, GameSystem::AttackInfo &attack_info)
{
	vector<Vector2i> panels;
	//for each case of the range
	for(unsigned int i = 0 ; i < attack_info.range.size() ; ++i)
	{
		//we look for the teams allowed to be targeted
		for(unsigned int j = 0 ; j < attack_info.target_teams.size() ; j++)
		{
			Vector2i v;
			
			switch(attack_info.target_type)
			{	
				//the range is computed according to the user
				case GameSystem::USER_TARGET:
					{
						v = GetActorPanel(launcher);
						
						if(launcher->GetDirection() == MMBNBattleActor::RIGHT)
						{	
							v.x += attack_info.range[i].x;
							v.y += attack_info.range[i].y;
						}
						else
						{
							v.x -= attack_info.range[i].x;
							v.y -= attack_info.range[i].y;
						}
						
						if(v.x < 0) v.x = 0;
						if(v.y < 0) v.y = 0;
						if(v.x >= m_width) v.x = m_width - 1;
						if(v.y >= m_height) v.y = m_height - 1;
					}
					break;
					
				//the range is computed according to the enemy
				case GameSystem::ENEMY_TARGET:
				//the range is computed according to the user area
				case GameSystem::USER_AREA_TARGET:
				//the range is computed according to the enemy area
				case GameSystem::ENEMY_AREA_TARGET:
				default:
					break;
			}
				
			//if the range is in an allowed panel team
			if((*m_panels_team)[v.x][v.y] == attack_info.target_teams[j])
			{
				//we add the panel into the vector, only if it is not already in
				bool found = false;
				for(unsigned int k = 0 ; !found && k < panels.size() ; ++k)
					if( (panels[k].x == v.x) && (panels[k].y == v.y) )
						found = true;
						
				if(!found)
					panels.push_back(v);
			}
				
		}
	}
	
	return panels;
}

bool MMBNPanelGrid::IsInRange(Vector2i &target, vector<Vector2i> &range)
{
	bool in_range = false;
	
	for(unsigned int i = 0 ; !in_range && (i < range.size()) ; ++i)
		if(range[i].x == target.x && range[i].y == target.y)
			in_range = true;
			
	return in_range;
}

void MMBNPanelGrid::AttackEnemies()
{
	if( m_actor->GetCurrentAnim()->GetCurrentFrame() == m_actor->GetInfo()->attack_frame )
	{
		Vector2i pos;
		
		vector<MMBNBattleActor*>::iterator it;
		for(it = m_enemies.begin() ; it != m_enemies.end() ; it++)
		{
			pos = GetActorPanel(*it);
			vector<Vector2i> range = GetTargetedPanels(m_actor, m_actor->GetInfo()->attack_info);
			
			if(IsInRange(pos, range))
			{
				//play enemy damage anim if enemy is not yet & only if it is a staggering attack
				if( (m_actor->GetInfo()->attack_info.stagger_enemy) && ( (*it)->GetState() != MMBNBattleActor::BATTLE_DAMAGED) )
					(*it)->SetState(MMBNBattleActor::BATTLE_DAMAGED);
					
				//if can attack, attack the enemy
				m_actor->Attack(*it);
				
				AnimationPtr impact = Animation::Load("System/Animation/Battle/AttackImpact", false, false);
				int x_rand = Random::RandomInt(0,m_x_inc / 2);
				int y_rand = Random::RandomInt(0,m_y_inc / 2);
				if(Random::RandomInt(0,2) == 1)
					x_rand = x_rand * -1;
				if(Random::RandomInt(0,2) == 1)
					y_rand = y_rand * -1;
				impact->SetPosition( (pos.x * m_x_inc) + m_x_map + m_x_inc / 2 + x_rand, (pos.y * m_y_inc) + m_y_map + m_y_inc / 2 + y_rand);
				
				m_attack_impact.push_back(impact);
				
			}
		}
		
	}
}

bool MMBNPanelGrid::AttackActor(MMBNBattleActor* launcher)
{
		Vector2i pos = GetActorPanel(m_actor);
		
		vector<Vector2i> range = GetTargetedPanels(launcher, launcher->GetInfo()->attack_info);
		
		if(IsInRange(pos, range))
		{
			//launcher in standing anim
			if(launcher->GetState() == MMBNBattleActor::BATTLE_STANDING)
				launcher->SetState(MMBNBattleActor::BATTLE_ATTACK);
			
			//launcher in attack anim
			if(launcher->GetState() == MMBNBattleActor::BATTLE_ATTACK)
			{
				//if the current attack anim frame is a damaging one
				if( launcher->GetCurrentAnim()->GetCurrentFrame() == launcher->GetInfo()->attack_frame )
				{
					//play actor damage anim if actor is not yet & only if it is a staggering attack
					if( (launcher->GetInfo()->attack_info.stagger_enemy) && (m_actor->GetState() != MMBNBattleActor::BATTLE_DAMAGED) )
						m_actor->SetState(MMBNBattleActor::BATTLE_DAMAGED);
					
					launcher->Attack(m_actor);
					
					AnimationPtr impact = Animation::Load("System/Animation/Battle/AttackImpact", false, false);
					int x_rand = Random::RandomInt(0,m_x_inc / 2);
					int y_rand = Random::RandomInt(0,m_y_inc / 2);
					if(Random::RandomInt(0,2) == 1)
						x_rand = x_rand * -1;
					if(Random::RandomInt(0,2) == 1)
						y_rand = y_rand * -1;
					impact->SetPosition( (pos.x * m_x_inc) + m_x_map + m_x_inc / 2 + x_rand, (pos.y * m_y_inc) + m_y_map + m_y_inc / 2 + y_rand);
					
					m_attack_impact.push_back(impact);
					return true;
				}

			}
		}
		
	return false;
}

		
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// MMBNEmotionDisplay
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

MMBNEmotionDisplay::MMBNEmotionDisplay()
{
	#ifdef _DEBUG
		LOG("Create EmotionDisplay")
	#endif
	
	m_actor = NULL;
	
	for(unsigned int i = 0 ; i < EMOTIONS_NB ; i++)
		m_emotions[i] = NULL;

	m_current_emotion = NORMAL;
}

MMBNEmotionDisplay::MMBNEmotionDisplay(MMBNBattleActor* mmbnba)
{
	#ifdef _DEBUG
		LOG("Create EmotionDisplay")
	#endif
	
	m_actor = mmbnba;
	
	string name = mmbnba->GetName();

	m_emotions[NORMAL] = ImgManager::GetImage("Actors/" + name + "/Emotions/normal.png");

	m_current_emotion = NORMAL;
}

MMBNEmotionDisplay::~MMBNEmotionDisplay()
{
	#ifdef _DEBUG
		LOG("Destroy EmotionDisplay")
	#endif
}

void MMBNEmotionDisplay::Display(float offX, float offY)
{
	oslDrawImageXY(m_emotions[m_current_emotion], m_position.x + offX, m_position.y + offY);
}

void MMBNEmotionDisplay::Move(float x, float y)
{
	SetPosition(m_position.x + x, m_position.y + y);
}

void MMBNEmotionDisplay::SetPosition(float x, float y)
{
	m_position.x = x;
	m_position.y = y;
}

Vector2f& MMBNEmotionDisplay::GetPosition()
{
	return m_position;
}









///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// MMBNLIFEBAR
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
MMBNLifeBar::MMBNLifeBar()
{
	#ifdef _DEBUG
		LOG("Create LifeBar")
	#endif
	
	m_actor = NULL;
	m_life_font = GameSystem::GetActorLifeFont();
}

MMBNLifeBar::MMBNLifeBar(MMBNBattleActor* mmbnba)
{
	#ifdef _DEBUG
		LOG("Create LifeBar")
	#endif
	
	m_actor = mmbnba;
	m_life_font = GameSystem::GetActorLifeFont();
}

MMBNLifeBar::~MMBNLifeBar()
{
	#ifdef _DEBUG
		LOG("Destroy LifeBar")
	#endif
}

void MMBNLifeBar::Display(float offX, float offY)
{
	oslDrawFillRect(m_position.x + offX, m_position.y + offY, m_position.x + m_size.x + offX, m_position.y + m_size.y + offY, RGB(64,64,64));
	oslDrawRect(m_position.x + offX, m_position.y + offY, m_position.x + m_size.x + offX, m_position.y + m_size.y + offY, RGB(255,255,255));

	MMBNString life_s;
	life_s.SetFont(m_life_font);

	ostringstream life_oss(ostringstream::out);
	life_oss << m_actor->GetHP();

	life_s = life_oss.str();

	int w = life_s.GetStringWidth();
	int h = m_life_font->GetCharHeight();

	//life_s.SetPosition( (m_size.x - w) / 2 + m_position.x, (m_size.y - h) / 2 + m_position.y );
	life_s.SetPosition( m_position.x + m_size.x - w - (m_life_font->GetCharWidth() / 2) + offX, (m_size.y - h) / 2 + m_position.y + offY);

	life_s.Display();
}

void MMBNLifeBar::Move(float x, float y)
{
	SetPosition(m_position.x + x, m_position.y + y);
}

void MMBNLifeBar::SetPosition(float x, float y)
{
	m_position.x = x;
	m_position.y = y;
}

Vector2f& MMBNLifeBar::GetPosition()
{
	return m_position;
}

void MMBNLifeBar::SetSize(unsigned int w, unsigned int h)
{
	m_size.x = w;
	m_size.y = h;
}

Vector2f& MMBNLifeBar::GetSize()
{
	return m_size;
}

void MMBNLifeBar::SetActor(MMBNBattleActor* mmbnba)
{
	m_actor = mmbnba;
}

MMBNBattleActor* MMBNLifeBar::GetActor()
{
	return m_actor;
}



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// BATTLE IA
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
MMBNBattleIA::MMBNBattleIA()
{
	#ifdef _DEBUG
		LOG("Create BattleIA")
	#endif
	
	m_map			= NULL	;
	m_actor			= NULL	;
	m_actor_team	= GameSystem::ENEMY	;
	
	m_attack_done 	= false;
}

MMBNBattleIA::MMBNBattleIA(MMBNPanelGrid* m, MMBNBattleActor* a, GameSystem::PanelTeam t)
{
	#ifdef _DEBUG
		LOG("Create BattleIA")
	#endif
	
	m_map			= m	;
	m_actor			= a	;
	m_actor_team	= t	;
	
	m_attack_done 	= false;
}

MMBNBattleIA::~MMBNBattleIA()
{
	#ifdef _DEBUG
		LOG("Destroy BattleIA")
	#endif
}

void MMBNBattleIA::Update()
{
	//-----MOVING-----
	if(!m_moving_timer.is_started())
		m_moving_timer.start();

	MMBNBattleActor::IAConfig* iac = m_actor->GetIAConfig();

	//move actor ?
	if( (m_actor->GetState() == MMBNBattleActor::BATTLE_STANDING) && (m_moving_timer.get_ticks() > iac->moving_time) )
	{
		Move();
		m_moving_timer.stop();
		m_moving_timer.start();
	}
	
	//-----ATTACK-----
	if(!m_attack_timer.is_started())
		m_attack_timer.start();
		
	//attack ?
	if(!m_attack_done && (m_attack_timer.get_ticks() > iac->attack_time) )
	{
		//if(m_actor->GetState() == MMBNBattleActor::BATTLE_STANDING)
		//	m_actor->SetState(MMBNBattleActor::BATTLE_ATTACK);
			
		m_attack_done = Attack();
		if(m_attack_done)
		{
			m_attack_timer.stop();
			m_attack_timer.start();
			m_attack_done = false;
			
			m_moving_timer.stop();
			m_moving_timer.start();
		}
		
	}
}

void MMBNBattleIA::Move()
{
	unsigned int max_x(0), max_y(0), min_x(m_map->GetWidth()-1), min_y(m_map->GetHeight() - 1);
	
	for(unsigned int i(0) ; i < m_map->GetWidth() ; ++i)
		for(unsigned int j(0) ; j < m_map->GetHeight() ; ++j)
		{
			if(m_map->GetPanelTeam(i, j) == m_actor_team)
			{
				if(i > max_x) max_x = i;
				if(i < min_x) min_x = i;
				if(j > max_y) max_y = j;
				if(j < min_y) min_y = j;
			}
		}

	int rand_x(0), rand_y(0);

	do
	{
		rand_x = Random::RandomInt(min_x, max_x);
		rand_y = Random::RandomInt(min_y, max_y);
	}while(!m_map->IsWalkable(rand_x, rand_y));

	m_map->PutActorOnPanel(m_actor, rand_x, rand_y);
}

bool MMBNBattleIA::Attack()
{
	//enemy attack actor
	if(m_actor_team == GameSystem::ENEMY)
	{
		return m_map->AttackActor(m_actor);
	}
	
	return true;
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// CUSTOM JAUGE
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
MMBNCustomGauge::MMBNCustomGauge()
{
	#ifdef _DEBUG
		LOG("Create CustomGauge")
	#endif

	m_empty				= ImgManager::GetImage("System/Animation/Battle/CustomGauge/empty.png")		;
	m_gauge				= ImgManager::GetImage("System/Animation/Battle/CustomGauge/gauge.png")		;
	m_full				= Animation::Load("System/Animation/Battle/CustomGauge/full")				;

	m_time_limit 		= 10000.0f																	;
	m_per		 		= 0.0f																		;

	m_state				= RISING																	;
	
	m_size.x 			= m_empty->sizeX															;
	m_size.y 			= m_empty->sizeY															;
}

MMBNCustomGauge::~MMBNCustomGauge()
{
	#ifdef _DEBUG
		LOG("Destroy CustomGauge")
	#endif
	
	//if(m_full) delete m_full;
}

void MMBNCustomGauge::Display(float offX, float offY)
{
	if(m_state == FULL)
	{
		m_full->Update();
		m_full->Display(offX, offY);
	}
	else
	{
		oslDrawImageXY(m_empty, m_position.x + offX, m_position.y + offY);
		oslSetImageTile(m_gauge, 8, 0, (m_gauge->sizeX - 8) * m_per, m_gauge->sizeY);
		oslDrawImageXY(m_gauge, m_position.x + 8 + offX, m_position.y + offY);
	}
}

void MMBNCustomGauge::Move(float x, float y)
{
	SetPosition(m_position.x + x, m_position.y + y);
}

void MMBNCustomGauge::SetPosition(float x, float y)
{
	m_full->SetPosition(x, y)	;
	m_position.x = x;
	m_position.y = y;
}

Vector2f& MMBNCustomGauge::GetPosition()
{
	return m_position;
}

void MMBNCustomGauge::Update()
{
	if(!m_timer.is_started()) m_timer.start();
	
	m_per = m_timer.get_ticks() / m_time_limit;
	if(m_per > 1.0f) { m_per = 1.0f; m_state = FULL; }
	else m_state = RISING;
	
	if(m_per < 0.0f) m_per = 0.0f;
}

void MMBNCustomGauge::Reset()
{
	if(m_timer.is_paused()) m_timer.unpause();
	
	m_per = 0.0f;
	m_state = RISING;
	m_timer.stop();
	//m_timer.start();
}

void MMBNCustomGauge::Pause()
{
	m_timer.pause();
}

void MMBNCustomGauge::Unpause()
{
	m_timer.unpause();
}

void MMBNCustomGauge::SetFillingDuration(float time)
{
	m_time_limit = time;
}

Vector2f& MMBNCustomGauge::GetSize()
{
	return m_size;
}

bool MMBNCustomGauge::IsFull()
{
	return (m_state == FULL);
}



//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// BATTLE CHIP SELECTOR
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
MMBNBattleChipSelector::MMBNBattleChipSelector()
{
	
	#ifdef _DEBUG
		LOG("Create BattleChip Selector")
	#endif

	m_selector 		= ImgManager::GetImage("System/Chips/CustomWindow/chip_selector.png")	;
	m_cursor		= Animation::Load("System/Chips/CustomWindow/Cursor")					;
	m_cursor2		= Animation::Load("System/Chips/CustomWindow/Cursor2")					;
	m_cursor3		= Animation::Load("System/Chips/CustomWindow/Cursor3")					;
	
	m_normal_chip	= GameSystem::GetChipImage(GameSystem::NORMAL_CHIP)						;
	m_mega_chip		= GameSystem::GetChipImage(GameSystem::MEGA_CHIP)						;
	m_giga_chip		= GameSystem::GetChipImage(GameSystem::GIGA_CHIP)						;
	m_dark_chip		= GameSystem::GetChipImage(GameSystem::DARK_CHIP)						;

	m_size.x 		= m_selector->sizeX														;
	m_size.y 		= m_selector->sizeY														;
	
	//Put some chips for testing
	MMBNBattleChip* drk_sword = MMBNBattleChip::Load("DrkSword_D");
	drk_sword->SetExtendedDisplay(false);
	m_current_chips.push_back(drk_sword);
	
	MMBNBattleChip* cus_volt1 = MMBNBattleChip::Load("CusVolt1_C");
	cus_volt1->SetExtendedDisplay(false);
	m_current_chips.push_back(cus_volt1);
	
	MMBNBattleChip* sword = MMBNBattleChip::Load("Sword_S");
	sword->SetExtendedDisplay(false);
	m_current_chips.push_back(sword);
	
	MMBNBattleChip* recov10_ = MMBNBattleChip::Load("Recov10_+");
	recov10_->SetExtendedDisplay(false);
	m_current_chips.push_back(recov10_);
	
	MMBNBattleChip* recov10_r = MMBNBattleChip::Load("Recov10_R");
	recov10_r->SetExtendedDisplay(false);
	m_current_chips.push_back(recov10_r);
	
	m_chips_position[0].x = 13	; 	m_chips_position[0].y = 157	;
	m_chips_position[1].x = 37	; 	m_chips_position[1].y = 157	;
	m_chips_position[2].x = 61	; 	m_chips_position[2].y = 157	;
	m_chips_position[3].x = 85	; 	m_chips_position[3].y = 157	;
	m_chips_position[4].x = 109	; 	m_chips_position[4].y = 157	;
	m_chips_position[5].x = 13	; 	m_chips_position[5].y = 193	;
	m_chips_position[6].x = 37	; 	m_chips_position[6].y = 193	;
	m_chips_position[7].x = 61	; 	m_chips_position[7].y = 193	;
	
	for(unsigned int i(0) ; i < 5 ; ++i)
	{
		m_letters_position[i].x = m_chips_position[i].x + ICON_WIDTH / 2;
		m_letters_position[i].y = 185;
	}
	
	for(unsigned int i(5) ; i < 8 ; ++i)
	{
		m_letters_position[i].x = m_chips_position[i].x + ICON_WIDTH / 2;
		m_letters_position[i].y = 221;
	}
	
	m_current_position	= CHIP		;
	m_selected_chip		= 0			;

	m_ok_pressed		= false		;
	
	m_show_description 	= false		;
}

MMBNBattleChipSelector::~MMBNBattleChipSelector()
{
	#ifdef _DEBUG
		LOG("Destroy BattleChip Selector")
	#endif
	
	//if(m_cursor) delete m_cursor;
	//if(m_cursor2) delete m_cursor2;
	//if(m_cursor3) delete m_cursor3;
	
	vector<MMBNBattleChip*>::iterator it;
	for(it = m_current_chips.begin() ; it != m_current_chips.end() ; ++it)
		if(*it) delete *it;
}

void MMBNBattleChipSelector::Update(OSL_CONTROLLER* k)
{
	//RIGHT
	if(k->pressed.right)
	{
		if(m_selected_chip != 4) m_selected_chip++;
		else m_current_position = OK;
		
		if(m_selected_chip >= 8) m_selected_chip = 7;
	}
	//LEFT
	else if(k->pressed.left)
	{
		if(m_current_position == CHIP)
		{
			if(m_selected_chip > 0) m_selected_chip--;
			else m_selected_chip = 0;
		}
		else if(m_current_position == OK)
			m_current_position = CHIP;
	}
	//UP
	else if(k->pressed.up)
	{
		if( (m_selected_chip >= 5) && (m_selected_chip <= 7) ) m_selected_chip -= 5;
	}
	//DOWN
	else if(k->pressed.down)
	{
		if( (m_selected_chip >= 0) && (m_selected_chip <= 2) ) m_selected_chip += 5;
		else if( (m_selected_chip == 3) || (m_selected_chip == 4) ) m_selected_chip = 7;
	}
	
	//CAN PRESS OK ?
	if( (m_current_position == OK) && (k->pressed.cross) ) m_ok_pressed = true;
	
	//[]
	if(k->pressed.square)
		if(m_selected_chip < m_current_chips.size()) m_show_description = !m_show_description;
}

void MMBNBattleChipSelector::Display(float offX, float offY)
{
	//selector
	oslDrawImageXY(m_selector, m_position.x + offX, m_position.y + offY);
	
	//chips
	for(unsigned int i(0) ; i < m_current_chips.size() ; ++i)
	{
		if(m_current_chips[i])
		{
			//icon
			oslDrawImageXY(m_current_chips[i]->GetIcon(), m_position.x + offX + m_chips_position[i].x, m_position.y + offY + m_chips_position[i].y);
			
			//letter
			MMBNString letter;
			letter.SetFont(GameSystem::GetCustomWindowLetterFont());
			letter = m_current_chips[i]->GetLetter();
			letter.SetPosition(m_position.x + offX + m_letters_position[i].x - letter.GetStringWidth() / 2, m_position.y + offY + m_letters_position[i].y - letter.GetStringHeight() / 2);
			letter.Display();
		}
	}
	
	
	//current chip
	if( (m_current_position == CHIP) && (m_selected_chip < m_current_chips.size()) && (m_current_chips[m_selected_chip] != NULL) )
	{
		oslDrawImageXY(GameSystem::GetChipImage(m_current_chips[m_selected_chip]->GetType()), m_position.x, m_position.y);
		m_current_chips[m_selected_chip]->SetPosition(m_position.x + offX + 22, m_position.y + offY + 18);
		m_current_chips[m_selected_chip]->Display(offX, offY);
	}
	else
		oslDrawImageXY(GameSystem::GetChipImage(GameSystem::NORMAL_CHIP), m_position.x, m_position.y);
		
	//cursor
	switch(m_current_position)
	{
		case CHIP:
			m_cursor->Update();
			m_cursor->SetPosition(m_position.x + offX + m_chips_position[m_selected_chip].x + ICON_WIDTH / 2, m_position.y + offY + m_chips_position[m_selected_chip].y + ICON_HEIGHT / 2);
			m_cursor->Display(offX, offY);
			break;
		case OK:
			m_cursor2->Update();
			m_cursor2->SetPosition(m_position.x + offX + 155, m_position.y + offY + 182);
			m_cursor2->Display(offX, offY);
			break;
		case SHUFFLE:
			m_cursor3->Update();
			m_cursor3->Display(offX, offY);
			break;
	}
	
	//chip description
	if(m_show_description)
	{
		if( (m_selected_chip < m_current_chips.size()) && (m_current_position == CHIP) )
		{
			MMBNString s;
			s.SetFont(GameSystem::GetBattleChipNormalDescFont());
			s = m_current_chips[m_selected_chip]->GetDescription();
			
			oslDrawFillRect(m_position.x + offX + 5, 272 - s.GetStringHeight() - 15, m_position.x + offX + 5 + s.GetStringWidth() + 10, 267, RGB(240,240,240));
			oslDrawRect(m_position.x + offX + 5, 272 - s.GetStringHeight() - 15, m_position.x + offX + 5 + s.GetStringWidth() + 10, 267, RGB(0,0,0));
			s.SetPosition(m_position.x + offX + 10, 267 - s.GetStringHeight() - 5);
			s.Display();
		}
	}
	
	
}

void MMBNBattleChipSelector::Move(float x, float y)
{
	m_position.x += x;
	m_position.y += y;
}

void MMBNBattleChipSelector::SetPosition(float x, float y)
{
	m_position.x = x;
	m_position.y = y;
}

Vector2f& MMBNBattleChipSelector::GetPosition()
{
	return m_position;
}

Vector2f& MMBNBattleChipSelector::GetSize()
{
	return m_size;
}

bool MMBNBattleChipSelector::OkPressed()
{
	return m_ok_pressed;
}

void MMBNBattleChipSelector::ResetCursor()
{
	m_ok_pressed = false;
	m_show_description = false;
	m_current_position = CHIP;
	m_selected_chip = 0;
}










//==============================================================================

MMBNBattleMap::MMBNBattleMap()
{
	#ifdef _DEBUG
		LOG("Create BattleMap")
	#endif
}

MMBNBattleMap::~MMBNBattleMap()
{	
	#ifdef _DEBUG
		LOG("Destroy BattleMap")
	#endif
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// RUN
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
int MMBNBattleMap::Run()
{

	int screen = Update();
	if(screen == SCREEN_BATTLEMAP) Display();

	return screen;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// INITIALIZE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void MMBNBattleMap::Initialize()
{
	if(GameBattle::GetBattleInfo().bgName.compare("") != 0)
		m_bg = Animation::Load("Backgrounds/" + GameBattle::GetBattleInfo().bgName);

	m_grid = new MMBNPanelGrid();


	m_lifeBar = new MMBNLifeBar(m_grid->GetActor());
	m_lifeBar->SetPosition(10, 5);
	m_lifeBar->SetSize(66, 24);

	m_emotion = new MMBNEmotionDisplay(m_grid->GetActor());
	m_emotion->SetPosition(10, m_lifeBar->GetPosition().y + m_lifeBar->GetSize().y + 5);

	m_enemy_display_edge = ImgManager::GetImage("Battle/enemy_display_edge.png");

	m_enemy_deleted 	= Animation::Load("System/Animation/Battle/EnemyDeleted", false, false);
	
	m_battle_start	 	= Animation::Load("System/Animation/Battle/BattleStart", false, false);
	
	#ifdef _DEBUG
	m_display_debug_info = true;
	#endif
	
	m_battle_time_string.SetFont(GameSystem::GetBattleFont());
	m_battle_time_string = "00:00:00";
	
	m_custom_gauge = new MMBNCustomGauge();
	
	m_chip_selector = new MMBNBattleChipSelector();
	m_select_chip = false;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DESTROY
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void MMBNBattleMap::Destroy()
{
	//if(m_bg) delete m_bg;

	if(m_grid) delete m_grid;

	if(m_lifeBar) delete m_lifeBar;

	if(m_emotion) delete m_emotion;
	
	//if(m_enemy_deleted) delete m_enemy_deleted;
	
	if(m_custom_gauge) delete m_custom_gauge;
	
	if(m_chip_selector) delete m_chip_selector;
	
	SndManager::StopBGM(0);
	
	m_battle_timer.stop();
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DISPLAY
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void MMBNBattleMap::Display()
{

	
	//+++++++++++++++++++++++++++++++++++++
	// GRAPHICS
	//+++++++++++++++++++++++++++++++++++++
	//======================================
	// BACKGROUND
	//======================================
	if(m_bg.get())
	{
		m_bg->Update();
		m_bg->GetCurrentSprite().SetPosition(0,0);
		m_bg->GetCurrentSprite().Display();
	}
	else
		oslDrawFillRect(0, 0, 480, 272, RGB(0,0,0));
		
	//======================================
	// PANELS
	//======================================
	m_grid->Display();
		
	

	//======================================
	// ACTORS
	//======================================
	//+++++++++++++++++++++++++++++++++++++
	// CHIP SELECTOR
	//+++++++++++++++++++++++++++++++++++++
	if(m_select_chip)
	{
		m_chip_selector->SetPosition(0, 0);
		m_chip_selector->Display();
		m_lifeBar->Display(m_chip_selector->GetSize().x + 5);
		m_emotion->Display(m_chip_selector->GetSize().x + 5);
	}
	else
	{
		m_lifeBar->Display();
		m_emotion->Display();
		
		//+++++++++++++++++++++++++++++++++++++
		// CUSTOM GAUGE
		//+++++++++++++++++++++++++++++++++++++
		m_custom_gauge->SetPosition(240 - m_custom_gauge->GetSize().x / 2, 5);
		m_custom_gauge->Display();
		
		//+++++++++++++++++++++++++++++++++++++
		// BATTLE TIME
		//+++++++++++++++++++++++++++++++++++++
		DisplayBattleTime(240 - m_battle_time_string.GetStringWidth() / 2, 5 + m_custom_gauge->GetSize().y);
	}
	
	// display names of enemies
	vector<MMBNBattleActor*> vect = m_grid->GetEnemies();
	unsigned int inc = 0;
	for(unsigned int i = 0 ; i < vect.size() ; i++)
	{
		
		MMBNString s;
		MMBNFont* f = GameSystem::GetBattleEnemyNameFont();
		
		s.SetFont(f);
		s = vect[i]->GetName();
		s.SetPosition(475 - s.GetStringWidth(), 5 + inc);
		
		oslDrawFillRect(475 - s.GetStringWidth(), 5 + inc, 480, 15 + inc, RGB(40, 40, 40));
		s.Display();
		oslDrawImageXY(m_enemy_display_edge, 475 - s.GetStringWidth() - m_enemy_display_edge->sizeX, 5 + inc);

		inc += f->GetCharHeight();
	}

	//"battle start anim" not finished ?
	if(!m_battle_start->IsOver())
	{
		m_battle_start->Update();
		m_battle_start->SetPosition(240,136);
		m_battle_start->Display();
	}
	
	//battle over ?
	if(m_grid->BattleIsOver())
	{
		m_enemy_deleted->Update();
		m_enemy_deleted->SetPosition(240, 136);
		m_enemy_deleted->Display();
		
		//+++++++++++++++++++++++++++++++++++++
		// SOUND
		//+++++++++++++++++++++++++++++++++++++
		//======================================
		// BGM
		//======================================
		SndManager::PlayBGM("Enemy_Deleted.bgm", 0, true);
	}
	else
	{
		//+++++++++++++++++++++++++++++++++++++
		// SOUND
		//+++++++++++++++++++++++++++++++++++++
		//======================================
		// BGM
		//======================================
		if(GameBattle::GetBattleInfo().bgmName.compare("") != 0)
			SndManager::PlayBGM(GameBattle::GetBattleInfo().bgmName + ".bgm", 0, true);
	}
	
	
	//+++++++++++++++++++++++++++++++++++++
	// AFFICHAGE INFO
	//+++++++++++++++++++++++++++++++++++++
	#ifdef _DEBUG
		if(!m_display_debug_info) return;

		Vector2f v = m_grid->GetActor()->GetPosition();
		Vector2i v_panel = m_grid->GetActorPanel(m_grid->GetActor());
		ostringstream obj_oss(ostringstream::out);
		obj_oss << m_grid->GetActor()->GetName() << " X : " << v.x  << " Y : " << v.y << " TX : " << v_panel.x << " TY : " << v_panel.y;

		oslSetTextColor(RGBA(255,0,0,255));
		oslPrintf_xy(5,25,obj_oss.str().c_str());

		int posY = 35;

		for(unsigned int i = 0 ; i < m_grid->GetEnemies().size() ; i++)
		{
			Vector2f v = m_grid->GetEnemies()[i]->GetPosition();
			Vector2i v_panel = m_grid->GetActorPanel(m_grid->GetEnemies()[i]);
			ostringstream obj_oss(ostringstream::out);
			obj_oss << m_grid->GetEnemies()[i]->GetName() << " X : " << v.x  << " Y : " << v.y << " TX : " << v_panel.x << " TY : " << v_panel.y;

			oslPrintf_xy(5,posY,obj_oss.str().c_str());
			posY += 10;
		}
	#endif
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// UPDATE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
int MMBNBattleMap::Update()
{

	if(!m_battle_start->IsOver())
		return SCREEN_BATTLEMAP;
		
		
	if(!m_battle_timer.is_started())
		m_battle_timer.start();
		
	//si on ne doit pas g�rer les �v�nements, on vide la pile et on sort
	/*if(!m_eventsEnabled)
	{
		sf::Event Event;
		while (App.GetEvent(Event));
		return SCREEN_BATTLEMAP;
	}*/

	//=============================
	// TRAITEMENT DES EVENEMENTS
	//=============================
	OSL_CONTROLLER* k = oslReadKeys();

	if(k->pressed.triangle) return SCREEN_CHARACTER_SELECT;
	
	#ifdef _DEBUG
		if(k->pressed.select) m_display_debug_info = !m_display_debug_info;
	#endif
	
	if(!m_select_chip) m_grid->Update(k);
	
	if(!m_grid->BattleIsOver())
	{
		
		if(m_select_chip) m_chip_selector->Update(k);
		else
		{
			m_custom_gauge->Update();
		}
		
		//gestion du h�ros
		ActorHandle(k);
		
	}
	else
	{
		if(m_enemy_deleted->IsOver() && k->pressed.cross) return SCREEN_CHARACTER_SELECT;
	}

	//par d�faut, on reste sur le m�me �cran
	return SCREEN_BATTLEMAP;
}

void MMBNBattleMap::ActorHandle(OSL_CONTROLLER* k)
{
	if( (k->pressed.R || k->pressed.L) && m_custom_gauge->IsFull() )
	{
		m_custom_gauge->Reset();
		m_custom_gauge->Pause();
		m_chip_selector->ResetCursor();
		m_select_chip = true;
		m_battle_timer.pause();
	}
	
	if(m_chip_selector->OkPressed())
	{
		m_select_chip = false;
		m_custom_gauge->Unpause();
		m_battle_timer.unpause();
	}
	
	
}

void MMBNBattleMap::DisplayBattleTime(int x, int y)
{
	unsigned int min, sec, centi;
	GetBattleTime(&min, &sec, &centi);
	
	ostringstream oss(ostringstream::out);
	oss.width(2);
	oss.fill('0');
	oss << min << ":";
	oss.width(2);
	oss.fill('0');
	oss << sec << ":";
	oss.width(2);
	oss.fill('0');
	oss << centi;
	
	m_battle_time_string = oss.str();
	m_battle_time_string.SetPosition(x,y);
	m_battle_time_string.Display();
	
}

void MMBNBattleMap::GetBattleTime(unsigned int* min, unsigned int* sec, unsigned int* centi)
{
	float time = m_battle_timer.get_ticks();
	*sec = time / 1000.0f;
	*centi = (time - ( (*sec) * 1000.0f)) / 10.0f;
	*min = *sec / 60;
	*sec = *sec % 60;
	
}
