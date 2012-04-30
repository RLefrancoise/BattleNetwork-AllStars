#include "Animation.hpp"

#include <sstream>

using namespace std;

Animation::Animation(string spritesPath, std::vector<int> delays, bool reverse, bool loop)
{
	unsigned int i = 1;
	ostringstream* oss = new ostringstream(ostringstream::out);
	(*oss) << i;

	while(i <= delays.size())
	{
		ExtendedSprite sprite;
		sprite.LoadCollisions(i, spritesPath);

		string path = spritesPath + "/" + oss->str() + ".png";
		/*char p[path.size() + 1];
		StringUtils::StringToChar(path, p);
		string rPath = spritesPath + "/" + oss->str() + "_r.png";
		char rp[rPath.size() + 1];
		StringUtils::StringToChar(rPath, rp);*/

		OSL_IMAGE* Image = ImgManager::GetImage(path);

		if( reverse  ) {
			/*if( !ImgManager::Exists(string(spritesPath + "/" + oss->str() + "_r.png").c_str()) )
			{
				OSL_IMAGE* rImage = oslCreateImage(Image->sizeX, Image->sizeY, OSL_IN_RAM, OSL_PF_5551);
				ImgManager::CopyImage(Image, rImage);
				oslMirrorImageH(rImage);
				ImgManager::AddImage(string(spritesPath + "/" + oss->str() + "_r.png"), rImage);
				
				ImgManager::RemoveImage(path);
			}*/
			
			//sprite.SetImage(ImgManager::GetImage(string(spritesPath + "/" + oss->str() + "_r.png")));
			sprite.SetImage(Image);
			sprite.SetReverseOnDisplay(true);
			sprite.ReverseCollisionsBoxes();
			
		}
		else sprite.SetImage(Image);
		m_sprites.push_back(sprite);
		i++;
		delete oss;
		oss = new ostringstream(ostringstream::out);
		(*oss) << i;
	}

	delete oss;

	m_delays = delays;

	m_running = false;
	m_isOver = false;
	m_reverse = reverse;
	m_loop = loop;
	m_currentFrame = 0;
	m_currentTime = 0;

	Start();

}

Animation::~Animation()
{
	
}

ExtendedSprite& Animation::GetCurrentSprite()
{
	return m_sprites[m_currentFrame];
}

void Animation::SetPosition(float x, float y)
{
	for(unsigned int i = 0 ; i < m_sprites.size() ; i++)
		m_sprites[i].SetPosition(x, y);
}

const Vector2f& Animation::GetPosition() const
{
	return m_sprites[0].GetPosition();
}

void Animation::Display(float offX, float offY)
{
	m_sprites[m_currentFrame].Display(offX, offY);
}
		
void Animation::Start()
{
	if(m_running) return;

	m_running = true;
	//gestion clock ici
	m_clock.stop();
	m_clock.start();
	//m_clock.Reset();
}

void Animation::Update()
{
	if(!m_running)
		Start();

	float elapsedTime = m_clock.get_ticks(); //in ms

	if(elapsedTime >= m_delays[m_currentFrame])
	{
		m_clock.stop();
		m_clock.start();
		m_currentFrame++;
		if(m_currentFrame >= m_sprites.size())
		{
			if(m_loop) m_currentFrame = 0; //loop animation
			else { m_currentFrame = m_sprites.size() - 1; m_isOver = true; }
		}
	}
}

void Animation::Stop()
{
	m_running = false;
	m_isOver = false;
	m_currentFrame = 0;
}

bool Animation::IsRunning()
{
	return m_running;
}

bool Animation::IsOver()
{
	return m_isOver;
}

unsigned int Animation::GetCurrentFrame()
{
	return m_currentFrame;
}

void Animation::Reverse()
{
	vector<ExtendedSprite>::iterator it;
	for(it = m_sprites.begin() ; it != m_sprites.end() ; ++it)
	{
		it->IsReversedOnDisplay() ? it->SetReverseOnDisplay(false) : it->SetReverseOnDisplay(true);
		it->ReverseCollisionsBoxes();
	}
	
	m_reverse = !m_reverse;
}

AnimationPtr Animation::Load(std::string spritesPath, bool reverse, bool loop)
{
	vector<int> delays;
	InitDelaysOfAnim(spritesPath, delays);
	return AnimationPtr(new Animation(spritesPath, delays, reverse, loop));
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// INIT DELAYS OF ANIM
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void Animation::InitDelaysOfAnim(string path, vector<int> &delays)
{
	int spritesNb = 0, delay = 0;
	ifstream in_anim( (path + string("/anim.txt")).c_str() , ifstream::in);

	if(!in_anim.good())
	{
		LOG("Impossible de trouver le fichier anim.txt pour " + path);
		oslQuit();
	}

	string line;
	while(getline(in_anim, line))
	{
		if(line.find("sprites=") == 0)
		{
			int equalsIndex = line.find_first_of("=");
			line = line.substr(equalsIndex + 1);
			istringstream iss(line);
			iss >> spritesNb;
		}
		else if(line.find("delays=") == 0)
		{
			int equalsIndex = line.find_first_of("=");
			line = line.substr(equalsIndex + 1);
			int commaIndex = -1;

			for(int i = 0 ; i < spritesNb ; i++)
			{
				commaIndex = line.find_first_of(",");

				string delay_s;
				if(commaIndex != -1) delay_s = line.substr(0, commaIndex);
				else delay_s = line;

				istringstream iss(delay_s);
				iss >> delay;
				delays.push_back(delay);

				line = line.substr(commaIndex + 1);
			}
			
		}
	}

	in_anim.close();
}