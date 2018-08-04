#pragma once

#include "base.hpp"
#include <string>
#include "rc.hpp"
#include "../front/front.hpp"

namespace front
{

	using ResId = uint16_t;


	struct Sprite
	{
		aabb2f box{0,0,0,0}; // normalized rect
		v2s dim{0,0};

		v2s delta{0,0};
		int16_t adv{0};
		TexUnit texu{0};

		//Sprite() = default;
	};

	b2s get_pixel_box(Sprite const& s);


	inline std::ostream & operator<<(std::ostream & o, Sprite const& s)
	{
		o << "Sprite(" << s.dim << ", " << s.box << ")";
		return o;
	}





	struct Spriteset
	{
		ResId base{0};

		TexUnit texu{0};
		ResId max_sprites{0};

		int16_t ex{5};   //TODO
		int16_t em{10};  //TODO

		int16_t height{0};   // line height
		int16_t ascender{0};  // position of baseline
		int16_t descender{0};
		int16_t width{0};    // approx width of a character; ex
		v2s pad{0,0};      // default padding

		v2s get_glyph_rpos(v2s cpos, Sprite const& g) const {
			return cpos + g.delta; //+ v2s(0, ascender);
		}

	};

	extern ext::darray1<Texture, ResId> textures;
	extern ext::darray1<Spriteset, ResId> spritesets;
	extern ext::darray1<Sprite, ResId> sprites;

	struct RCReader
	{

		RCParser p;

		ResId base{0};
		Texture * tex{nullptr};
		Spriteset * ss{nullptr};

		void rc(Path const& path_rc);

		void read_rc();
		void read_texture();
		void read_spriteset();
		void read_sprite();

	};


	/// Return sprite box on texture in pixels
	b2s get_pixel_box(Sprite const& s);

	/// Return subsprite
	Sprite make_subsprite(Sprite const& o, b2s sub);


	inline Spriteset const& ss(ResId id) {
		return spritesets.at(id);
	}

	inline Sprite const& res(ResId id) {
		return sprites.at(id);
	}

	inline Sprite const& res(ResId base, ResId id) {
		return sprites.at(base + id);
	}

	using Font = Spriteset;

}
