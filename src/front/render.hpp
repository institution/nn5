#pragma once

#include "res.hpp"
#include <string>
#include "front.hpp"

namespace front
{
	using front::RenderCall;

	extern front::Front g_renderer;


	inline void render_texture(b2s dst, TexUnit texu)
	{
		RenderCall rc;
		rc.win = &g_renderer;
		rc.mode = front::MODE_TEXTURE;
		rc.texu = texu;
		rc.dst = dst;
		rc.uv0 = aabb2f(0,0,1,1);
		rc.call();
	}

	inline void render_sprite(v2s pos, Sprite const& s0)
	{
		RenderCall rc;
		rc.win = &g_renderer;
		rc.mode = front::MODE_TEXTURE;
		rc.texu = s0.texu;
		rc.dst = b2s(pos, s0.dim);
		rc.uv0 = s0.box;
		rc.call();
	}


	inline void render_sprite_replace(v2s pos, Sprite const& s0, Sprite const& s1)
	{
		RenderCall rc;
		rc.win = &g_renderer;
		rc.mode = front::MODE_REPLACE;
		rc.texu = s0.texu;
		rc.dst = b2s(pos, s0.dim);
		rc.uv0 = s0.box;
		rc.uv1 = s1.box;
		rc.call();
	}




	inline void render_fill(b2s box, RGBA8 color)
	{
		RenderCall rc;
		rc.win = &g_renderer;
		rc.mode = front::MODE_FILL;
		rc.color = color;
		rc.dst = box;
		rc.call();
	}

	inline void render_aamask(TexUnit texu, v2s pos, Sprite const& s0, RGBA8 color)
	{
		RenderCall rc;
		rc.win = &g_renderer;
		rc.mode = front::MODE_AAMASK;
		rc.texu = texu;
		rc.dst = b2s(pos, s0.dim);
		rc.uv0 = s0.box;
		rc.color = color;
		rc.call();
	}
	
	
	inline void render_aamask(v2s pos, Sprite const& s0, RGBA8 color)
	{
		RenderCall rc;
		rc.win = &g_renderer;
		rc.mode = front::MODE_AAMASK;
		rc.texu = s0.texu;
		rc.dst = b2s(pos, s0.dim);
		rc.uv0 = s0.box;
		rc.color = color;
		rc.call();
	}

	inline void render_aaline_test(v2s a, v2s b, RGBA8 c, int8_t thick)
	{
		// render textured quad

		/*set_blend_fill(c);

		if (a == b) {
			return;
		}

		auto aa = v2f(a);
		auto bb = v2f(b);

		auto x = bb - aa;
		auto y = v2f{-x[1], x[0]};
		auto n = (y * (1.0f / std::sqrt(y.norm2()))) * thick;

		// quad as triangle fan x,y + u,v + displacement
		GLfloat data[] = {
			aa[0], aa[1],  0.0f, 0.0f, -n[0], -n[1],
			aa[0], aa[1],  0.0f, 1.0f,  n[0],  n[1],
			bb[0], bb[1],  1.0f, 1.0f,  n[0],  n[1],
			bb[0], bb[1],  1.0f, 0.0f, -n[0], -n[1]
		};
		*/
		//_render_call_GL(line_seg.id, data, sizeof(data));
	}

	/*inline void render_inner(b2s box, RGBA8 color, uint8_t th) {
	
		auto dim = box.dim;
		auto a = box.pos;
		auto b = box.pos + v2s{dim[0], 0};
		auto c = box.pos + v2s{dim[0], dim[1]};
		auto d = box.pos + v2s{0, dim[1]};
		
		render_fill({a, {dim[0], th}});
		render_fill({a, {dim[0], th}});
		render_fill({a, {dim[0], th}});
		render_fill({a, {dim[0], th}});
	}*/



	/// Return approximate text dim
	v2s approx_text_dim(Font const& font, int16_t x, int16_t y = 0);


	/// Return text dim
	v2s get_text_dim(Font const& font, std::string const& text);

	/// Render text at pos (top-left)
	b2s render_text(
		v2s pos,
		Font const& font, RGBA8 fg,
		std::string const& text
	);

	/// Render text
	b2s render_text(
		b2s box, v2f align,
		Font const& font,
		RGBA8 fg, RGBA8 bg,
		std::string const& text
	);

	/// Render text
	b2s render_text(
		b2s box, v2f align,
		Font const& font, RGBA8 fg,
		std::string const& text
	);


	struct RenderStream
	{
		b2s box;
		Font const& font;
		RGBA8 fg;
		int16_t tab_size;
		
		v2s cpos;
		
		RenderStream(b2s box, Font const& font, RGBA8 fg): box(box), font(font), fg(fg) {
			this->box.pos += font.pad;
			this->box.dim -= font.pad;
			this->box.dim -= font.pad;
			cpos = this->box.pos;
			tab_size = font.ex * 8;
		}
		
		void put(char t) {
			if (t == '\n') {
				cpos[1] += font.height;
				cpos[0] = box.pos[0];
			}
			else if (t == '\t') {
				cpos[0] += tab_size;
			}
			else if (t == '\r') {
				cpos[0] = box.pos[0];
			}			
			else {
				auto & g = res(font.base, t);
				render_aamask(font.texu, font.get_glyph_rpos(cpos, g), g, fg);
				cpos[0] += g.adv; // move to next position
			}
		}
	};
	
	
	inline void put(RenderStream & rs, char t) { rs.put(t); } 	
	
	

}
