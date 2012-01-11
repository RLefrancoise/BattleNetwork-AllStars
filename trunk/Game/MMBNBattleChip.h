#ifndef MMBNBATTLECHIP_H
#define MMBNBATTLECHIP_H

#include <oslib/oslib.h>
#include <map>

#include "Displayable.h"
#include "GameSystem.h"

class MMBNBattleChip : public Displayable
{

	public:
		
		static void LoadMaps();

		virtual ~MMBNBattleChip()									;

		static MMBNBattleChip* Load(std::string name)				;

		void Display(float offX = 0, float offY = 0)				;
		void Move(float x, float y)									;
		void SetPosition(float x, float y)							;

		Vector2f& GetPosition()										;
		OSL_IMAGE* GetImage()										;
		OSL_IMAGE* GetIcon()										;
		char GetLetter()											;
		GameSystem::ChipType GetType()								;
		std::string GetDescription()								;
		
		void SetExtendedDisplay(bool extended)						;

	private:

		MMBNBattleChip()											;

		std::string					m_name							;
		std::string					m_description					;
		GameSystem::ChipElement		m_element						;
		int							m_power							;
		char						m_letter						;
		OSL_IMAGE*					m_pic							;
		OSL_IMAGE*					m_icon							;
		GameSystem::DamageType		m_damage_type					;
		GameSystem::ChipType		m_chip_type						;
		GameSystem::TargetType		m_target_type					;

		bool						m_pause_when_used				;

		Vector2f					m_position						;

		bool						m_extended_display				;


		static std::map<std::string, GameSystem::ChipElement> m_element_map;
		static std::map<std::string, GameSystem::DamageType> m_damage_map;
		static std::map<std::string, GameSystem::TargetType> m_target_map;
		static std::map<std::string, GameSystem::ChipType> m_type_map;

		void DisplayExtended(float offX = 0, float offY = 0);
		void DisplaySimple(float offX = 0, float offY = 0);

};

#endif
