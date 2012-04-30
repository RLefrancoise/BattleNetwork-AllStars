#ifndef MMBN_BATTLEMAP_H
#define MMBN_BATTLEMAP_H

#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>
#include <oslib/oslib.h>
#include <vector>
#include <string>

#include "Screens.hpp"
#include "Screen.hpp"
#include "Animation.hpp"
#include "Utils.h"
#include "MMBNBattleActor.h"
#include "MMBNBattleAttack.h"
#include "MMBNBattleChip.h"
#include "MMBNString.h"

class MMBNBattleIA;

class MMBNPanelGrid
{

	public:

		MMBNPanelGrid();
		~MMBNPanelGrid();

		void	Update(OSL_CONTROLLER* k)					;
		void	Display()					;

		void	PutActorOnPanel(MMBNBattleActor* actor, unsigned int x, unsigned int y);
		Vector2i GetActorPanel(MMBNBattleActor* actor);
		void	MoveActor(MMBNBattleActor* actor, int offX, int offY);

		void	PutProjectileOnPanel(BattleProjectilePtr proj, unsigned int x, unsigned int y);
		Vector2i GetProjectilePanel(BattleProjectilePtr proj);
		
		GameSystem::PanelType GetPanelType(unsigned int x, unsigned int y);
		GameSystem::PanelTeam GetPanelTeam(unsigned int x, unsigned int y);

		bool	IsInsideGrid(int x, int y);
		bool	IsWalkable(unsigned int x, unsigned int y);

		MMBNBattleActor*	GetActor() const;
		const std::vector<MMBNBattleActor*>& GetEnemies() const;

		void	ActorHandle(OSL_CONTROLLER* k);

		bool	BattleIsOver();
		bool	ActorIsDead();

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		
		void AttackEnemies();
		bool AttackActor(MMBNBattleActor* launcher, MMBNBattleActor* target);
		bool UseSkillOnActor(MMBNBattleActor* launcher, MMBNBattleActor* target, const MMBNBattleAttackPtr& skill);

	private :
		
		typedef boost::multi_array< GameSystem::PanelType, 2 >					Panels			;
		typedef boost::shared_ptr<Panels>										PanelsPtr		;
		typedef boost::multi_array< GameSystem::PanelTeam, 2 >					PanelsTeam		;
		typedef boost::shared_ptr<PanelsTeam>									PanelsTeamPtr	;

		PanelsPtr		m_panels		;
		PanelsTeamPtr	m_panels_team	;

		OSL_IMAGE* m_panel_pictures[GameSystem::PANELS_TEAM_NB];
		AnimationPtr m_panel_animations[GameSystem::PANEL_TYPES_NB];

		std::vector<AnimationPtr> m_attack_impact		;
		int 		 m_current_impact_anim;
		bool		 m_display_attack_impact;
		
		unsigned int m_width				;
		unsigned int m_height				;

		unsigned int m_x_inc				;
		unsigned int m_y_inc				;

		unsigned int m_x_map				;
		unsigned int m_y_map				;

		MMBNBattleActor* m_actor				;
		std::vector<MMBNBattleActor*> m_enemies	;
		std::vector<BattleProjectilePtr> m_projectiles;
		std::vector<MMBNBattleIA*> m_ia			;
		
		std::vector<Vector2i>	m_targeted_panels;
		
		bool	m_actor_is_dead				;
		bool 	m_can_attack				;
		
		void 					AddProjectile(BattleProjectilePtr proj)												;
		
		MMBNBattleActor* 		GetEnemyOnPanel(unsigned int x, unsigned int y)										;
		
		std::vector<Vector2i> 	GetTargetedPanels(MMBNBattleActor* launcher, const AttackInfoPtr& attack_info)		;
		bool					IsInRange(Vector2i &target, std::vector<Vector2i> &range)							;
		
		void					ColorPanel(unsigned int x, unsigned int y, OSL_COLOR color, int border = 0)			;
		bool					IsInProjectileRange(BattleProjectilePtr proj, unsigned int posx, unsigned int posy)	;
		void					GetProjectileRangePanels(BattleProjectilePtr proj, std::vector<Vector2i>& targeted_panels);
		
		void					AddPanelToTargetedPanels(Vector2i& v)												;

	
};

class MMBNEmotionDisplay : public Displayable
{
	private :
		enum Emotions
		{
			NORMAL,
			EMOTIONS_NB
		};

	OSL_IMAGE* m_emotions[EMOTIONS_NB];
	Emotions m_current_emotion;

	MMBNBattleActor* m_actor;

	Vector2f m_position;

	public:
		MMBNEmotionDisplay();
		MMBNEmotionDisplay(MMBNBattleActor* mmbnba);
		~MMBNEmotionDisplay();

		void Display(float offX = 0, float offY = 0)		;
		void Move(float x, float y)							;
		void SetPosition(float x, float y)					;
		const Vector2f& GetPosition() const					;




};

class MMBNLifeBar : public Displayable
{
	private :
		Vector2f			m_position						;
		Vector2f			m_size							;
		MMBNBattleActor*	m_actor							;

		MMBNFont*			m_life_font						;

	public:
		MMBNLifeBar()										;
		MMBNLifeBar(MMBNBattleActor* mmbnba)				;
		~MMBNLifeBar()										;

		void Display(float offX = 0, float offY = 0)		;
		void Move(float x, float y)							;
		void SetPosition(float x, float y)					;
		const Vector2f& GetPosition() const					;
		void SetSize(unsigned int w, unsigned int h)		;
		Vector2f& GetSize()									;
		void SetActor(MMBNBattleActor* mmbnba)				;
		MMBNBattleActor* GetActor()							;

};

class MMBNCustomGauge : public Displayable
{
	private :

		enum CustomGaugeState
		{
			RISING,
			FULL
		};

		Vector2f			m_position						;
		Vector2f			m_size							;

		OSL_IMAGE*			m_empty							;
		OSL_IMAGE*			m_gauge							;
		AnimationPtr		m_full							;

		Timer				m_timer							;
		float				m_time_limit					;
		float				m_per							;

		CustomGaugeState	m_state							;

	public:
		MMBNCustomGauge()									;
		~MMBNCustomGauge()									;
		void Display(float offX = 0, float offY = 0)		;
		void Move(float x, float y)							;
		void SetPosition(float x, float y)					;
		const Vector2f& GetPosition() const					;
		void Update()										;
		void Reset()										;
		void Pause()										;
		void Unpause()										;
		void SetFillingDuration(float time)					;
		Vector2f& GetSize()									;
		bool IsFull()										;
};

class MMBNBattleChipSelector : public Displayable
{
	private:

		enum CursorPosition
		{
			CHIP,
			OK,
			SHUFFLE
		};

		Vector2f			m_position						;
		Vector2f			m_size							;

		OSL_IMAGE*			m_selector						;
		AnimationPtr		m_cursor						;
		AnimationPtr		m_cursor2						;
		AnimationPtr		m_cursor3						;
		OSL_IMAGE*			m_normal_chip					;
		OSL_IMAGE*			m_mega_chip						;
		OSL_IMAGE*			m_giga_chip						;
		OSL_IMAGE*			m_dark_chip						;

		std::vector<MMBNBattleChip*> m_current_chips		;
		std::vector<unsigned int> m_selected_chips			;
		//MMBNBattleChip*		m_current_chips[8]				;
		//int					m_selected_chips[5]				;
		Vector2i			m_chips_position[8]				;
		Vector2i			m_letters_position[8]			;

		CursorPosition		m_current_position				;
		unsigned int		m_selected_chip					;

		bool				m_ok_pressed					;

		bool				m_show_description				;
		
		static const unsigned int ICON_WIDTH = 21			;
		static const unsigned int ICON_HEIGHT = 21			;
		static const unsigned int MAX_CHIPS = 8				;
		static const unsigned int MAX_SELECTED_CHIPS = 5	;

	public:
		MMBNBattleChipSelector()							;
		~MMBNBattleChipSelector()							;
		void Update(OSL_CONTROLLER* k)						;
		void Display(float offX = 0, float offY = 0)		;
		void Move(float x, float y)							;
		void SetPosition(float x, float y)					;
		const Vector2f& GetPosition() const					;
		Vector2f& GetSize()									;

		bool OkPressed()									;

		void ResetCursor()									;
};

class MMBNBattleMap : public Screen
{
	private:

		//HUD elements
		MMBNPanelGrid*			m_grid						;
		AnimationPtr			m_bg						;
		MMBNLifeBar*			m_lifeBar					;
		MMBNEmotionDisplay*		m_emotion					;
		MMBNCustomGauge*		m_custom_gauge				;
		MMBNBattleChipSelector*	m_chip_selector				;
		OSL_IMAGE*				m_enemy_display_edge		;
		AnimationPtr			m_enemy_deleted				;
		AnimationPtr			m_battle_start				;
		AnimationPtr			m_you_lose					;
		
		bool					m_select_chip				;

		#ifdef _DEBUG
		//debug
		bool					m_display_debug_info		;
		#endif
		
		//battle infos
		bool					m_battle_is_over;			;
		Timer					m_battle_timer				;
		MMBNString				m_battle_time_string		;



		void DisplayBattleTime(int x, int y);
		void GetBattleTime(unsigned int* min, unsigned int* sec, unsigned int* centi);

		int		Update()					;
		void	Display()					;

		//gestion des évènements en relation avec le héros
		void ActorHandle(OSL_CONTROLLER* k)	;

	public:

		MMBNBattleMap()						;
		~MMBNBattleMap()					;

		int		Run()						;
		void	Initialize()				;
		void	Destroy()					;
};

class MMBNBattleIA
{
	private:
		MMBNPanelGrid* m_map;
		MMBNBattleActor* m_actor;
		GameSystem::PanelTeam m_actor_team;
		
		int m_current_actor_attack; //index of the current attack the ia is using
		
		Timer m_moving_timer;
		
		Timer m_attack_timer;
		Timer m_move_after_attack_timer;
		
		Timer m_move_after_skill_timer;
		
		bool m_can_attack;
		bool m_attack_done;
		
		bool m_is_damaged;
		bool m_move_after_damage;
		
		bool m_is_attacking;
		bool m_move_after_attack;
		
		bool m_is_using_skill;
		bool m_move_after_skill;
		
		void Move();
		bool Attack();
		bool Skill(unsigned int skill_nb);
		
		unsigned int m_current_skill;
		bool m_skill_choosed;

	public:
		MMBNBattleIA();
		MMBNBattleIA(MMBNPanelGrid* m, MMBNBattleActor* a, GameSystem::PanelTeam t);
		~MMBNBattleIA();
		void Update();

};

#endif
