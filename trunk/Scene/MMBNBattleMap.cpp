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
		
	//load the pictures for each type of panels
	m_panel_pictures[PLAYER] = ImgManager::GetImage("Battle/player_panel.png");
	m_panel_pictures[ENEMY]  = ImgManager::GetImage("Battle/enemy_panel.png");
	m_panel_pictures[EMPTY]	 = NULL;
	m_panel_pictures[BROKEN] = NULL;

	//actor and enemies
	m_actor = MMBNBattleActor::Load("DarkRoxas");
	PutActorOnPanel(m_actor, 2, 2);

	m_enemies.push_back( MMBNBattleActor::Load("Riku") );
	PutActorOnPanel(m_enemies[0], 7, 2);

	m_enemies.push_back( MMBNBattleActor::Load("Sora") );
	PutActorOnPanel(m_enemies[1], 8, 1);
	
	m_enemies.push_back( MMBNBattleActor::Load("DualWieldRoxas") );
	PutActorOnPanel(m_enemies[2], 8, 3);

	vector<MMBNBattleActor*>::iterator it;
	for(it = m_enemies.begin() ; it != m_enemies.end() ; it++)
	{
		(*it)->SetDirection(MMBNBattleActor::LEFT);
		(*it)->SetState(MMBNBattleActor::BATTLE_STANDING);
	}

	m_life_font = MMBNFont::Load("MMBNBattleFont");

}

MMBNPanelGrid::~MMBNPanelGrid()
{
	if(m_actor) delete m_actor;

	vector<MMBNBattleActor*>::iterator it;
	for(it = m_enemies.begin() ; it != m_enemies.end() ; it++)
		delete (*it);

	if(m_life_font) delete m_life_font;
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

			if( (*m_panels)[i][j] == PLAYER )
				if( m_panel_pictures[PLAYER] != NULL ) oslDrawImageXY( m_panel_pictures[PLAYER], x, y );

			if( (*m_panels)[i][j] == ENEMY )
				if( m_panel_pictures[ENEMY] != NULL ) oslDrawImageXY( m_panel_pictures[ENEMY], x, y );

			if( (*m_panels)[i][j] == EMPTY )
				if( m_panel_pictures[EMPTY] != NULL ) oslDrawImageXY( m_panel_pictures[EMPTY], x, y );

			if( (*m_panels)[i][j] == BROKEN )
				if( m_panel_pictures[BROKEN] != NULL ) oslDrawImageXY( m_panel_pictures[BROKEN], x, y );
			
		}


	//======================================
	// ACTORS
	//======================================
	m_actor->Display();

	vector<MMBNBattleActor*>::iterator it;
	for(it = m_enemies.begin() ; it != m_enemies.end() ; it++)
		(*it)->Display();

	//display life of enemies
	for(it = m_enemies.begin() ; it != m_enemies.end() ; it++)
	{
		MMBNString life_s;
		life_s.SetFont(m_life_font);

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

MMBNPanelGrid::PanelType MMBNPanelGrid::GetPanelType(unsigned int x, unsigned int y)
{
	return (*m_panels)[x][y];
}

bool MMBNPanelGrid::IsInsideGrid(int x, int y)
{
	if( (x >= 0) && (x < m_width) && (y >= 0) && (y < m_height) ) return true;

	return false;
}

MMBNBattleActor* MMBNPanelGrid::GetActor()
{
	return m_actor;
}

vector<MMBNBattleActor*>& MMBNPanelGrid::GetEnemies()
{
	return m_enemies;
}

void MMBNPanelGrid::ActorHandle(OSL_CONTROLLER* k)
{

	Vector2i v = GetActorPanel(m_actor);

	if(k->pressed.left)
		v.x = v.x - 1;
	else if(k->pressed.right)
		v.x = v.x + 1;
	else if(k->pressed.up)
		v.y = v.y - 1;
	else if(k->pressed.down)
		v.y = v.y + 1;
	
	if( IsInsideGrid(v.x, v.y) && ( GetPanelType(v.x, v.y) == PLAYER ) )
		PutActorOnPanel(m_actor, v.x, v.y);
	
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
















//==============================================================================

MMBNBattleMap::MMBNBattleMap()
{
}

MMBNBattleMap::~MMBNBattleMap()
{
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
	m_bg = new Animation("Map/Background", std::vector<int>(7,150), false, true);

	m_grid = new MMBNPanelGrid();


	m_lifeBar = new MMBNLifeBar(m_grid->GetActor());
	m_lifeBar->SetPosition(10, 5);
	m_lifeBar->SetSize(66, 24);

	m_emotion = new MMBNEmotionDisplay(m_grid->GetActor());
	m_emotion->SetPosition(10, m_lifeBar->GetPosition().y + m_lifeBar->GetSize().y + 5);

	m_enemy_display_edge = ImgManager::GetImage("Battle/enemy_display_edge.png");


	m_display_debug_info = true;
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
	// SOUND
	//+++++++++++++++++++++++++++++++++++++
	//======================================
	// BGM
	//======================================
	SndManager::PlayBGM("Battle_Field.bgm", 0, true);


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
	if(k->pressed.R) m_display_debug_info = !m_display_debug_info;
	

	//gestion du héros
	ActorHandle(k);

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

	m_grid->ActorHandle(k);
	
}
