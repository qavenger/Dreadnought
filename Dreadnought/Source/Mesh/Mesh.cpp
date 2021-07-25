#include <stdafx.h>
#include "Mesh.h"
#include <MathUtil.h>
#include <Input.h>
void Mesh::Build(IRHIDevice* Device)
{
	gDevice->ResetCommandList();
	gDevice->BuildIndexBuffer(IndexBuffer);
	gDevice->BuildVertexBuffer(VertexBuffer);
	PSO->VertexShader = VertexShader;
	PSO->PixelShader = PixelShader;
	PSO->Init();
	gDevice->BuildPipelineStateObject(PSO);

	struct PerObjectData
	{
		_Matrix WVP;
		float4 Color;
	} Data;
	Data.Color = float4( 1,0,1,0 );
	float3 EyePosition = float3(-5, 0, 0);
	float3 CenterPosition = float3(0, 0, 0);
	//XMFLOAT3 Up = XMFLOAT3(0, 1, 0);
	float3 forward = (CenterPosition - EyePosition).SafeNormalize();
	float3 right = float3::UpVector ^ forward;
	float3 up = forward ^ right;
	float asp = 1024 / 720.f;
	float fov = GMath::Deg2Rad * 45.0f;
	float n = 0.1f;
	float f = 100.0f;
	float x = GMath::Tan(fov) * asp;
	float y = GMath::Tan(fov);
	_Matrix proj(
		0,		0,		f / (f - n),1,
		n* x,	0,		0,			0,
		0,		n* y,	0,			0,
		0,		0,		-n*f/(f-n),	0
	);
	Data.WVP = _Matrix(
		forward.x, right.x, up.x, 0,
		forward.y, right.y, up.y, 0,
		forward.z, right.z, up.z, 0,
		-EyePosition|forward, -EyePosition|right, -EyePosition|up,1
	);
	//Data.WVP = Data.WVP.GetInverseFast();
	Data.WVP *= proj;
	Data.WVP = Data.WVP.GetTransposed();
	XMMATRIX vm = XMMatrixLookAtLH(XMVectorSet(-5, 0, 0, 1), XMVectorSet(0, 0, 0, 1), XMVectorSet(0,1,0,0));
	XMMATRIX vp = XMMatrixTranspose(XMMatrixMultiply(vm, XMMatrixPerspectiveFovLH(45.f, 1024 / 720.f, 0.1f, 100.f)));
	//XMMATRIX Trans = XMMatrixLookAtLH(XMLoadFloat3(&EyePosition), XMLoadFloat3(&CenterPosition), XMLoadFloat3(&Up));
	//XMStoreFloat4x4(&Data.WVP, XMMatrixTranspose(XMMatrixMultiply(Trans, XMMatrixPerspectiveFovLH(45.f, 1024 / 720.f, 0.1f, 100.f))));
	Input::BindKeyInputEvent(EKeyCode::KeyBoardW, EKeyInputState::PRESSED, this, &Mesh::MoveForward);
	Input::BindKeyInputEvent(EKeyCode::KeyBoardW, EKeyInputState::RELEASED, this, &Mesh::Stop);
	Input::BindKeyInputEvent(EKeyCode::KeyBoardS, EKeyInputState::PRESSED, this, &Mesh::MoveBackward);
	Input::BindKeyInputEvent(EKeyCode::KeyBoardS, EKeyInputState::RELEASED, this, &Mesh::Stop);
	
	Input::BindKeyInputEvent(EKeyCode::KeyBoardQ, EKeyInputState::PRESSED, this, &Mesh::MoveUp);
	Input::BindKeyInputEvent(EKeyCode::KeyBoardQ, EKeyInputState::RELEASED, this, &Mesh::Stop);
	Input::BindKeyInputEvent(EKeyCode::KeyBoardE, EKeyInputState::PRESSED, this, &Mesh::MoveDown);
	Input::BindKeyInputEvent(EKeyCode::KeyBoardE, EKeyInputState::RELEASED, this, &Mesh::Stop);

	RHIConstantBuffer* ConstantBuffer = gDevice->CreateConstantBuffer();
	ConstantBuffer->SetElementCount(1);
	ConstantBuffer->SetElementSize(sizeof(PerObjectData));
	gDevice->BuildConstantBuffer(ConstantBuffer);
	ConstantBuffer->UpdateData(&Data, sizeof(PerObjectData), 0);
	PSO->ConstantBuffer = ConstantBuffer;
	gDevice->FlushCommandQueueSync();
}

void Mesh::Draw(IRHIDevice* Device)
{
	distance += direction;
	height += heightDirection;
	SCOPE_EVENT_STR_FORMAT(Draw, "Draw_%s", ObjectName.c_str())
	{
		DrawInfo Info(IndexBuffer, VertexBuffer, PSO);
		auto* ConstantBuffer = PSO->ConstantBuffer;

		struct PerObjectData
		{
			_Matrix WVP;
			float4 Color;
		} Data;
		Data.Color = float4(1, 0, 1, 0);
		float t = (float)Engine::GetGameTime();
		//float3 EyePosition(-5, 0, 0);
		float3 EyePosition(distance*GMath::Cos(t), distance*GMath::Sin(t),height);
		float3 CenterPosition = float3(0, 0, 0);
		//XMFLOAT3 Up = XMFLOAT3(0, 1, 0);
		float3 forward = (CenterPosition - EyePosition).SafeNormalize();
		float3 right = float3::UpVector ^ forward;
		float3 up = forward ^ right;
		float asp = 1024 / 720.f;
		float fov = GMath::Deg2Rad * 35.0f;
		float n = 0.1;
		float f = 100.0f;
		float x = 1/GMath::Tan(fov);
		float zf = (1) / (n - f);
		Data.WVP = _Matrix(
			forward.x, right.x, up.x, 0,
			forward.y, right.y, up.y, 0,
			forward.z, right.z, up.z, 0,
			-EyePosition | forward, -EyePosition | right, -EyePosition | up, 1
		);

		_Matrix proj(
			0,			0,			zf,			1,
			x / asp,	0,			0,			0,
			0,			x,			0,			0,
			0,			0,			-f*zf,		0
		);

		//Data.WVP = Data.WVP.GetInverseFast();
		_Matrix transform(
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		);

		_Matrix view(
			forward.x, right.x, up.x, 0,
			forward.y, right.y, up.y, 0,
			forward.z, right.z, up.z, 0,
			-EyePosition | forward, -EyePosition | right, -EyePosition | up, 1
		);

		Data.WVP = transform*view*proj;
		//Data.WVP = Data.WVP.GetTransposed();
		/*Data.WVP = _Matrix(	GMath::Cos(gEngine->GetGameTime()), GMath::Sin(gEngine->GetGameTime()), 0, 0,
							-GMath::Sin(gEngine->GetGameTime()), GMath::Cos(gEngine->GetGameTime()), 0, 0,
							0, 0, 1, 0,
							0 ,0, 5, 1
		) * proj1;*/
		//Data.WVP = Data.WVP.GetTransposed();
		ConstantBuffer->UpdateData(&Data, sizeof(PerObjectData), 0);
		Device->DrawElements(Info);
	}
}

void Mesh::Destroy()
{
	if (VertexBuffer)
		delete VertexBuffer;
	if (IndexBuffer)
		delete IndexBuffer;
	if (PSO)
		delete PSO;
}

void Mesh::MoveForward()
{
	direction = -gEngine->GetDeltaTime();
}

void Mesh::MoveUp()
{
	heightDirection += gEngine->GetDeltaTime();
}

void Mesh::MoveDown()
{
	heightDirection -= gEngine->GetDeltaTime();
}

void Mesh::Stop()
{
	direction = 0;
	heightDirection = 0;
}

void Mesh::MoveBackward()
{
	direction = gEngine->GetDeltaTime();
}
