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
			if( !ImgManager::Exists(string(spritesPath + "/" + oss->str() + "_r.png").c_str()) )
			{
				OSL_IMAGE* rImage = oslCreateImage(Image->sizeX, Image->sizeY, OSL_IN_RAM, OSL_PF_5551);
				ImgManager::CopyImage(Image, rImage);
				oslMirrorImageH(rImage);
				ImgManager::AddImage(string(spritesPath + "/" + oss->str() + "_r.png"), rImage);
				
			}
			
			sprite.SetImage(ImgManager::GetImage(string(spritesPath + "/" + oss->str() + "_r.png")));
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
			else m_currentFrame = m_sprites.size() - 1;
		}
	}
}

void Animation::Stop()
{
	m_running = false;
	m_currentFrame = 0;
}

bool Animation::IsRunning()
{
	return m_running;
}
