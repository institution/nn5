#include "gui.hpp"

namespace gui {

	Board board;
	Panel panel;
	Window window;
	Button reset_button;
	Button pass_button;
	Button resign_button;
	Button next_button;
	Go::Board game;
	Go::SFGParser sfg;




	v2s g_mouse_pos{0,0};
	float g_dt{0};

	// Board -----------------------------------------------------

	void Board::set_cell_text(Go::Vec p, float f)
	{
		auto ind = (p[0] + p[1] * Go::RULE_BOARD) * 8;
		to_string(&cell_text.at(ind), 8, f, 2);
	}

	/*b2s Board::get_text_box() const {
		return b2s(pos + v2s(1, TILE_DIM[0]*3 + 4), v2s(TILE_DIM[1]*3 + 2, 18));
	}*/

	void Board::init(v2s p) {
		game.reset();

		v2s d = get_cell_dim() * int16_t(Go::RULE_BOARD);
		box = b2s(p, d);
		hl = Go::ActionNone;
	}



	struct PlyInfo {
		char letter;
		FixedString<8> name;
		RGBA8 color;
	};

	PlyInfo g_ply_info[3] = {
		{'N', "none", CGray},
		{'B', "black", CBlack},
		{'W', "white", CWhite},
	};

	PlyInfo get_ply_info(Go::Ply ply) {
		switch (ply) {
			case Go::PlyNone:
				return g_ply_info[0];
			case Go::PlyBlack:
				return g_ply_info[1];
			case Go::PlyWhite:
				return g_ply_info[2];
		}
		ext::fail("");
	}





	FixedString<8> get_action_name(Go::Action a)
	{
		FixedString<8> s;
		auto r = s.range();
		if (a == Go::ActionNone) {
			print(r, "none");
		}
		else if (a == Go::ActionPass) {
			print(r, "pass");
		}
		else if (a == Go::ActionResign) {
			print(r, "resign");
		}
		else {
			auto v = Go::action_to_vec(a);
			print(r, "{}{}", Char('a' + v[0]), Char('a' + v[1]));
		}
		return s;
	}

	void Board::render() {
		auto cell_dim = get_cell_dim();
		auto hd = cell_dim[0] / 2;
		auto fd = cell_dim[0];

		render_fill(box, CYellow);

		int16_t line_len = fd * (Go::RULE_BOARD - 1) + 2;


		// render grid
		for (int16_t i = 0; i < Go::RULE_BOARD; ++i) {
			render_fill(b2s(box.pos + v2s(hd + fd*i - 1, hd - 1), v2s(2, line_len)), CBlack);
			render_fill(b2s(box.pos + v2s(hd - 1, hd + fd*i - 1), v2s(line_len, 2)), CBlack);
		}

		// render stones
		for (int16_t j = 0; j < Go::RULE_BOARD; ++j) {
			for (int16_t i = 0; i < Go::RULE_BOARD; ++i) {
				v2s cell_pos = box.pos + vmul(cell_dim, {i,j});

				auto ply = game.get(Go::Vec(i,j));
				if (ply != Go::PlyNone) {
					render_aamask(cell_pos, res(0), get_ply_info(ply).color);
				}

				// ghost on mouse over
				if (game.player != Go::PlyNone and game.get({i,j}) == Go::PlyNone) {
					if (overlap(b2s(cell_pos, cell_dim), g_mouse_pos)) {
						auto c = get_ply_info(game.player).color;
						c.a *= 0.8;
						render_aamask(cell_pos, res(0), c);
					}
				}

			}
		}
	}




	v2s Board::get_grid_pos(v2s click_pos) const
	{
		auto cell_dim = get_cell_dim();
		auto p = (click_pos - box.pos);
		return v2s{p[0] / cell_dim[0], p[1] / cell_dim[1]};
	}


	bool Board::handle(SDL_Event &e) {
		switch (e.type) {
			case SDL_MOUSEBUTTONDOWN: {
				if (game.player != Go::PlyNone) {
					auto m_pos = get_mouse_pos(e);
					if (overlap(box, m_pos)) {
						Go::Vec vec = Go::Vec(get_grid_pos(m_pos));
						if (game.get(vec) == Go::PlyNone) {
							//std::cout << "clicked on: " << grid_pos << "\n";
							Err err = Go::move(game, game, Go::action_move(vec), game.player);
							if (err) {
								print("MESSAGE: {}\n", err);
							}
							return 1;
						}
					}
				}
				break;
			}
		}
		return 0;
	}




	// Button ---------------------------------------------------------



	void Button::init(b2s box, std::string label, std::function<void()> const& f) {
		this->box = box;
		this->label = label;
		this->handler = f;
	}

	void Button::render() {

		auto full_box = box;
		RGBA8 fg = CText;
		RGBA8 bg = CWood;

		if (BUTTON_STYLE == 'T') {

			auto side_box = b2s(box.pos, v2s(Ex, box.dim[1]));
			auto text_box = b2s(box.pos + v2s(Ex,0), box.dim - v2s(Ex,0));

			//auto text_dim = get_text_dim(ss(0), label);
			//auto text_pos = calc_align(text_box, text_dim, v2f(0.5, 0.5));

			render_fill(full_box, fg);
			render_fill(shrink(full_box, int16_t(1)), bg);

			render_fill(side_box, fg);
			if (not overlap(box, g_mouse_pos)) {
				render_fill(shrink(side_box, int16_t(1)), bg);
			}

			render_text(full_box, v2f(0.5, 0.5), ss(0), fg, label);
		}
		else {

			if (overlap(box, g_mouse_pos)) {

				render_fill(full_box, fg);
				render_fill(shrink(full_box, int16_t(1)), bg);
				render_text(full_box, v2f(0.5, 0.5), ss(0), fg, label);
			}
			else {
				render_fill(full_box, fg);
				render_text(full_box, v2f(0.5, 0.5), ss(0), bg, label);
			}
		}


	}

	bool Button::handle(SDL_Event &e) {
		switch(e.type) {
			case SDL_MOUSEBUTTONDOWN: {
				auto p = get_mouse_pos(e);
				if (overlap(box, p)) {
					handler();
					return 1;
				}
				break;
			}
		}
		return 0;
	}

	// misc ---------------------------------------------------



	FixedString<8> format_score(Go::Score2 s2)
	{
		FixedString<8> r;
		auto rr = r.range();

		if (s2 == 0) {
			print(rr, "");
		}
		else {
			assert((s2 & 1) == 1);
			char c = s2 < 0 ? 'B' : 'W';
			print(rr, "{}+{}.5", Char(c), std::abs(s2) >> 1);
		}
		return r;
	}



	// Panel --------------------------------------------------

	void Panel::init(b2s b) {
		box = b;

		{
			auto dim = v2s(12*Ex, 1.4*Em);
			auto pos = calc_align(this->box, dim, {0.5, 0.5});
			next_button.init({pos, dim}, ">>", []() {
				sfg_next(sfg, game, game);
			});
		}

		{
			auto dim = v2s(12*Ex, 1.4*Em);
			auto pos = calc_align(panel.box, dim, {0.5, 0.9});
			reset_button.init({pos, dim}, "Reset game", []() {
				game.reset();
			});
		}

		{
			auto dim = v2s(12*Ex, 1.4*Em);
			auto pos = calc_align(panel.box, dim, {0.5, 0.8});
			resign_button.init({pos, dim}, "Resign", []() {
				if (game.player != Go::PlyNone) {
					Err r = Go::move(game, game, Go::ActionResign, game.player);
					if (r) {
						print("MESSAGE: {}\n", r);
					}
				}
			});
		}


		{
			auto dim = v2s(12*Ex, 1.4*Em);
			auto pos = calc_align(panel.box, dim, {0.5, 0.7});
			pass_button.init({pos, dim}, "Pass", []() {
				if (game.player != Go::PlyNone) {
					Err r = Go::move(game, game, Go::ActionPass, game.player);
					if (r) {
						print("MESSAGE: {}\n", r);
					}
				}
			});
		}
	}


	void Panel::render() {
		render_fill(box, CWood);

		RenderStream rs(box, spritesets.at(0), CWhite);
		rs.tab_size = 7 * Ex;

		print(rs, "PLAY\n");
		print(rs, "Turn:\r\t{}/{}\n", game.turn, Go::RULE_TURN_LIMIT);
		print(rs, "Player:\r\t{}\n", get_ply_info(game.player).name);
		print(rs, "AH:\r\t{} {}\n", get_action_name(game.ah[0]), get_action_name(game.ah[1]));
		print(rs, "Komi:\r\t{}\n", format_score(Go::RULE_KOMI));
		print(rs, "Result:\r\t{}\n", format_score(game.result));
		print(rs, "\n");
		//print(rs, "REPLAY\n");



		next_button.render();
		reset_button.render();
		pass_button.render();
		resign_button.render();

		//print(rs, ": {}", game.ah[1], game.ah[0])
		//print(rs, "captured: {}", game.score)
		//print(rs, "result: {}", game.result)
	}

	bool Panel::handle(SDL_Event &e) {

		if (next_button.handle(e)) return 1;
		if (reset_button.handle(e)) return 1;
		if (pass_button.handle(e)) return 1;
		if (resign_button.handle(e)) return 1;

		return 0;
	}



	// Window --------------------------------------------------

	void Window::init(char const* path_sfg) {

		print("INFO: creating window\n");

		g_mouse_pos = v2s(-5, -5);

		v2s board_dim = board.get_cell_dim() * int16_t(Go::RULE_BOARD);

		v2s window_dim = board_dim + v2s(24 * Ex, 0);

		g_renderer.create("Go", window_dim);
		g_renderer.clear();
		g_renderer.flip();

		print("INFO: loading resources\n");
		front::RCReader rc;
		rc.rc("res/main.rc");
		rc.read_rc();

		board.init({0,0});
		panel.init({board_dim[0], 0, 24 * Ex, board_dim[1]});

		if (strcmp(path_sfg, "") != 0) {
			sfg_load(sfg, path_sfg);
		}

	}

	void Window::render()
	{
		int x,y;
		SDL_GetMouseState(&x, &y);
		g_mouse_pos = get_ctx_pos({x,y});


		g_renderer.clear();

		board.render();
		panel.render();


		g_renderer.flip();
	}






	Window::~Window() {
		g_renderer.destroy();
	}

	bool Window::handle(SDL_Event &o)
	{
		switch (o.type) {
			case SDL_QUIT:
				f_stop = true;
				return 1;
			case SDL_WINDOWEVENT:
				switch (o.window.event) {
					case SDL_WINDOWEVENT_SIZE_CHANGED:
					case SDL_WINDOWEVENT_RESIZED: {
						auto new_dim = v2s(o.window.data1, o.window.data2);
						if (g_renderer.win_dim != new_dim)
						{
							g_renderer.resize_view(new_dim, g_renderer.ctx_dim);
						}
						return 1;
					}
				}
				break;
			default:

				if (board.handle(o)) return 1;
				if (panel.handle(o)) return 1;

				break;
		}
		return 0;
	}




}
