#include "MMBNBattleMap.h"

using namespace std;

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// MMBNPANELGRID
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
MMBNPanelGrid::MMBNPanelGrid()
{
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
	m_panel_animations[EMPTY] 	= NULL;
	m_panel_animations[BROKEN] 	= NULL;
	m_panel_animations[CRACKED] = NULL;
	m_panel_animations[FIRE] 	= NULL;
	m_panel_animations[ICE] 	= NULL;
	m_panel_animations[GRASS] 	= NULL;
	m_panel_animations[POISON] 	= NULL;
	m_panel_animations[WATER] 	= NULL;
	
	//actor and enemies
	m_actor = MMBNBattleActor::Load("Roxas", false, true, false);
	PutActorOnPanel(m_actor, 2, 2);

	m_enemies.push_back( MMBNBattleActor::Load("Killua", true, false, true) );
	PutActorOnPanel(m_enemies[0], 5, 0);
	
	/*m_enemies.push_back( MMBNBattleActor::Load("Sora") );
	PutActorOnPanel(m_enemies[1], 5, 1);
	
	m_enemies.push_back( MMBNBattleActor::Load("DarkRoxas") );
	PutActorOnPanel(m_enemies[2], 5, 2);
	
	m_enemies.push_back( MMBNBattleActor::Load("Megaman") );
	PutActorOnPanel(m_enemies[3], 5, 3);

	m_enemies.push_back( MMBNBattleActor::Load("Protoman") );
	PutActorOnPanel(m_enemies[4], 5, 4);*/
	
	vector<MMBNBattleActor*>::iterator it;
	for(it = m_enemies.begin() ; it != m_enemies.end() ; it++)
	{
		(*it)->SetDirection(MMBNBattleActor::LEFT);
		(*it)->SetState(MMBNBattleActor::BATTLE_STANDING);
	}

	//ia
	m_ia.push_back(new MMBNBattleIA(this, m_enemies[0], ENEMY));
	/*m_ia.push_back(new MMBNBattleIA(this, m_enemies[1], ENEMY));
	m_ia.push_back(new MMBNBattleIA(this, m_enemies[2], ENEMY));
	m_ia.push_back(new MMBNBattleIA(this, m_enemies[3], ENEMY));
	m_ia.push_back(new MMBNBattleIA(this, m_enemies[4], ENEMY));*/

}

MMBNPanelGrid::~MMBNPanelGrid()
{
	//actor
	if(m_actor) delete m_actor;

	//enemies
	vector<MMBNBattleActor*>::iterator it;
	for(it = m_enemies.begin() ; it != m_enemies.end() ; it++)
		delete (*it);

	//panel animations
	for(unsigned int i = 0 ; i < PANEL_TYPES_NB ; i++)
		if(m_panel_animations[i]) delete m_panel_animations[i];
			
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
	for(unsigned int i = 0 ; i < EMOTIONS_NB ; i++)
		m_emotions[i] = NULL;

	m_current_emotion = NORMAL;
}

MMBNEmotionDisplay::MMBNEmotionDisplay(MMBNBattleActor* mmbnba)
{
	string name = mmbnba->GetName();

	m_emotions[NORMAL] = ImgManager::GetImage("Actors/" + name + "/Emotions/normal.png");

	m_current_emotion = NORMAL;
}

MMBNEmotionDisplay::~MMBNEmotionDisplay()
{

}

void MMBNEmotionDisplay::Display(float offX, float offY)
{
	oslDrawImageXY(m_emotions[m_current_emotion], m_position.x, m_position.y);
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
	m_actor = NULL;
	m_life_font = MMBNFont::Load("MMBNActorLifeFont");
}

MMBNLifeBar::MMBNLifeBar(MMBNBattleActor* mmbnba)
{
	m_actor = mmbnba;
	m_life_font = MMBNFont::Load("MMBNActorLifeFont");
}

MMBNLifeBar::~MMBNLifeBar()
{
	if(m_life_font) delete m_life_font;
}

void MMBNLifeBar::Display(float offX, float offY)
{
	oslDrawFillRect(m_position.x, m_position.y, m_position.x + m_size.x, m_position.y + m_size.y, RGB(64,64,64));
	oslDrawRect(m_position.x, m_position.y, m_position.x + m_size.x, m_position.y + m_size.y, RGB(255,255,255));

	MMBNString life_s;
	life_s.SetFont(m_life_font);

	ostringstream life_oss(ostringstream::out);
	life_oss << m_actor->GetHP();

	life_s = life_oss.str();

	int w = life_s.GetStringWidth();
	int h = m_life_font->GetCharHeight();

	//life_s.SetPosition( (m_size.x - w) / 2 + m_position.x, (m_size.y - h) / 2 + m_position.y );
	life_s.SetPosition( m_position.x + m_size.x - w - (m_life_font->GetCharWidth() / 2), (m_size.y - h) / 2 + m_position.y );

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
	m_map			= NULL	;
	m_actor			= NULL	;
	m_actor_team	= MMBNPanelGrid::ENEMY	;
}

MMBNBattleIA::MMBNBattleIA(MMBNPanelGrid* m, MMBNBattleActor* a, MMBNPanelGrid::PanelTeam t)
{
	m_map			= m	;
	m_actor			= a	;
	m_actor_team	= t	;
}

MMBNBattleIA::~MMBNBattleIA()
{

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
	if(m_full) delete m_full;
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








//==============================================================================

MMBNBattleMap::MMBNBattleMap()
{
	LOG("Create BattleMap")
}

MMBNBattleMap::~MMBNBattleMap()
{
	LOG("Destroy BattleMap")
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
	m_bg = Animation::Load("Map/Background");
	//m_bg = new Animation("Map/Background", std::vector<int>(7,150), false, true);

	m_grid = new MMBNPanelGrid();


	m_lifeBar = new MMBNLifeBar(m_grid->GetActor());
	m_lifeBar->SetPosition(10, 5);
	m_lifeBar->SetSize(66, 24);

	m_emotion = new MMBNEmotionDisplay(m_grid->GetActor());
	m_emotion->SetPosition(10, m_lifeBar->GetPosition().y + m_lifeBar->GetSize().y + 5);

	m_enemy_display_edge = ImgManager::GetImage("Battle/enemy_display_edge.png");

	m_enemy_deleted = Animation::Load("System/Animation/Battle/EnemyDeleted", false, false);
	
	m_display_debug_info = true;
	
	m_battle_time_string.SetFont(GameSystem::GetBattleFont());
	m_battle_time_string = "00:00:00";
	
	m_custom_gauge = new MMBNCustomGauge();
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DESTROY
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void MMBNBattleMap::Destroy()
{
	if(m_bg) delete m_bg;

	if(m_grid) delete m_grid;

	if(m_lifeBar) delete m_lifeBar;

	if(m_emotion) delete m_emotion;
	
	if(m_enemy_deleted) delete m_enemy_deleted;
	
	if(m_custom_gauge) delete m_custom_gauge;
	
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
	m_lifeBar->Display();
	m_emotion->Display();

	// display names of enemies
	vector<MMBNBattleActor*> vect = m_grid->GetEnemies();
	unsigned int inc = 0;
	for(unsigned int i = 0 ; i < vect.size() ; i++)
	{
		//if enemy dead, don't display life
		if(vect[i]->IsDead()) continue;
		
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

	//+++++++++++++++++++++++++++++++++++++
	// CUSTOM GAUGE
	//+++++++++++++++++++++++++++++++++++++
	m_custom_gauge->SetPosition(240 - m_custom_gauge->GetSize().x / 2, 5);
	m_custom_gauge->Display();
	
	//+++++++++++++++++++++++++++++++++++++
	// BATTLE TIME
	//+++++++++++++++++++++++++++++++++++++
	DisplayBattleTime(240 - m_battle_time_string.GetStringWidth() / 2, 5 + m_custom_gauge->GetSize().y);
		
		
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
		SndManager::PlayBGM("Battle_Field.bgm", 0, true);
	}
	
	
	//+++++++++++++++++++++++++++++++++++++
	// AFFICHAGE INFO
	//+++++++++++++++++++++++++++++++++++++
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
	if(k->pressed.select) m_display_debug_info = !m_display_debug_info;
	
	if(!m_grid->BattleIsOver())
	{
		if( (k->pressed.R || k->pressed.L) && m_custom_gauge->IsFull() ) m_custom_gauge->Reset();
		
		m_grid->Update(k);
		m_custom_gauge->Update();
		
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

	/*Vector2i v = m_grid->GetActorPanel(m_actor);

	if(k->pressed.left)
		v.x = v.x - 1;
	else if(k->pressed.right)
		v.x = v.x + 1;
	else if(k->pressed.up)
		v.y = v.y - 1;
	else if(k->pressed.down)
		v.y = v.y + 1;
	


	if( m_grid->IsInsideGrid(v.x, v.y) && ( m_grid->GetPanelType(v.x, v.y) == MMBNPanelGrid::PLAYER ) )
		m_grid->PutActorOnPanel(m_actor, v.x, v.y);*/
	
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
