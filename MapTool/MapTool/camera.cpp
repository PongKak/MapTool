#include "camera.h"



Camera::Camera()
	: m_Position(0.0f, 0.0f, 0.0f),
	m_Right(1.0f,0.0f,0.0f),
	m_Up(0.0f,1.0f,0.0f),
	m_Look(0.0f,0.0f,1.0f)
{
	SetLens(XM_PIDIV2, 800.0f / (float)600.0f, 0.01f, 1000.0f);

	XMVECTOR pos = XMVectorSet(-20.0f, 45.0f, 10.0f, 1.0f);
	XMVECTOR target = XMVectorSet(50.0f, 0.0f, 50.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	LookAt(pos, target, up);
}


Camera::~Camera()
{
}

XMVECTOR Camera::GetPositionXM() const
{
	return XMLoadFloat3(&m_Position);
}

XMFLOAT3 Camera::GetPosition() const
{
	return m_Position;
}

void Camera::SetPosition(float x, float y, float z)
{
	m_Position = XMFLOAT3(x, y, z);
}

void Camera::SetPosition(const XMFLOAT3 & v)
{
	m_Position = v;
}

XMVECTOR Camera::GetRightXM() const
{
	return XMLoadFloat3(&m_Right);
}

XMFLOAT3 Camera::GetRight() const
{
	return m_Position;
}

XMVECTOR Camera::GetUpXm() const
{
	return XMLoadFloat3(&m_Up);
}

XMFLOAT3 Camera::GetUp() const
{
	return m_Up;
}

XMVECTOR Camera::GetLookXM() const
{
	return XMLoadFloat3(&m_Look);
}

XMFLOAT3 Camera::GetLook() const
{
	return m_Look;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = zn;
	m_FarZ = zf;

	m_NearWindowHeight = m_NearZ * 2.0f * tanf(0.5f*m_FovY);
	m_FarWindowHeight = m_FarZ * 2.0f * tanf(0.5f*m_FovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
	XMStoreFloat4x4(&m_Projection, P);
}

float Camera::GetNearZ() const
{
	return m_NearZ;
}

float Camera::GetFarZ() const
{
	return m_FarZ;
}

float Camera::GetAspect() const
{
	return m_Aspect;
}

float Camera::GetFovY() const
{
	return m_FovY;
}

float Camera::GetFovX() const
{
	float halfWidth = GetNearWindowHeight() * 0.5f;
	return 2.0f * atan(halfWidth / m_NearZ);
}

float Camera::GetNearWindowWidth() const
{
	return m_Aspect * m_NearWindowHeight;
}

float Camera::GetNearWindowHeight() const
{
	return m_NearWindowHeight;
}

float Camera::GetFarWindowWidth() const
{
	return m_Aspect * m_FarWindowHeight;
}

float Camera::GetFarWindowHeight() const
{
	return m_FarWindowHeight;
}


void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&m_Position, pos);
	XMStoreFloat3(&m_Look, L);
	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);
}



XMMATRIX Camera::GetView() const
{
	return XMLoadFloat4x4(&m_View);
}

XMMATRIX Camera::GetProjection() const
{
	return XMLoadFloat4x4(&m_Projection);
}

XMMATRIX Camera::GetViewProjection() const
{
	return XMMatrixMultiply(GetView(), GetProjection());
}

void Camera::HorizontalMove(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&m_Right);
	XMVECTOR p = XMLoadFloat3(&m_Position);

	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, r, p));

}

void Camera::VerticalMove(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR u = XMLoadFloat3(&m_Up);
	XMVECTOR p = XMLoadFloat3(&m_Position);

	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, u, p));
}

void Camera::Walk(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&m_Look);
	XMVECTOR p = XMLoadFloat3(&m_Position);

	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, l, p));
}

void Camera::Pitch(float angle)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_Right), angle);

	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
}

void Camera::RotateY(float angle)
{
	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));

}

void Camera::UpdateViewMatrix()
{	
	XMVECTOR R = XMLoadFloat3(&m_Right);
	XMVECTOR U = XMLoadFloat3(&m_Up);
	XMVECTOR L = XMLoadFloat3(&m_Look);
	XMVECTOR P = XMLoadFloat3(&m_Position);


	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));
	R = XMVector3Cross(U, L);

	float x = -(XMVectorGetX(XMVector3Dot(P, R)));
	float y = -(XMVectorGetX(XMVector3Dot(P, U)));
	float z = -(XMVectorGetX(XMVector3Dot(P, L)));

	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
	XMStoreFloat3(&m_Look, L);


	m_View(0, 0) = m_Right.x;
	m_View(1, 0) = m_Right.y;
	m_View(2, 0) = m_Right.z;
	m_View(3, 0) = x;

	m_View(0, 1) = m_Up.x;
	m_View(1, 1) = m_Up.y;
	m_View(2, 1) = m_Up.z;
	m_View(3, 1) = y;

	m_View(0, 2) = m_Look.x;
	m_View(1, 2) = m_Look.y;
	m_View(2, 2) = m_Look.z;
	m_View(3, 2) = z;

	m_View(0, 3) = 0.0f;
	m_View(1, 3) = 0.0f;
	m_View(2, 3) = 0.0f;
	m_View(3, 3) = 1.0f;

}
