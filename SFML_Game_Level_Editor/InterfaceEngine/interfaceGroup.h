#pragma once

namespace edt {
	class tGroup : public tAbstractBasicClass { // �����-���������
	protected:
		std::list<tAbstractBasicClass*> elem;		// ��������� ���������, ���������� � ������ ������

	public:
		tGroup(tAbstractBasicClass* _owner);
		tGroup(tAbstractBasicClass* _owner, nlohmann::json& js);
		tGroup(const tGroup& g);
		virtual ~tGroup();

		void forEach(unsigned int code, tAbstractBasicClass* from = nullptr);	// ��������� ������� ��� ���� ������������
		void makeObjectsFromJson(tAbstractBasicClass* _owner, nlohmann::json& js);

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
		virtual void _insert(tAbstractBasicClass* object);		// �������� �������� � ������ ������������
		virtual bool _delete(tAbstractBasicClass* object);		// �������� �������� �� ������
		virtual void select(tAbstractBasicClass* object);		// ��������� ����� "�������" � ��������

		// Getters
		virtual nlohmann::json getParamsInJson() const;
	};
}