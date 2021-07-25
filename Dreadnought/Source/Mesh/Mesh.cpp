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
		float3 Color;
	} Data;
	//Data.Color = float4( 1,0,1,0 );
	//float3 EyePosition = float3(-5, 0, 0);
	//float3 CenterPosition = float3(0, 0, 0);
	////XMFLOAT3 Up = XMFLOAT3(0, 1, 0);
	//float3 forward = (CenterPosition - EyePosition).SafeNormalize();
	//float3 right = float3::UpVector ^ forward;
	//float3 up = forward ^ right;
	//float asp = 1024 / 720.f;
	//float fov = GMath::Deg2Rad * 45.0f;
	//float n = 0.1f;
	//float f = 100.0f;
	//float x = GMath::Tan(fov) * asp;
	//float y = GMath::Tan(fov);
	//_Matrix proj(
	//	0,		0,		f / (f - n),1,
	//	n* x,	0,		0,			0,
	//	0,		n* y,	0,			0,
	//	0,		0,		-n*f/(f-n),	0
	//);
	//Data.WVP = _Matrix(
	//	forward.x, right.x, up.x, 0,
	//	forward.y, right.y, up.y, 0,
	//	forward.z, right.z, up.z, 0,
	//	-EyePosition|forward, -EyePosition|right, -EyePosition|up,1
	//);
	////Data.WVP = Data.WVP.GetInverseFast();
	//Data.WVP *= proj;
	//Data.WVP = Data.WVP.GetTransposed();
	//Data.Color = EyePosition;
	Input::BindKeyInputEvent(EKeyCode::KeyBoardW, EKeyInputState::PRESSED, this, &Mesh::MoveForward);
	Input::BindKeyInputEvent(EKeyCode::KeyBoardW, EKeyInputState::RELEASED, this, &Mesh::Stop);
	Input::BindKeyInputEvent(EKeyCode::KeyBoardS, EKeyInputState::PRESSED, this, &Mesh::MoveBackward);
	Input::BindKeyInputEvent(EKeyCode::KeyBoardS, EKeyInputState::RELEASED, this, &Mesh::Stop);
	
	Input::BindKeyInputEvent(EKeyCode::KeyBoardE, EKeyInputState::PRESSED, this, &Mesh::MoveUp);
	Input::BindKeyInputEvent(EKeyCode::KeyBoardE, EKeyInputState::RELEASED, this, &Mesh::Stop);
	Input::BindKeyInputEvent(EKeyCode::KeyBoardQ, EKeyInputState::PRESSED, this, &Mesh::MoveDown);
	Input::BindKeyInputEvent(EKeyCode::KeyBoardQ, EKeyInputState::RELEASED, this, &Mesh::Stop);

	Input::BindKeyInputEvent(EKeyCode::KeyBoardA, EKeyInputState::PRESSED, this, &Mesh::MoveLeft);
	Input::BindKeyInputEvent(EKeyCode::KeyBoardA, EKeyInputState::RELEASED, this, &Mesh::Stop);
	Input::BindKeyInputEvent(EKeyCode::KeyBoardD, EKeyInputState::PRESSED, this, &Mesh::MoveRight);
	Input::BindKeyInputEvent(EKeyCode::KeyBoardD, EKeyInputState::RELEASED, this, &Mesh::Stop);
	
	RHIConstantBuffer* ConstantBuffer = gDevice->CreateConstantBuffer();
	ConstantBuffer->SetElementCount(1);
	ConstantBuffer->SetElementSize(sizeof(PerObjectData));
	gDevice->BuildConstantBuffer(ConstantBuffer);
	ConstantBuffer->UpdateData(&Data, sizeof(PerObjectData), 0);
	PSO->ConstantBuffers.push_back( ConstantBuffer );

	RHIConstantBuffer* Intensity = gDevice->CreateConstantBuffer();
	Intensity->SetElementCount(1);
	Intensity->SetElementSize(4);
	gDevice->BuildConstantBuffer(Intensity);
	float Inst = 2.f;
	Intensity->UpdateData(&Inst, 4, 0);
	PSO->ConstantBuffers.push_back(Intensity);

	gDevice->FlushCommandQueueSync();
}

void Mesh::Draw(IRHIDevice* Device)
{
	distance += direction;
	height += heightDirection;
	angle += angleSpeed;
	SCOPE_EVENT_STR_FORMAT(Draw, "Draw_%s", ObjectName.c_str())
	{
		DrawInfo Info(IndexBuffer, VertexBuffer, PSO);
		auto* ConstantBuffer = PSO->ConstantBuffers[0];

		struct PerObjectData
		{
			_Matrix WVP;
			float3 Color;
		} Data;
		//float3 EyePosition(-5, 0, 0);
		float3 EyePosition(distance*GMath::Cos(angle), distance*GMath::Sin(angle),height);
		Data.Color = EyePosition;
		float3 CenterPosition = float3(0, 0, 0);
		float3 forward = (CenterPosition - EyePosition);
		float3 right = float3::UpVector ^ forward;
		Data.WVP =  LookFromMatrix(EyePosition, forward, float3::UpVector) * 
					PerspectiveMatrix(GMath::Deg2Rad * 90.0f, 1024 / 720.0f, 0.1, 100.0f);
		
		Data.WVP = Data.WVP.GetTransposed();
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
	angleSpeed = 0;
}

void Mesh::MoveBackward()
{
	direction = gEngine->GetDeltaTime();
}

void Mesh::MoveRight()
{
	angleSpeed = gEngine->GetDeltaTime();
}

void Mesh::MoveLeft()
{
	angleSpeed = -gEngine->GetDeltaTime();
}
