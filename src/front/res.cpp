#include "res.hpp"



namespace front {

	using namespace ext;


	ext::darray1<Texture, ResId> textures;
	ext::darray1<Spriteset, ResId> spritesets;
	ext::darray1<Sprite, ResId> sprites;

	/// Return sprite box on texture in pixels
	b2s get_pixel_box(Sprite const& s)
	{

		v2f df = s.box.dim();

		v2f ratio = ext::vdiv0(v2f(s.dim), df);

		auto p = vmul<float_t>(s.box.pos, ratio);
		auto d = vmul<float_t>(df, ratio); // this is s.dim
		return b2s(v2s(p),v2s(d));
	}

	/// Return subsprite
	Sprite make_subsprite(Sprite const& o, b2s sub)
	{
		// sub -- pixel subbox

		auto fdim = o.box.dim();
		auto pdim = v2f(o.dim);

		// pix * ratio -> rel
		auto ratio = vdiv0(fdim, pdim);

		Sprite s;
		s.dim = sub.dim;
		s.box.pos = o.box.pos + vmul(v2f(sub.pos), ratio);
		s.box.end = s.box.pos + vmul(v2f(sub.dim), ratio);
		s.texu = o.texu;
		return s;
	}


	using ext::operator/;

	//	font.texu = TEXU_FONT0;
	//	font.base = FONT_TINY;
	//	front::bind_texture(TEXU_SPRITES, textures.at(0));
	//	front::bind_texture(TEXU_FONT0, textures.at(1));



	void RCReader::read_sprite()
	{
		auto id = ss->base + p.read_uint16();
		Sprite & sprite = sprites.at(id);

		sprite.texu = textures.get_pos(*tex);

		while (p.ok())
		{
			auto & sym = p.read_symbol();

			if (sym == "name") {
				auto name = p.read_string();
				print("sprite {} name {}\n", id, name);
				
			}
			else if (sym == "box") {
				auto x = p.read_int16();
				auto y = p.read_int16();
				auto dx = p.read_int16();
				auto dy = p.read_int16();

				auto a = vdiv(v2f(x,y), v2f(tex->dim));
				auto b = vdiv(v2f(x + dx, y + dy), v2f(tex->dim));

				sprite.box = aabb2f(a,b);
				sprite.dim = v2s(dx,dy);
			}
			else if (sym == "delta") {
				auto x = p.read_int16();
				auto y = p.read_int16();
				sprite.delta = v2s(x,y);
			}
			else if (sym == "bearing") {
				auto x = p.read_int16();
				auto y = p.read_int16();
				sprite.delta = v2s(x,-y);
			}
			else if (sym == "adv") {
				sprite.adv = p.read_int16();
			}
			else {
				p.unread();
				break;
			}
		}
	}


	void RCReader::rc(Path const& path_rc)
	{
		p.rc(path_rc);    // todo full path  support
	}



	void RCReader::read_spriteset()
	{
		{
			auto s = p.read_uint16();
			ss = &spritesets.at(s);
			print("INFO: spriteset {}\n", s);
		}

		ss->texu = textures.get_pos(*tex);
		print("INFO: texu {}\n", ss->texu);

		while (p.ok())
		{
			auto & sym = p.read_symbol();

			if (sym == "name") {
				auto s = p.read_string();
				print("INFO: name {}\n", s);
			}
			else if (sym == "max_sprites") {
				ss->base = base;
				print("INFO: base {}\n", ss->base);
				ss->max_sprites = p.read_int16();
				base += ss->max_sprites;
				print("INFO: max_sprites {}\n", ss->max_sprites);
			}
			else if (sym == "height") {
				ss->height = p.read_int16();
			}
			else if (sym == "width") {
				ss->width = p.read_int16();
			}
			else if (sym == "ascender") {
				ss->ascender = p.read_int16();
			}
			else if (sym == "descender") {
				ss->descender = p.read_int16();
			}
			else if (sym == "pad") {
				ss->pad[0] = p.read_int16();
				ss->pad[1] = p.read_int16();
			}
			else if (sym == "rc") {
				auto s = p.read_string();
				print("INFO: rc {}\n", s);
				rc(s);
			}
			else {
				p.unread();
				break;
			}
		}
		p.end();
	}

	void RCReader::read_texture()
	{
		auto id = p.read_uint16();
		tex = &textures.at(id);
		print("INFO: texture {}\n", id);

		while (p.ok())
		{
			auto & sym = p.read_symbol();

			if (sym == "path")
			{
				Path path = p.read_string();
				*tex = make_texture(load_png_RGBA8(path));
				bind_texture(id, *tex);
				print("INFO: path {}\n", path);
			}
			else {
				p.unread();
				break;
			}
		}
		p.end();
	}


	void RCReader::read_rc()
	{
		while (p.ok())
		{
			auto & sym = p.read_symbol();

			if (sym == "spriteset") {
				read_spriteset();
			}
			else if (sym == "sprite") {
				read_sprite();
			}
			else if (sym == "texture") {
				read_texture();
			}
			else if (sym == "max_textures") {
				auto s = p.read_uint16();
				textures.resize(s);
				print("INFO: max_textures {}\n", s);
			}
			else if (sym == "max_sprites") {
				auto s = p.read_uint16();
				sprites.resize(s);
				print("INFO: max_sprites {}\n", s);
			}
			else if (sym == "max_spritesets") {
				auto s = p.read_uint16();
				spritesets.resize(s);
				print("INFO: max_spritesets {}\n", s);
			}
			else if (sym == "rc") {
				auto s = p.read_string();
				print("INFO: rc {}\n", s);
				rc(s);
			}
			else {
				p.set_error("read_rc: unknown command: '{}", sym);
			}
		}
		p.end();


	}

}
