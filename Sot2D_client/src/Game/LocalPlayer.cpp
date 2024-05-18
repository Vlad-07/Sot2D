#include "LocalPlayer.h"

#include "GameLayer.h"


LocalPlayer* LocalPlayer::s_Instance = nullptr;


LocalPlayer::LocalPlayer() : m_CameraController(16.0f / 9.0f)
{
	EIS_ASSERT(!s_Instance, "Local player already exists!");
	s_Instance = this;

	m_Texture = Eis::Texture2D::Create("assets/textures/player.png");

	Init();
}
LocalPlayer::~LocalPlayer()
{
	s_Instance = nullptr;
}

void LocalPlayer::Init()
{
	m_OldPos = m_Pos = glm::vec2(0);
	m_ActualRotation = m_DisplayRotation = 0.0f;
	m_CameraController.SetPosition(glm::vec3(0));
	m_CameraController.SetCameraSpeed(c_WalkSpeed);
	m_CameraController.SetMinZoom(2.0f);
	m_CameraController.SetMaxZoom(8.0f);
	m_CameraController.SetZoom(5.0f);
	m_CameraController.SetZoomSpeedEffect(false);
}

void LocalPlayer::OnUpdate(Eis::TimeStep ts)
{
	// Position
	m_CameraController.OnUpdate(ts);
	SetPos(m_CameraController.GetCamera().GetPosition());

	// Sprint
	if (!GameLayer::Get().m_GodMode)
		m_CameraController.SetCameraSpeed(Eis::Input::IsKeyPressed(EIS_KEY_LEFT_SHIFT) ? c_RunSpeed : c_WalkSpeed);

	// Rotation
	if (fabs(m_ActualRotation - m_DisplayRotation) > 181.0f) // fix going from -pi to pi/2
		m_DisplayRotation += 360.0f * (m_ActualRotation - m_DisplayRotation > 0.0f ? 1.0f : -1.0f);
	m_DisplayRotation += (m_ActualRotation - m_DisplayRotation) * 15.0f * ts; // interpolate rotation
}

void LocalPlayer::SetPos(const glm::vec2& pos)
{
	m_OldPos = m_Pos;
	m_Pos = pos;

	if (m_Pos != m_OldPos)
		m_ActualRotation = glm::degrees(atan2f(m_Pos.y - m_OldPos.y, -m_Pos.x + m_OldPos.x) * -1.0f);
}