#pragma once

#include "Player.h"

#include <Eis/Renderer/Objects/OrthoCameraController.h>


class LocalPlayer : public Player
{
public:
	LocalPlayer();
	~LocalPlayer();

	void Init();

	void OnUpdate(Eis::TimeStep ts);

	virtual void SetPos(const glm::vec2& pos) override;
	bool HasMoved() { if (m_OldPos != m_Pos) { m_OldPos = m_Pos; return true; } else return false; }

	virtual void SetRotation(float rot) { m_Rotation = m_ActualRotation = rot; }

	auto& GetCameraController() { return m_CameraController; }

private:
	Eis::OrthoCameraController m_CameraController;
	glm::vec2 m_OldPos;
	float m_ActualRotation;

private:
	static LocalPlayer* s_Instance;
};