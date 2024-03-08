#pragma once

#include "Player.h"

#include <Eis/Renderer/Objects/OrthoCameraController.h>


static constexpr float c_WalkSpeed = 5.0f;
static constexpr float c_RunSpeed = 7.5f;


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

	void SetRunning(bool running) { m_Running = running; m_CameraController.SetCameraSpeed(m_Running ? c_RunSpeed : c_WalkSpeed); }

private:
	Eis::OrthoCameraController m_CameraController;
	glm::vec2 m_OldPos;
	float m_ActualRotation;
	bool m_Running;

private:
	static LocalPlayer* s_Instance;
};