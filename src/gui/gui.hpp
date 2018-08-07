#pragma once

#include <functional>
#include "ext/ext.hpp"
#include "front/front.hpp"
#include "front/render.hpp"
#include "go/go.hpp"
#include "go/sfg-parser.hpp"


/**
Display GO board;
	board
	score
	player
	turn
	winner


Mode 1) Play
	Reset game;
	Make move;


Mode 2) Replay
	Load sfg file;
	Show moves with space;


space - load next move from sfg




**/

namespace gui {

	using namespace ext;
	using namespace front;


	RGBA8 const
		CLightGray{180,180,180,255},
		CWhite{255,255,255,255},
		CBlack{0,0,0,255},
		CGray{128,128,128,255},
		CYellow{220,179,92,255},
		CWood{126,50,27,255},
		CLightWood{150,70,40,255},
		CButton{100,100,200,255},
		CText{255,255,255,255},
		CLightGreen{120,255,120,255};

	char const BUTTON_STYLE = 'T';

	int16_t const Ex = 10;
	int16_t const Em = 21;

	using Event = SDL_Event;

	struct Board;
	struct Window;
	struct Panel;

	extern Go::Board game;
	extern Board board;
	extern Window window;
	extern Panel panel;

	extern Go::SFGParser sfg;



	struct Widget
	{
		virtual void update();
		virtual void render();
		virtual bool handle(SDL_Event &e);
	};

	struct Button
	{
		b2s box;
		//v2s border;
		std::string label{"-1"};
		std::function<void()> handler;

		void init(b2s box, std::string label, std::function<void()> const& f);
		void update();
		void render();
		bool handle(SDL_Event &e);
	};





	struct Board
	{
		b2s box;

		// recommended action highlight
		Go::Action hl;

		// text - 8 char per cell
		std::array<char, 8 * Go::RULE_BOARD * Go::RULE_BOARD> cell_text;

		void init(v2s p);
		void render();
		bool handle(SDL_Event &e);

		v2s get_cell_dim() const { return v2s{48,48}; }
		//b2s get_cell_box(Go::Vec p) const;
		void set_cell_text(Go::Vec p, float f);

		v2s get_grid_pos(v2s click_pos) const;
	};



	struct Panel
	{
		b2s box;

		void init(b2s p);
		void render();
		bool handle(SDL_Event &e);
	};


	struct Window
	{
		bool f_stop{false};

		Window() = default;
		~Window();

		void init(char const* path_sfg);
		void render();
		bool handle(SDL_Event &e);
	};






	inline v2s get_ctx_pos(v2s win_pos) {
		return v2s(ext::vmul(v2f(win_pos),
			vdiv(
				v2f(g_renderer.ctx_dim),
				v2f(g_renderer.win_dim)
			)
		));
	}

	inline v2s get_mouse_pos(SDL_Event &e) {
		return get_ctx_pos(v2s(e.button.x, e.button.y));
	}


}
