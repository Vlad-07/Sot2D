#pragma once

#include <Eis.h>


static constexpr float c_WalkSpeed = 5.0f;
static constexpr float c_RunSpeed = 7.5f;


class LocalPlayer
{
public:
	LocalPlayer();
	LocalPlayer(const LocalPlayer&) = delete;
	LocalPlayer(LocalPlayer&&) = delete;
	void operator=(const LocalPlayer&) = delete;
	~LocalPlayer();

	void Init();

	void OnUpdate(Eis::TimeStep ts);

	glm::vec2 GetPos() const { return m_Pos; }
	void SetPos(const glm::vec2& pos);
	bool HasMoved() { if (m_OldPos != m_Pos) { m_OldPos = m_Pos; return true; } else return false; }
	
	float GetRotation() const { return m_DisplayRotation; }
	void SetRotation(float rot) { m_DisplayRotation = m_ActualRotation = rot; }

	auto& GetCameraController() { return m_CameraController; }

	const auto& GetTexture() const { return m_Texture; }

private:
	Eis::OrthoCameraController m_CameraController;

	glm::vec2 m_Pos,
			  m_OldPos;
	float m_DisplayRotation,
		  m_ActualRotation;

	Eis::Ref<Eis::Texture2D> m_Texture;

private:
	static LocalPlayer* s_Instance;
};