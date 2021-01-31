#include "stdafx.h"
#include "interfaceEngine.h"

namespace edt
{
	tRenderRect::tRenderRect(tAbstractBasicClass* _owner, sf::FloatRect rect) :
		tObject(_owner),
		render_squad(sf::VertexArray(sf::Quads, 4)),
		clear_color({ 0, 0, 0, 255 }),
		need_rerender(true)
	{
		tObject::setPosition({ rect.left, rect.top });
		render_texture.create((unsigned int)rect.width, (unsigned int)rect.height);

		setTextureSize({ (unsigned int)rect.width, (unsigned int)rect.height });
		setSize({ rect.width, rect.height });
		setTextureRect({ 0.f, 0.f, rect.width, rect.height });
	}

	tRenderRect::tRenderRect(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tObject(_owner, js),
		render_squad(sf::VertexArray(sf::Quads, 4)),
		need_rerender(true)
	{
		vec<unsigned char> vuc = js["clear_color"].get<vec<unsigned char>>();
		clear_color = { vuc[0], vuc[1], vuc[2], vuc[3] };

		vec<float> vf = js["size"].get<vec<float>>();
		setSize({ vf[0], vf[1] });
		setTextureRect({ 0.f, 0.f, vf[0], vf[1] });

		vec<unsigned int> vui = js["texture_size"].get<vec<unsigned int>>();
		render_texture.create(vui[0], vui[1]);
		setTextureSize({ vui[0], vui[1] });

		setPosition({ x, y });

		setAnchor(anchor);
	}

	tRenderRect::tRenderRect(const tRenderRect& r) :
		tObject(r),
		render_squad(r.render_squad),
		clear_color(r.clear_color),
		need_rerender(r.need_rerender)
	{
		sf::Sprite spr;
		spr.setTexture(r.render_texture.getTexture());
		render_texture.draw(spr);
		render_texture.display();
	}

	tRenderRect::~tRenderRect()
	{
	}

	void tRenderRect::draw(sf::RenderTarget& target)
	{
		if (checkOption(option_mask.can_be_drawn))
		{
			if (need_rerender)
			{
				need_rerender = false;
				updateTexture();
			}
			// Отобразиться
			target.draw(render_squad, &render_texture.getTexture());
		}
	}

	void tRenderRect::move(sf::Vector2f delta)
	{
		tObject::move(delta);
		sf::Vector2f pos;
		for (char i = 0; i < 4; i++)
		{
			pos = render_squad[i].position;
			render_squad[i].position = pos + delta;
		}
	}

	void tRenderRect::setClearColor(const sf::Color& color)
	{
		clear_color = color;
	}

	void tRenderRect::setTextureSize(const sf::Vector2u& new_size)
	{
		render_texture.create(new_size.x, new_size.y);
		need_rerender = true;
	}

	void tRenderRect::setTextureRect(const sf::FloatRect& rect)
	{
		render_squad[0].texCoords = { rect.left, rect.top };
		render_squad[1].texCoords = { rect.left + rect.width, rect.top };
		render_squad[2].texCoords = { rect.left + rect.width, rect.top + rect.height };
		render_squad[3].texCoords = { rect.left, rect.top + rect.height };
	}

	void tRenderRect::setSize(const sf::Vector2f& new_size)
	{
		render_squad[0].position = { x, y };
		render_squad[1].position = { x + new_size.x, y };
		render_squad[2].position = { x + new_size.x, y + new_size.y };
		render_squad[3].position = { x, y + new_size.y };

		sf::FloatRect rect = {
			render_squad[0].texCoords.x,
			render_squad[0].texCoords.y,
			new_size.x,
			new_size.y
		};
		setTextureRect(rect);
	}

	void tRenderRect::setPosition(const sf::Vector2f& new_position)
	{
		tObject::setPosition(new_position);
		sf::Vector2f offset = getRelativeStartPosition();
		
		for (unsigned int i = 1; i <= 3; i++)
		{
			render_squad[i].position = {
				x + render_squad[i].position.x - render_squad[0].position.x,
				y + render_squad[i].position.y - render_squad[0].position.y
			};
		}
		render_squad[0].position = { x, y };

		for (unsigned int i = 0; i < 4; i++)
		{
			render_squad[i].position += offset;
		}
	}

	const sf::Vector2u tRenderRect::getTextureSize() const
	{
		return render_texture.getSize();
	}

	const sf::FloatRect tRenderRect::getLocalBounds() const
	{
		return sf::FloatRect(
			render_squad[0].position.x,
			render_squad[0].position.y,
			render_squad[1].position.x - render_squad[0].position.x,
			render_squad[3].position.y - render_squad[0].position.y
		);
	}

	nlohmann::json tRenderRect::getParamsInJson() const
	{
		nlohmann::json js = tObject::getParamsInJson();

		sf::Vector2f size = render_squad[2].position - render_squad[0].position;
		sf::Vector2u tex_size = render_texture.getSize();

		js["size"] = { size.x, size.y };
		js["texture_size"] = { tex_size.x, tex_size.y };
		js["clear_color"] = { clear_color.r, clear_color.g, clear_color.b, clear_color.a };

		return js;
	}
}