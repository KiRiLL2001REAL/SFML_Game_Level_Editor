#pragma once

namespace edt{
	class tDisplay : public tRenderRect, public tGroup {
	protected:
		tAbstractBasicClass* owner;

	public:
		tDisplay(tAbstractBasicClass* _owner, sf::FloatRect rect = { 0, 0, 100, 100 });
		tDisplay(tAbstractBasicClass* _owner, nlohmann::json& js);
		tDisplay(const tDisplay& d);
		virtual ~tDisplay();

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
		virtual void updateTexture();

		// Setters
		void setCameraOffset(const sf::Vector2f& new_offset);
		virtual void setOwner(tAbstractBasicClass* new_owner);
		virtual void setTextureSize(const sf::Vector2u& new_size);

		// Getters
		const sf::Vector2f& getCameraOffset() const;
		virtual const bool pointIsInsideMe(sf::Vector2i point) const;
		virtual tAbstractBasicClass* getOwner() const;
		virtual const sf::FloatRect getLocalBounds() const;
		virtual const sf::FloatRect getGlobalBounds() const;
		virtual nlohmann::json getParamsInJson() const;
	};
}