#pragma once

#include <DirectXMath.h>

using namespace DirectX;
class Camera
{

public:
	Camera();
	~Camera();


	XMVECTOR GetPositionXM() const;
	XMFLOAT3 GetPosition() const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& v);


	XMVECTOR GetRightXM() const;
	XMFLOAT3 GetRight() const;

	XMVECTOR GetUpXm() const;
	XMFLOAT3 GetUp() const;

	XMVECTOR GetLookXM() const;
	XMFLOAT3 GetLook() const;

	
	float GetNearZ() const;
	float GetFarZ() const;
	float GetAspect() const;
	float GetFovY() const;
	float GetFovX() const;

	float GetNearWindowWidth() const;
	float GetNearWindowHeight() const;
	float GetFarWindowWidth() const;
	float GetFarWindowHeight() const;


	void SetLens(float fovY, float aspect, float zn, float zf);

	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);


	XMMATRIX GetView() const;
	XMMATRIX GetProjection() const;
	XMMATRIX GetViewProjection() const;

	void HorizontalMove(float d);
	void VerticalMove(float d);
	void Walk(float d);

	void Pitch(float angle);
	void RotateY(float angle);


	void UpdateViewMatrix();

private:

	XMFLOAT3 m_Position;
	XMFLOAT3 m_Right;
	XMFLOAT3 m_Up;
	XMFLOAT3 m_Look;


	float m_NearZ;
	float m_FarZ;
	float m_Aspect;
	float m_FovY;
	float m_NearWindowHeight;
	float m_FarWindowHeight;

	XMFLOAT4X4 m_View;
	XMFLOAT4X4 m_Projection;
};

