#pragma once

#include "Player.h"

#include <Eis/Renderer/Objects/OrthoCameraController.h>

class LocalPlayer : public Player
{
public:
	LocalPlayer() : Player(), m_CameraController(16.0f / 9.0f)
	{
		EIS_ASSERT(!s_Instance, "Local player already exists!");
	}
	~LocalPlayer()
	{
		s_Instance = nullptr;
	}

	void OnUpdate(Eis::TimeStep ts)
	{
		m_CameraController.OnUpdate(ts);
		SetPos(m_CameraController.GetCamera().GetPosition());
	}

	auto& GetCameraController() { return m_CameraController; }

private:
	Eis::OrthoCameraController m_CameraController;

private:
	static LocalPlayer* s_Instance;
};