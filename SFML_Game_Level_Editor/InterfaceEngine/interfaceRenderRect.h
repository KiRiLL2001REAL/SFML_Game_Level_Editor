#pragma once

namespace edt {
	class tRenderRect : public tObject {
	protected:
		sf::VertexArray render_squad;		// Фигура, в которой выполняется отрисовка
		sf::RenderTexture render_texture;	// Текстура, в которой отрисовываются объекты
		sf::Color clear_color;				// Цвет очистки текстуры
		bool need_rerender;					// Нужна ли перерисовка

	public:
		tRenderRect(tAbstractBasicClass* _owner, sf::FloatRect rect = { 0, 0, 64, 64 });
		tRenderRect(tAbstractBasicClass* _owner, nlohmann::json& js);
		tRenderRect(const tRenderRect& r);
		virtual ~tRenderRect();

		virtual void draw(sf::RenderTarget& target);
		virtual void move(sf::Vector2f delta);

		// Setters
		void setClearColor(const sf::Color& new_color);
		void setTextureRect(const sf::FloatRect& new_rect);
		virtual void setSize(const sf::Vector2f& new_size);
		virtual void setPosition(const sf::Vector2f& new_position);
		virtual void setTextureSize(const sf::Vector2u& new_size);

		// Getters
		const sf::Vector2u getTextureSize() const;
		virtual const sf::FloatRect getLocalBounds() const;
		virtual nlohmann::json getParamsInJson() const;
	};
}