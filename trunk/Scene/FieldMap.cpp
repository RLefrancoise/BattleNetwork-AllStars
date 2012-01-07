#include "FieldMap.h"
#include "Utils.h"
#include "Variables.h"

#include "Actor.h"
#include "Object.h"

#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>

using namespace std;

#define PI 3.14159265

float coter = sqrt((float) (32 * 32 + 16 * 16));
float newDiag = sqrt(coter * coter + coter * coter);


FieldMap::FieldMap()
{	
	LOG("Create FieldMap")
}

FieldMap::~FieldMap()
{
	LOG("Destroy FieldMap")
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// INITIALIZE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void FieldMap::Initialize()
{
	//=====================================
	// MAPPING
	//=====================================

	m_grid.tileXSize = 64.0f;
	m_grid.tileYSize = 32.0f;
	m_grid.xTilesNb = 50;
	m_grid.yTilesNb = 25;

	m_grid.xScroll = 0;
	m_grid.yScroll = 0;

	m_grid.origin = Vector2f(240, 5);

	m_grid.yVectorPoint.x = m_grid.origin.x + (m_grid.tileXSize / 2);
	m_grid.yVectorPoint.y = m_grid.origin.y + (m_grid.tileYSize / 2);
	m_grid.xVectorPoint.x = m_grid.origin.x - (m_grid.tileXSize / 2);
	m_grid.xVectorPoint.y = m_grid.origin.y + (m_grid.tileYSize / 2);

	m_grid.xVector.x = m_grid.xVectorPoint.x - m_grid.origin.x;
	m_grid.xVector.y = m_grid.xVectorPoint.y - m_grid.origin.y;

	m_grid.yVector.x = m_grid.yVectorPoint.x - m_grid.origin.x;
	m_grid.yVector.y = m_grid.yVectorPoint.y - m_grid.origin.y;

	boost::array<int, 2> shape = {{ m_grid.xTilesNb, m_grid.yTilesNb }};

	MappingPtr pfm( new Mapping(shape) );
	m_grid.firstMapping.swap(pfm);

	CollisionsPtr pCollisions( new Collisions(shape) );
	m_grid.collisions.swap(pCollisions);

	ObjectsPtr pObjects( new Objects(shape) );
	m_grid.objects.swap(pObjects);

	for(int x = 0 ; x < m_grid.xTilesNb ; x++)
	{
		for(int y = 0 ; y < m_grid.yTilesNb ; y++)
		{
			// premier mapping
			(*m_grid.firstMapping)[x][y].first = -1; //-1 pour absence de sprite ÅEafficher
			(*m_grid.firstMapping)[x][y].second = -1; //-1 pour absence de sprite ÅEafficher

			// collisions
			(*m_grid.collisions)[x][y] = 0;
			

		}
	}

	m_grid.tileset = NULL;

	//=====================================
	// GRAPHICS
	//=====================================
	m_actor = Actor::Load("Sora");
	m_actor->SetPosition(160,260);
	m_actor->SetTileWidth(0);
	m_actor->SetTileHeight(0);

	AddElement(m_actor, 160, 260, false, "Sora");

	m_grid.xScroll = m_actor->GetPosition().x - 240;
	m_grid.yScroll = m_actor->GetPosition().y - 136;

	m_view.SetSize(480,272);
	m_view.SetCenter(160,260);

	m_bg = Animation::Load("Map/Background");

	bool properties[Actor::PROPERTIES_NB] = {false, false};
	Actor* welcome = Actor::Load("Welcome", properties);
	welcome->SetTileWidth(0);
	welcome->SetTileHeight(0);
	AddElement(welcome, 9, 11, true, "Welcome");

	Actor* protoman = Actor::Load("Protoman");
	AddElement(protoman, 11, 10, true, "Protoman");
	
	
	
	Load("CentralArea");
	
}

void FieldMap::Destroy()
{
	if(m_actor) delete m_actor;
	if(m_bg) delete m_bg;

	for(int i = 0 ; i < m_grid.xTilesNb ; ++i)
		for(int j = 0 ; j < m_grid.yTilesNb ; ++j)
		{
			if( (*m_grid.objects)[i][j] ) delete (*m_grid.objects)[i][j];
		}
	
	vector<FieldMapElement*>::iterator it2 = m_displayables.begin();
	for(it2 = m_displayables.begin() ; it2 != m_displayables.end() ; ++it2)
		if(*it2) delete *it2;
		
	m_displayables.clear();
	
	SndManager::StopBGM(0);
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// RUN
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
int FieldMap::Run()
{

	int screen = Update();
	if(screen == SCREEN_FIELDMAP) Display();

	return screen;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DISPLAY
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void FieldMap::Display()
{

	//+++++++++++++++++++++++++++++++++++++
	// GRAPHICS
	//+++++++++++++++++++++++++++++++++++++
	//======================================
	// BACKGROUND
	//======================================
	/*App.SetView(App.GetDefaultView());
	m_bg->Update();
	m_bg->GetCurrentSprite().SetPosition(0,0);
	App.Draw(m_bg->GetCurrentSprite());
	App.SetView(m_view);*/
	oslSetScreenClipping(0, 0, 480, 272);
	m_bg->Update();
	m_bg->GetCurrentSprite().SetPosition(0,0);
	m_bg->GetCurrentSprite().Display();
	//======================================
	// MAP
	//======================================
	DisplayIsoMap();
	
	//======================================
	// ACTOR & ENEMIES
	//======================================
	//m_actor->Move(-m_grid.xScroll, -m_grid.yScroll);
	//m_actor->Display(-240, -136);

	//+++++++++++++++++++++++++++++++++++++
	// SOUND
	//+++++++++++++++++++++++++++++++++++++
	//======================================
	// BGM
	//======================================
	SndManager::PlayBGM("Digital_Strider.bgm", 0, true);

	//=======================================
	// ISO
	//=======================================
	Vector2f actorPos = GetActorTile(m_actor->GetPosition().x, m_actor->GetPosition().y);
	ostringstream ap_oss(ostringstream::out);
	ap_oss << "Actor Tile X : " << actorPos.x  << " Y : " << actorPos.y;

	oslSetTextColor(RGBA(255,0,0,255));
	oslSetBkColor(RGBA(0,0,0,0));
	oslSetScreenClipping(0, 0, 480, 272);
	oslPrintf_xy(5,20,ap_oss.str().c_str());

	ostringstream p_oss(ostringstream::out);
	p_oss << "Actor X : " << m_actor->GetPosition().x  << " Y : " << m_actor->GetPosition().y;

	oslSetTextColor(RGBA(255,0,0,255));
	oslSetBkColor(RGBA(0,0,0,0));
	oslSetScreenClipping(0, 0, 480, 272);
	oslPrintf_xy(5,30,p_oss.str().c_str());

	//m_actorTileText.SetPosition(20 + (App.GetView().GetCenter().x - App.GetView().GetSize().x / 2), 100 + (App.GetView().GetCenter().y - App.GetView().GetSize().y / 2));
	//App.Draw(m_actorTileText);

	int posY = 50;
	oslSetTextColor(RGBA(255,0,0,255));
	oslSetBkColor(RGBA(0,0,0,0));

	vector<FieldMapElement*>::iterator it;

	for(it = m_displayables.begin() ; it != m_displayables.end() ; it++)
	{
		Vector2f objPos = (*it)->GetPosition();
		Vector2f t_objPos = GetActorTile(objPos.x, objPos.y);
		//Vector2f s_pos = (*it)->GetColliShape().GetPosition();
		ostringstream obj_oss(ostringstream::out);
		obj_oss << "Obj X : " << objPos.x  << " Y : " << objPos.y << " TX : " << t_objPos.x << " TY : " << t_objPos.y /*<< " SX : " << s_pos.x << " SY : " << s_pos.y*/ << " ==> " << m_elements_ids[*it];

		oslPrintf_xy(5,posY,obj_oss.str().c_str());
		posY += 10;

	}
	//Vector2f objPos = (*m_grid.objects)[6][11]->GetColliShape().GetPosition();
	//ostringstream obj_oss(ostringstream::out);
	//obj_oss << "Obj X : " << objPos.x  << " Y : " << objPos.y;

	//oslPrintf_xy(5,50,obj_oss.str().c_str());

}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// UPDATE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
int FieldMap::Update()
{

	//si on ne doit pas gÈrer les ÈvËnements, on vide la pile et on sort
	/*if(!m_eventsEnabled)
	{
		sf::Event Event;
		while (App.GetEvent(Event));
		return SCREEN_BATTLEMAP;
	}*/

	//=============================
	// TRAITEMENT DES EVENEMENTS
	//=============================
	/*sf::Event Event;
	while (App.PollEvent(Event))
	{
		// FenÍtre fermÈe
		if (Event.Type == sf::Event::Closed)
			return SCREEN_EXIT;

		// Touche 'echap' appuyÈe
		if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))
			return SCREEN_EXIT;
	}
*/
	OSL_CONTROLLER* k = oslReadKeys();

	if(k->pressed.triangle) return SCREEN_TITLE;

	//gestion du hÈros
	ActorHandle();

	//gestion de la camÈra

	m_grid.xScroll = m_actor->GetPosition().x - 240;
	m_grid.yScroll = m_actor->GetPosition().y - 136;

	m_view.SetCenter( m_actor->GetPosition().x, m_actor->GetPosition().y );

	//par dÈfaut, on reste sur le mÍme Ècran
	return SCREEN_FIELDMAP;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// LOAD
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void FieldMap::Load(string file)
{

	LOG( "Loading map " + (string("Map/") + file) )

	//--------------------
	// INFO
	//--------------------
	ifstream in_info( (string("Map/") + file + string("/info.txt")).c_str() , ifstream::in);
	LOG( "Loading " + (string("Map/") + file + string("/info.txt")) )

	if(!in_info.good())
	{
		LOG( "Can't load " + (string("Map/") + file + string("/info.txt")) )
		oslQuit();
	}

	string line;
	while(getline(in_info, line))
	{

		if(line.find("tileset") == 0)
		{
			
			vector<string> v = StringUtils::Split(line, "=");
			line = v[1];
			int pngIndex = line.find_last_of(".png");
			line = line.substr(0, pngIndex - 3);
			line += ".png";
			
			m_grid.tileset = ImgManager::GetImage(line);
			LOG( "Tileset loaded" )
		}
		else if(line.find("width") == 0)
		{
			int equalsIndex = line.find_first_of("=");
			line = line.substr(equalsIndex + 1);

			istringstream iss(line);
			int width = 0;
			iss >> width;

			m_grid.xTilesNb = width;
			LOG( "Width initialized" )
		}
		else if(line.find("height") == 0)
		{
			int equalsIndex = line.find_first_of("=");
			line = line.substr(equalsIndex + 1);

			istringstream iss(line);
			int height = 0;
			iss >> height;

			m_grid.yTilesNb = height;
			LOG( "Height initialized" )
		}
	}

	in_info.close();

	LOG( "Loading " + (string("Map/") + file + string("/info.txt done")) )
	LOG( "Initialize map..." )

	//reshape the grid with new size
	boost::array<int, 2> shape = {{ m_grid.xTilesNb, m_grid.yTilesNb }};
	
	MappingPtr pfm( new Mapping(shape) );
	m_grid.firstMapping.swap(pfm);

	ObjectsPtr pObjects( new Objects(shape) );
	m_grid.objects.swap(pObjects);

	(*m_grid.objects)[6][11] = new Object();
	(*m_grid.objects)[6][11]->SetSprite( ImgManager::GetImage("Map/Objects/Comptoir/1.png") );
	//(*m_grid.objects)[6][11]->SetOrigin(0,21);
	(*m_grid.objects)[6][11]->SetOrigin(64,53);
	(*m_grid.objects)[6][11]->SetTileWidth(0);
	(*m_grid.objects)[6][11]->SetTileHeight(2);
	(*m_grid.objects)[6][11]->GetColliShape().AddPoint(64,53);
	(*m_grid.objects)[6][11]->GetColliShape().AddPoint(0,21);
	(*m_grid.objects)[6][11]->GetColliShape().AddPoint(8 + 2,16 - 2);
	(*m_grid.objects)[6][11]->GetColliShape().AddPoint(72 + 2,49 - 2);
	(*m_grid.objects)[6][11]->GetColliShape().SetColor(RGB(0,0,255));
	
	AddElement( (*m_grid.objects)[6][11], 6, 11, true, "Comptoir");
	//PutActorOnTile((*m_grid.objects)[6][11], 6, 11);
	//m_displayables.push_back( (*m_grid.objects)[6][11] );

	(*m_grid.objects)[8][9] = new Object();
	(*m_grid.objects)[8][9]->SetSprite( ImgManager::GetImage("Map/Objects/Comptoir2/1.png") );
	(*m_grid.objects)[8][9]->SetTileWidth(4);
	(*m_grid.objects)[8][9]->SetTileHeight(4);
	(*m_grid.objects)[8][9]->SetOrigin(111 + 16,133 + 8);
	//(*m_grid.objects)[4][5]->SetOrigin(124,16);
	(*m_grid.objects)[8][9]->GetColliShape().AddPoint(124,16);
	(*m_grid.objects)[8][9]->GetColliShape().AddPoint(133,20);
	(*m_grid.objects)[8][9]->GetColliShape().AddPoint(19,77);
	(*m_grid.objects)[8][9]->GetColliShape().AddPoint(122,128);
	(*m_grid.objects)[8][9]->GetColliShape().AddPoint(111,133);
	(*m_grid.objects)[8][9]->GetColliShape().AddPoint(0,77);

	AddElement( (*m_grid.objects)[8][9], 8, 9, true, "Comptoir2");

	//PutActorOnTile((*m_grid.objects)[8][9], 8, 9);
	//m_displayables.push_back( (*m_grid.objects)[8][9] );

	/*(*m_grid.objects)[9][11] = new Object();
	(*m_grid.objects)[9][11]->SetSprite( ImgManager::GetImage("Map/Objects/Welcome/1.png") );
	(*m_grid.objects)[9][11]->SetOrigin(15,53);*/

	CollisionsPtr pCollisions( new Collisions(shape) );
	m_grid.collisions.swap(pCollisions);

	LOG( "Initializing done" )

	//--------------------
	// TILES1
	//--------------------
	ifstream in_tiles1( (string("Map/") + file + string("/tiles1.txt")).c_str() , ifstream::in);
	LOG( "Loading " + (string("Map/") + file + string("/tiles1.txt")) )

	if(!in_tiles1.good())
	{
		LOG( "Can't load " + (string("Map/") + file + string("/tiles1.txt")) )
		oslQuit();
	}

	int y = 0;

	while( getline(in_tiles1, line) )
	{
		vector<string> result = StringUtils::Split(line, " ");
		for(int x = 0 ; x < m_grid.xTilesNb ; x++)
		{
			vector<string> coord = StringUtils::Split(result[x], "|");

			if( (coord[0].compare("N") == 0) || (coord[1].compare("N") == 0) )
			{
				(*m_grid.firstMapping)[x][y].first = -1;
				(*m_grid.firstMapping)[x][y].second = -1;
			}
			else
			{
				istringstream iss(coord[0] + " " + coord[1]);
				iss >> (*m_grid.firstMapping)[x][y].first;
				iss >> (*m_grid.firstMapping)[x][y].second;
			}
			
		}

		y++;
	}

	in_tiles1.close();
	LOG( "Loading " + (string("Map/") + file + string("/tiles1.txt done")) )

	//--------------------
	// COLLISIONS
	//--------------------
	ifstream in_colli( (string("Map/") + file + string("/collisions.txt")).c_str() , ifstream::in);
	LOG( "Loading " + (string("Map/") + file + string("/collisions.txt")) )

	if(!in_colli.good())
	{
		LOG( "Can't load " + (string("Map/") + file + string("/collisions.txt")) )
		oslQuit();
	}

	y = 0;

	while( getline(in_colli, line) )
	{
		int spaceIndex = 0;
		string subline, endOfLine = line;

		for(int x = 0 ; x < m_grid.xTilesNb ; x++)
		{

			subline = endOfLine;

			spaceIndex = endOfLine.find_first_of(" ", spaceIndex);
			subline = endOfLine.substr(0, spaceIndex);
			endOfLine = endOfLine.substr(spaceIndex+1);

			if(subline.compare("1") == 0)
				(*m_grid.collisions)[x][y] = 1;
			else
				(*m_grid.collisions)[x][y] = 0;

		}

		y++;
	}

	in_colli.close();
	LOG( "Loading " + (string("Map/") + file + string("/collisions.txt done")) )
	LOG( "Loading map done" )
}

void FieldMap::AddElement(FieldMapElement* e, float x, float y, bool catchToGrid, string id)
{
	if(catchToGrid)
		PutActorOnTile(e, (int) x, (int) y);
	else
		e->SetPosition(x,y);
		
	m_displayables.push_back(e);

	if(id.size() != 0)
		m_elements_ids[e] = id;

	e->SetFieldMap(this);
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// ACTOR HANDLE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void FieldMap::ActorHandle()
{
	bool leftPressed = false;
	bool rightPressed = false;
	bool upPressed = false;
	bool downPressed = false;

	OSL_CONTROLLER* k = oslReadKeys();

	if(k->held.left) leftPressed = true;
	if(k->held.right) rightPressed = true;
	if(k->held.up) upPressed = true;
	if(k->held.down) downPressed = true;

	if(downPressed) {
		if(leftPressed)
			MoveActor(m_actor, AM_DOWN_LEFT);
		else if(rightPressed)
			MoveActor(m_actor, AM_DOWN_RIGHT);
		else
			MoveActor(m_actor, AM_DOWN);
	}
	else if(upPressed) {
		if(leftPressed)
			MoveActor(m_actor, AM_UP_LEFT);
		else if(rightPressed)
			MoveActor(m_actor, AM_UP_RIGHT);
		else
			MoveActor(m_actor, AM_UP);
	}

	else if(leftPressed) {
		MoveActor(m_actor, AM_LEFT);
	}
	else if(rightPressed) {
		MoveActor(m_actor, AM_RIGHT);
	}

	else if(!upPressed && !downPressed && !leftPressed && !rightPressed) {
		if(m_actor->GetDirection() == Actor::UP)
			m_actor->SetState(Actor::FIELD_STANDING_UP);
		else if(m_actor->GetDirection() == Actor::RIGHT)
			m_actor->SetState(Actor::FIELD_STANDING_RIGHT);
		else if(m_actor->GetDirection() == Actor::LEFT)
			m_actor->SetState(Actor::FIELD_STANDING_LEFT);
		else if(m_actor->GetDirection() == Actor::DOWN)
			m_actor->SetState(Actor::FIELD_STANDING_DOWN);
		else if(m_actor->GetDirection() == Actor::DOWN_LEFT)
			m_actor->SetState(Actor::FIELD_STANDING_DOWN_LEFT);
		else if(m_actor->GetDirection() == Actor::DOWN_RIGHT)
			m_actor->SetState(Actor::FIELD_STANDING_DOWN_RIGHT);
		else if(m_actor->GetDirection() == Actor::UP_LEFT)
			m_actor->SetState(Actor::FIELD_STANDING_UP_LEFT);
		else if(m_actor->GetDirection() == Actor::UP_RIGHT)
			m_actor->SetState(Actor::FIELD_STANDING_UP_RIGHT);
	}

}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// MOVE ACTOR
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void FieldMap::MoveActor(Actor* actor, ActorMoving move)
{

	float propotion = 0.5;
	float vitesse = 128;
	float v_ht= vitesse / (propotion +1);
	float v_vt = vitesse - v_ht;

	float x = 0, y = 0;
	Vector2f v;
	float fps = Variables::GetFPS();
	//float fps = (float)1000/ (float)App.GetFrameTime(); 
	switch(move)
	{
		
		case AM_DOWN_LEFT :
			actor->SetState(Actor::FIELD_RUN_DOWN_LEFT);
			actor->SetDirection(Actor::DOWN_LEFT);

			//x = -((-m_grid.xVector.x) * vitesse) / fps ;
			//y = -((-m_grid.xVector.y) * vitesse) / fps ;
			x = -v_ht / fps;
			y = v_vt / fps;

			actor->Move(x, y);
			break;

		case AM_DOWN_RIGHT :
			actor->SetState(Actor::FIELD_RUN_DOWN_RIGHT);
			actor->SetDirection(Actor::DOWN_RIGHT);

			//x = -((-m_grid.yVector.x) * vitesse) / fps;
			//y = -((-m_grid.yVector.y) * vitesse) / fps;

			x = v_ht / fps;
			y = v_vt / fps;

			actor->Move(x, y);
			break;

		case AM_UP_LEFT :
			actor->SetState(Actor::FIELD_RUN_UP_LEFT);
			actor->SetDirection(Actor::UP_LEFT);

			//x = -((m_grid.yVector.x) * vitesse) / fps;
			//y = -((m_grid.yVector.y) * vitesse) / fps;

			x = -v_ht / fps;
			y = -v_vt / fps;

			actor->Move(x, y);
			break;

		case AM_UP_RIGHT :
			actor->SetState(Actor::FIELD_RUN_UP_RIGHT);
			actor->SetDirection(Actor::UP_RIGHT);

			//x = -((m_grid.xVector.x) * vitesse) / fps;
			//y = -((m_grid.xVector.y) * vitesse) / fps;

			x = v_ht / fps;
			y = -v_vt / fps;

			actor->Move(x, y);
			break;

		case AM_LEFT :
			actor->SetState(Actor::FIELD_RUN_LEFT);
			actor->SetDirection(Actor::LEFT);
			
			//x = -v_ht / fps;
			x = (-vitesse * (64.0f / newDiag) ) / fps;

			actor->Move(x, y);
			break;

		case AM_RIGHT :
			actor->SetState(Actor::FIELD_RUN_RIGHT);
			actor->SetDirection(Actor::RIGHT);
			 
			//x = v_ht / fps;
			x = (vitesse * (64.0f / newDiag)) / fps;

			actor->Move(x, y);
			break;

		case AM_UP :
			actor->SetState(Actor::FIELD_RUN_UP);
			actor->SetDirection(Actor::UP);
			
			//y = -v_vt / fps;
			y = (-vitesse * (32.0f / newDiag)) / fps;

			actor->Move(x, y);
			break;

		case AM_DOWN :
			actor->SetState(Actor::FIELD_RUN_DOWN);
			actor->SetDirection(Actor::DOWN);

			//y = v_vt / fps;
			y = (vitesse * (32.0f / newDiag)) / fps;

			actor->Move(x, y);
			break;
	}

	// collisions avec le dÈcor ?
	// on regarde si la surface reprÈsentant le perso est en collision avec le dÈcor
	Shape s = actor->GetColliShape();
	
	for(int i = 0 ; i < s.GetPointsCount() ; i++)
	{
		v = GetActorTile(actor->GetPosition().x + s.GetPointPosition(i).x, actor->GetPosition().y + s.GetPointPosition(i).y);

		if( (v.x < 0) 
			|| (v.x >= m_grid.xTilesNb) 
			|| (v.y < 0) 
			|| (v.y >= m_grid.yTilesNb) 
			|| ( !isWalkable(v.x,v.y) ) 
			) { actor->Move(-x,-y); break; }

	}

	/*for(int j(0) ; j < m_grid.xTilesNb ; j++)
		for(int k(0) ; k < m_grid.yTilesNb ; k++)
		{
			if( (*m_grid.objects)[j][k] )
				while(actor->Collide( ( (*m_grid.objects)[j][k] ) ) ) // while plutot que if pour Èviter de rester figÈ, ce cas arrive rarement mais peut arriver.
				{
					actor->Move(-x,-y);
					break;
				}
		}*/

	vector<FieldMapElement*>::iterator it;
	for(it = m_displayables.begin() ; it != m_displayables.end() ; ++it)
	{
		if ( (*it) == actor ) continue;
		while(actor->Collide(*it))
		{
			actor->Move(-x, -y);
			break;
		}
	}
		
	

}

void FieldMap::PutActorOnTile(Displayable* actor, int x, int y)
{
	int _x = m_grid.origin.x + x * m_grid.xVector.x + y * m_grid.yVector.x;
	int _y = m_grid.origin.y + x * m_grid.xVector.y + y * m_grid.yVector.y;

	actor->SetPosition(_x,_y);
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DISPLAY ISO MAP
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void FieldMap::DisplayIsoMap()
{

	//Calcul de la sous map potentiellement affichable
	Vector2f posCenter = GetActorTile(m_view.GetCenter().x, m_view.GetCenter().y);
	//Vector2f posCenter = GetActorTile(m_actor->GetPosition().x, m_actor->GetPosition().y);

	Vector2f left = GetActorTile(m_view.GetCenter().x + m_view.GetSize().x / 2, m_view.GetCenter().y - m_view.GetSize().y / 2 );
	if(left.x < 0) left.x = 0;
	if(left.y < 0) left.y = 0;

	Vector2f right = GetActorTile(m_view.GetCenter().x - m_view.GetSize().x / 2, m_view.GetCenter().y + m_view.GetSize().y / 2);
	if(right.x >= m_grid.xTilesNb) right.x = m_grid.xTilesNb - 1;
	if(right.y >= m_grid.yTilesNb) right.y = m_grid.yTilesNb - 1;

	Vector2f top = GetActorTile(m_view.GetCenter().x - m_view.GetSize().x / 2, m_view.GetCenter().y - m_view.GetSize().y / 2);
	if(top.x < 0) top.x = 0;
	if(top.y < 0) top.y = 0;

	Vector2f bottom = GetActorTile(m_view.GetCenter().x + m_view.GetSize().x / 2, m_view.GetCenter().y + m_view.GetSize().y / 2);
	if(bottom.x >= m_grid.xTilesNb) bottom.x = m_grid.xTilesNb - 1;
	if(bottom.y >= m_grid.yTilesNb) bottom.y = m_grid.yTilesNb - 1;

	InitZOrders(left, right, top, bottom);

	//Calcul de la position de la camÈra
	Vector2f centerCamera = m_view.GetCenter();
	int leftCamera(centerCamera.x - m_view.GetSize().x / 2);
	int topCamera(centerCamera.y - m_view.GetSize().y / 2);

	FloatRect rectCamera(leftCamera, topCamera, 960, 720);
	
	//Affichage de la carte en utilisant le clipping
	Vector2f p1,p2,p3,p4;

	Vector2f posActor = GetActorTile(m_actor->GetPosition().x, m_actor->GetPosition().y);
	int actorZOrder = posActor.x + posActor.y;

	std::map<int, std::vector<Vector2i> >::iterator it;
	vector<Vector2i> v;



	/*int minx,maxx,miny,maxy;
	minx = m_grid.xScroll / m_grid.tileXSize;
	miny = m_grid.yScroll / m_grid.tileYSize;
	maxx = (m_grid.xScroll + 480) / m_grid.tileXSize;
	maxy = (m_grid.yScroll + 272) / m_grid.tileYSize;

	ostringstream ap_oss(ostringstream::out);
	ap_oss << minx  << " " << maxx << " " << miny << " " << maxy;

	oslSetTextColor(RGBA(255,0,0,255));
	oslSetBkColor(RGBA(0,0,0,0));
	oslPrintf_xy(5,40,ap_oss.str().c_str());*/

	/*for(int i = minx ; i <= maxx ; i++)
	{
		for(int j = miny ; j <= maxy ; j++)
		{
			//DrawSpriteCell(i, j, (*m_grid.firstMapping)[i][j].first, (*m_grid.firstMapping)[i][j].second);
		}
	}*/

	//---------------PremiËre couche-----------------
	
	for(it = m_zOrders.begin() ; it != m_zOrders.end() ; ++it)
	{
		v = (*it).second;
		vector<Vector2i>::iterator it2;
		for(it2 = v.begin() ; it2 != v.end() ; ++it2)
		{
			//CrÈation du rectangle englobant du tile que l'on veut afficher
			p1.x = m_grid.origin.x + (*it2).x * m_grid.xVector.x + (*it2).y * m_grid.yVector.x;
			p1.y = m_grid.origin.y + (*it2).x * m_grid.xVector.y + (*it2).y * m_grid.yVector.y;

			p2.x = p1.x - m_grid.xVector.x;
			p2.y = p1.y - m_grid.xVector.y;

			p3.x = p2.x - m_grid.yVector.x;
			p3.y = p2.y - m_grid.yVector.y;

			p4.x = p1.x - m_grid.yVector.x;
			p4.y = p1.y - m_grid.yVector.y;
			
			FloatRect tileRect(p4.x, p3.y, p2.x - p4.x, p1.y - p3.y);
			
			//Si ce rectangle n'est pas ÅEl'intÈrieur de la camÈra, alors c'est que le tile n'est pas visible, on l'ignore
			if(!tileRect.Intersects(rectCamera)) continue;
			
			DrawSpriteCell((*it2).x, (*it2).y, (*m_grid.firstMapping)[(*it2).x][(*it2).y].first, (*m_grid.firstMapping)[(*it2).x][(*it2).y].second);
		}
			
	}

	//---------------DeuxiËme couche-----------------
	while(!m_d_zOrders.empty())
	{
		/*vector<Displayable*> v = m_d_zOrder_map[m_d_zOrders.top()];
		vector<Displayable*>::iterator it;
		for(it = v.begin() ; it != v.end() ; ++it)
		{
			Vector2f pos = (*it)->GetPosition();
			DrawObject(pos.x, pos.y, *it);
		}*/

		Displayable* d = m_d_zOrders.top();
		//DrawObject(d->GetPosition().x, d->GetPosition().y, d);
		d->Display(-m_grid.xScroll, -m_grid.yScroll);
		m_d_zOrders.pop();
	}

	/*for(it = m_zOrders.begin() ; it != m_zOrders.end() ; ++it)
	{
		if((*it).first > actorZOrder) break;

		v = (*it).second;
		vector<Vector2i>::iterator it2;
		for(it2 = v.begin() ; it2 != v.end() ; ++it2)
			if( ((*m_grid.objects)[(*it2).x][(*it2).y]) )
			{
				DrawObject((*it2).x, (*it2).y, (*m_grid.objects)[(*it2).x][(*it2).y]);

				//collisions des objets
				/*Vector2f v = ((*m_grid.objects)[(*it2).x][(*it2).y])->GetPosition();
				v.x -= ((*m_grid.objects)[(*it2).x][(*it2).y])->GetOrigin().x;
				v.y -= ((*m_grid.objects)[(*it2).x][(*it2).y])->GetOrigin().y;
				((*m_grid.objects)[(*it2).x][(*it2).y])->GetColliShape().SetPosition( v );
				//#ifdef _DEBUG
				((*m_grid.objects)[(*it2).x][(*it2).y])->GetColliShape().Display();
				//oslDrawImage( ((*m_grid.objects)[(*it2).x][(*it2).y])->GetColliShape() );
				//#endif
			}
	}

	m_actor->Display(-m_grid.xScroll, -m_grid.yScroll); // offset pour centrer a l'Ècran

	for(it = it ; it != m_zOrders.end() ; ++it)
	{
		v = (*it).second;
		vector<Vector2i>::iterator it2;
		for(it2 = v.begin() ; it2 != v.end() ; ++it2)
			if( ((*m_grid.objects)[(*it2).x][(*it2).y]) )
			{
				DrawObject((*it2).x, (*it2).y, (*m_grid.objects)[(*it2).x][(*it2).y]);
				
				//collisions des objets
				/*Vector2f v = ((*m_grid.objects)[(*it2).x][(*it2).y])->GetPosition();
				v.x -= ((*m_grid.objects)[(*it2).x][(*it2).y])->GetOrigin().x;
				v.y -= ((*m_grid.objects)[(*it2).x][(*it2).y])->GetOrigin().y;
				((*m_grid.objects)[(*it2).x][(*it2).y])->GetColliShape().SetPosition( v );
				//#ifdef _DEBUG
				((*m_grid.objects)[(*it2).x][(*it2).y])->GetColliShape().Display();
				//oslDrawImage( ((*m_grid.objects)[(*it2).x][(*it2).y])->GetColliShape() );
				//#endif
			}
	}*/

	/*vector<Actor*>::iterator it_actors;
	for(it_actors = m_actors.begin() ; it_actors != m_actors.end() ; ++it_actors)
		(*it_actors)->Display(-m_grid.xScroll, -m_grid.yScroll);*/

	//lignes horizontales
	/*for(int j = 0 ; j <= m_grid.yTilesNb ; j ++)
	{
		p1.x = m_grid.origin.x + j * m_grid.yVector.x;
		p1.y = m_grid.origin.y + j * m_grid.yVector.y;

		p2.x = m_grid.origin.x + (m_grid.xTilesNb) * m_grid.xVector.x + j * m_grid.yVector.x;
		p2.y = m_grid.origin.y + (m_grid.xTilesNb) * m_grid.xVector.y + j * m_grid.yVector.y;

		/*p1.x = m_grid.origin.x + j * m_grid.yVector.x - m_grid.xScroll;
		p1.y = m_grid.origin.y + j * m_grid.yVector.y - m_grid.yScroll;

		p2.x = m_grid.origin.x + (m_grid.xTilesNb) * m_grid.xVector.x + j * m_grid.yVector.x - m_grid.xScroll;
		p2.y = m_grid.origin.y + (m_grid.xTilesNb) * m_grid.xVector.y + j * m_grid.yVector.y - m_grid.yScroll;

		oslDrawLine(p1.x, p1.y - m_grid.tileYSize, p2.x, p2.y - m_grid.tileYSize, RGB(255, 255, 255));
		
	}

	//lignes verticales
	for(int i = 0 ; i <= m_grid.xTilesNb ; i ++)
	{
		p1.x = m_grid.origin.x + i * m_grid.xVector.x;
		p1.y = m_grid.origin.y + i * m_grid.xVector.y;
		
		p2.x = m_grid.origin.x + i * m_grid.xVector.x + (m_grid.yTilesNb) * m_grid.yVector.x;
		p2.y = m_grid.origin.y + i * m_grid.xVector.y + (m_grid.yTilesNb) * m_grid.yVector.y;

		/*p1.x = m_grid.origin.x + i * m_grid.xVector.x - m_grid.xScroll;
		p1.y = m_grid.origin.y + i * m_grid.xVector.y - m_grid.yScroll;
		
		p2.x = m_grid.origin.x + i * m_grid.xVector.x + (m_grid.yTilesNb) * m_grid.yVector.x - m_grid.xScroll;
		p2.y = m_grid.origin.y + i * m_grid.xVector.y + (m_grid.yTilesNb) * m_grid.yVector.y - m_grid.yScroll;

		oslDrawLine(p1.x, p1.y - m_grid.tileYSize, p2.x, p2.y - m_grid.tileYSize, RGB(255, 255, 255));
		
	}*/

}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// GET ACTOR TILE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
Vector2f FieldMap::GetActorTile(float x, float y)
{
	Vector2f position;

	float posx = x - m_grid.origin.x;
	float posy = y - m_grid.origin.y;

	position.y = (float)  (floor((posy / m_grid.tileYSize) + (posx / m_grid.tileXSize)) + 1);
    position.x = (float)  (ceil((posy / m_grid.tileYSize) - (posx / m_grid.tileXSize)));

	return position;
}

Vector2f FieldMap::IsoToScreen(float x, float y)
{
	Vector2f screenCord;
 
	screenCord.x = m_grid.origin.x + (int)((x - y) * m_grid.tileXSize / 2.0f);
	screenCord.y = m_grid.origin.y + (int)((x + y) * m_grid.tileYSize / 2.0f);
 
    return screenCord;
}

void FieldMap::DrawColoredTile(float x, float y, unsigned int color)
{/*
	sf::Vector2f p1,p2,p3,p4;

	p1.x = m_grid.origin.x + x * m_grid.xVector.x + y * m_grid.yVector.x;
	p1.y = m_grid.origin.y + x * m_grid.xVector.y + y * m_grid.yVector.y;

	p2.x = p1.x - m_grid.xVector.x;
	p2.y = p1.y - m_grid.xVector.y;

	p3.x = p2.x - m_grid.yVector.x;
	p3.y = p2.y - m_grid.yVector.y;

	p4.x = p1.x - m_grid.yVector.x;
	p4.y = p1.y - m_grid.yVector.y;

	sf::Shape s;
	s.AddPoint(p1);
	s.AddPoint(p2);
	s.AddPoint(p3);
	s.AddPoint(p4);
	s.SetColor(c);
	App.Draw(s);*/
}

void FieldMap::DrawObject(float x, float y, Displayable* o)
{

	float posx = m_grid.origin.x + x * m_grid.xVector.x + y * m_grid.yVector.x /*- o->GetOrigin().x*/;
	float posy = m_grid.origin.y + x * m_grid.xVector.y + y * m_grid.yVector.y /*- o->GetOrigin().y*/;

	o->SetPosition(posx, posy);
	//o->SetOffset(-m_grid.xScroll, -m_grid.yScroll);
	o->Display(-m_grid.xScroll, -m_grid.yScroll);

}

bool FieldMap::isWalkable(float x, float y)
{
	if( (*m_grid.collisions)[x][y] == 1 ) return false;

	return true;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DRAW SPRITE CELL
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void FieldMap::DrawSpriteCell(float x, float y, int spriteCol, int spriteLine)
{
	/*if( (spriteCol == -1) || (spriteLine == -1) ) return;

	sf::Vector2f p1;

	p1.x = m_grid.origin.x + x * m_grid.xVector.x + y * m_grid.yVector.x;
	p1.y = m_grid.origin.y + x * m_grid.xVector.y + y * m_grid.yVector.y;

	m_grid.tileset.SetSubRect(sf::IntRect( spriteCol * m_grid.tileXSize, spriteLine * (m_grid.tileYSize + 8), m_grid.tileXSize, m_grid.tileYSize + 8));
	m_grid.tileset.SetPosition(p1.x, p1.y);
	m_grid.tileset.SetOrigin(32,32);
	App.Draw(m_grid.tileset);*/

	if( (spriteCol == -1) || (spriteLine == -1) ) return;

	//int frame = spriteCol * (m_grid.tileset->sizeX / m_grid.tileXSize) + spriteLine;
	if(!m_grid.tileset) return;

	m_grid.tileset->x = (m_grid.origin.x + x * m_grid.xVector.x + y * m_grid.yVector.x) - m_grid.tileXSize / 2 - m_grid.xScroll;
	m_grid.tileset->y = (m_grid.origin.y + x * m_grid.xVector.y + y * m_grid.yVector.y) - m_grid.tileYSize - m_grid.yScroll;
	//oslSetImageFrame(m_grid.tileset, frame);
	oslSetImageTileSize(m_grid.tileset, spriteCol * m_grid.tileXSize, spriteLine * (m_grid.tileYSize + 8), m_grid.tileXSize, m_grid.tileYSize + 8);
    oslDrawImage(m_grid.tileset);

}

void FieldMap::InitZOrders(Vector2f& left, Vector2f& right, Vector2f& top, Vector2f& bottom)
{

	m_zOrders.clear();

	for(int i(left.x)  ; i <= right.x ;  i++)
	{
		for(int j(top.y) ; j <= bottom.y ; j++)
		{
			//if( (*m_grid.objects)[i][j] )
				m_zOrders[i + j].push_back(Vector2i(i,j));
			
		}
	}

	while(!m_d_zOrders.empty())
		m_d_zOrders.pop();

	vector<FieldMapElement*>::iterator it;
	for(it = m_displayables.begin() ; it != m_displayables.end() ; ++it)
	{
		Vector2f v = (*it)->GetPosition();
		//if( (v.x < left.x) || (v.x > right.x) || (v.y < top.y) || (v.y > bottom.y) ) continue;

		m_d_zOrders.push(*it);
	}

	m_d_zOrders.push(m_actor);

}
