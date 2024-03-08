#include "LocalPlayer.h"

#define PI 3.14159265359f

LocalPlayer* LocalPlayer::s_Instance = nullptr;


LocalPlayer::LocalPlayer() : Player(), m_CameraController(16.0f / 9.0f), m_OldPos(m_Pos), m_ActualRotation(0.0f), m_Running(false)
{
	EIS_ASSERT(!s_Instance, "Local player already exists!");

	LoadTexture();

	m_CameraController.SetZoomSpeedEffect(false);
	m_CameraController.SetMinZoom(2.0f);
	m_CameraController.SetMaxZoom(6.0f);
	m_CameraController.SetCameraSpeed(c_WalkSpeed);
}
LocalPlayer::~LocalPlayer()
{
	s_Instance = nullptr;
}

void LocalPlayer::Init()
{
	m_OldPos = m_Pos = glm::vec2(0);
	m_CameraController.SetPosition(glm::vec3(0));
	m_CameraController.SetCameraSpeed(c_WalkSpeed);
	m_CameraController.SetMinZoom(2.0f);
	m_CameraController.SetMaxZoom(6.0f);
	m_CameraController.SetZoom(2.0f);
}

void LocalPlayer::OnUpdate(Eis::TimeStep ts)
{
	m_CameraController.OnUpdate(ts);
	SetPos(m_CameraController.GetCamera().GetPosition());

	if (fabs(m_ActualRotation - m_Rotation) > 3.0f * PI / 2.0f) // fix going from -pi to pi/2
		m_Rotation += 2 * PI * (m_ActualRotation - m_Rotation > 0.0f ? 1.0f : -1.0f);

	m_Rotation += (m_ActualRotation - m_Rotation) * 0.3f; // interpolate rotation
}

void LocalPlayer::SetPos(const glm::vec2& pos)
{
	m_OldPos = m_Pos;
	m_Pos = pos;

	if (m_Pos != m_OldPos)
		m_ActualRotation = atan2f((m_Pos - m_OldPos).y, -(m_Pos - m_OldPos).x) * -1.0f;
}