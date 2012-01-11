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
			(*m_panels_team)[i][j] = PLAYER;
		
	for(unsigned int i = (m_width / 2) ; i < m_width ; i++)
		for(unsigned int j = 0 ; j < m_height ; j++)
			(*m_panels_team)[i][j] = ENEMY;
		
	//all the panels are normal
	for(unsigned int i = 0 ; i < m_width ; i++)
		for(unsigned int j = 0 ; j < m_height ; j++)
			(*m_panels)[i][j] = NORMAL;
			
	//load the pictures for each team
	m_panel_pictures[PLAYER] = ImgManager::GetImage("System/Animation/Battle/Panels/player_panel.png");
	m_panel_pictures[ENEMY]  = ImgManager::GetImage("System/Animation/Battle/Panels/enemy_panel.png");
	
	//load the animations for each type of panels
	m_panel_animations[NORMAL] 	= Animation::Load("System/Animation/Battle/Panels/Normal", false, true);
	/*m_panel_animations[EMPTY] 	= NULL;
	m_panel_animations[BROKEN] 	= NULL;
	m_panel_animations[CRACKED] = NULL;
	m_panel_animations[FIRE] 	= NULL;
	m_panel_animations[ICE] 	= NULL;
	m_panel_animations[GRASS] 	= NULL;
	m_panel_animations[POISON] 	= NULL;
	m_panel_animations[WATER] 	= NULL;*/
	
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
		m_ia.push_back(new MMBNBattleIA(this, m_enemies.back(), ENEMY));
	}
	
	#ifdef _DEBUG
	LOG("Battle enemies loaded")
	#endif
	
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

inline MMBNPanelGrid::PanelType MMBNPanelGrid::GetPanelType(unsigned int x, unsigned int y)
{
	return (*m_panels)[x][y];
}

inline MMBNPanelGrid::PanelTeam MMBNPanelGrid::GetPanelTeam(unsigned int x, unsigned int y)
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
		
		if( IsInsideGrid(v.x, v.y) && ( GetPanelTeam(v.x, v.y) == PLAYER ) )
			PutActorOnPanel(m_actor, v.x, v.y);
	}
	
	//ATTACKING
	if( k->pressed.cross )
	{
		//play attack anim
		if(m_actor->GetState() == MMBNBattleActor::BATTLE_STANDING) //attack only if in standing phase
			m_actor->SetState(MMBNBattleActor::BATTLE_ATTACK);
			
		Vector2i pos;
		
		vector<MMBNBattleActor*>::iterator it;
		for(it = m_enemies.begin() ; it != m_enemies.end() ; it++)
		{
			pos = GetActorPanel(*it);
			if(pos.x == (v.x + 1) && (pos.y == v.y))
			{
				if((*it)->GetState() == MMBNBattleActor::BATTLE_STANDING)
					(*it)->SetState(MMBNBattleActor::BATTLE_DAMAGED);
					
				m_actor->Attack(*it);
			}
		}		
	}
	
}

void MMBNPanelGrid::Update(OSL_CONTROLLER* k)
{
	//Update IA
	vector<MMBNBattleIA*>::iterator it;
	
	for(it = m_ia.begin() ; it != m_ia.end() ; it++)
		(*it)->Update();
		
	ActorHandle(k);
	
	
	//state management
	
	//actor
	{
		//dead ?
		if(m_actor->IsDead()) m_actor->SetState(MMBNBattleActor::BATTLE_DEAD);
		
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
					i--;
				}
				break;
				
			case MMBNBattleActor::BATTLE_STANDING:
			default:
				break;
			
		}
	}
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
	
	for(unsigned int i = 0 ; i < EMOTIONS_NB ; i++)
		m_emotions[i] = NULL;

	m_current_emotion = NORMAL;
}

MMBNEmotionDisplay::MMBNEmotionDisplay(MMBNBattleActor* mmbnba)
{
	#ifdef _DEBUG
		LOG("Create EmotionDisplay")
	#endif
	
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
	m_actor_team	= MMBNPanelGrid::ENEMY	;
}

MMBNBattleIA::MMBNBattleIA(MMBNPanelGrid* m, MMBNBattleActor* a, MMBNPanelGrid::PanelTeam t)
{
	#ifdef _DEBUG
		LOG("Create BattleIA")
	#endif
	
	m_map			= m	;
	m_actor			= a	;
	m_actor_team	= t	;
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
	m_bg = Animation::Load("Backgrounds/" + GameBattle::GetBattleInfo().bgName);

	m_grid = new MMBNPanelGrid();


	m_lifeBar = new MMBNLifeBar(m_grid->GetActor());
	m_lifeBar->SetPosition(10, 5);
	m_lifeBar->SetSize(66, 24);

	m_emotion = new MMBNEmotionDisplay(m_grid->GetActor());
	m_emotion->SetPosition(10, m_lifeBar->GetPosition().y + m_lifeBar->GetSize().y + 5);

	m_enemy_display_edge = ImgManager::GetImage("Battle/enemy_display_edge.png");

	m_enemy_deleted = Animation::Load("System/Animation/Battle/EnemyDeleted", false, false);
	
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
	oslSetScreenClipping(0, 0, 480, 272);
	m_bg->Update();
	m_bg->GetCurrentSprite().SetPosition(0,0);
	m_bg->GetCurrentSprite().Display();

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

	//battle over ?
	if(m_grid->BattleIsOver())
	{
		m_enemy_deleted->Update();
		m_enemy_deleted->SetPosition(240, 161);
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

	if(!m_battle_timer.is_started())
		m_battle_timer.start();
		
	//si on ne doit pas gérer les évènements, on vide la pile et on sort
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

	if(k->pressed.triangle) return SCREEN_TITLE;
	
	#ifdef _DEBUG
		if(k->pressed.select) m_display_debug_info = !m_display_debug_info;
	#endif
	
	if(!m_grid->BattleIsOver())
	{
		
		if(m_select_chip) m_chip_selector->Update(k);
		else
		{
			m_grid->Update(k);
			m_custom_gauge->Update();
		}
		
		//gestion du héros
		ActorHandle(k);
		
	}
	else
	{
		if(m_enemy_deleted->IsOver() && k->pressed.cross) return SCREEN_TITLE;
	}

	//par défaut, on reste sur le même écran
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
