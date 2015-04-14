#include "Mouse.h"


CMouse* CMouse::Self = new CMouse;
HWND CMouse::m_phWnd;

CMouse::~CMouse()
{
	if (InputDevice != nullptr)
	{
		InputDevice->Release();
		InputDevice = nullptr;
	}
}

//=============================================================================
//������
//=============================================================================
HRESULT CMouse::Init(LPDIRECTINPUT8 DInput,HINSTANCE hInstance,HWND hWnd)
{
	DIPROPDWORD dipdw;
	// �uDirectInput�f�o�C�X�v�I�u�W�F�N�g�̍쐬
	if( FAILED( DInput->CreateDevice( GUID_SysMouse,&InputDevice, NULL ) ) )
	{
		MessageBox(hWnd,"�}�E�X�̃C���v�b�g�f�o�C�X�̍쐬�Ɏ��s","ERROR!",MB_OK|MB_ICONERROR);
		return E_FAIL;
	} 
	
	// �f�o�C�X���}�E�X�ɐݒ�
	if( FAILED( InputDevice->SetDataFormat( &c_dfDIMouse2 ) ) )
	{
		MessageBox(hWnd,"�f�[�^�t�H�[�}�b�g�̐ݒ�Ɏ��s","ERROR!",MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
	// �������x���̐ݒ�
	if( FAILED( InputDevice->SetCooperativeLevel( 
		hWnd,DISCL_NONEXCLUSIVE | DISCL_FOREGROUND )) )
	{
		MessageBox(hWnd,"�������[�h�̐ݒ�Ɏ��s","ERROR!",MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
	// the header
	dipdw.diph.dwSize = sizeof(dipdw);
	dipdw.diph.dwHeaderSize = sizeof(dipdw);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = DIPROPAXISMODE_REL;
	// the data
	dipdw.dwData = 16;
	
	InputDevice->SetProperty(DIPROP_AXISMODE,&dipdw.diph);

	// �f�o�C�X���u�擾�v����
	InputDevice->Acquire();

	m_phWnd = hWnd;
	WheelStatus = WHEEL_NEUTRAL;

	m_phWnd = hWnd;

	_MoveX = _MoveY = 0;
	return S_OK;
}

void CMouse::Uninit(void)
{
	delete Self;
}

//=============================================================================
//�X�V
//=============================================================================
void CMouse::Update(void)
{
	DIMOUSESTATE2 CurrentMouseStatus;
	if(FAILED(InputDevice->GetDeviceState(sizeof(DIMOUSESTATE2),&CurrentMouseStatus)))
	{
		InputDevice->Acquire();
		return ;
	}
	POINT Pos;
	GetCursorPos(&Pos);
	ScreenToClient(m_phWnd,&Pos);	//���΍��W���΍��W�ɕϊ�

	_MoveX = (float)(Pos.x - MouseStatus.lX);
	_MoveY = (float)(Pos.y - MouseStatus.lY);
	MouseStatus.lX = Pos.x;
	MouseStatus.lY = Pos.y;
	if(CurrentMouseStatus.lZ - MouseStatus.lZ == 0)
	{
		WheelStatus = WHEEL_NEUTRAL;
	}
	else if(CurrentMouseStatus.lZ - MouseStatus.lZ > 0)
	{
		WheelStatus = WHEEL_UP;
	}
	else
	{
		WheelStatus = WHEEL_DOWN;
	}

	
	//�L�[���͔���
	for(int cnt=0;cnt<3;cnt++)
	{
		TriggerMouse[cnt] = (MouseStatus.rgbButtons[cnt]^CurrentMouseStatus.rgbButtons[cnt])&CurrentMouseStatus.rgbButtons[cnt];
		ReleaseMouse[cnt] = (MouseStatus.rgbButtons[cnt]^CurrentMouseStatus.rgbButtons[cnt])&~CurrentMouseStatus.rgbButtons[cnt];
		MouseStatus.rgbButtons[cnt] = CurrentMouseStatus.rgbButtons[cnt];
	}
}

//=============================================================================
// �}�E�X��X���W�擾
//=============================================================================
float CMouse::PosX(void)
{
	return (float)MouseStatus.lX;
}
//=============================================================================
// �}�E�X��Y���W�擾
//=============================================================================
float CMouse::PosY(void)
{
	return (float)MouseStatus.lY;
}
//=============================================================================
// �}�E�X�̃E�B�[�����擾
//=============================================================================
int CMouse::Wheel(void)
{
	return WheelStatus;
}
//=============================================================================
// �}�E�X�̃v���X�擾
//=============================================================================
bool CMouse::Press(int nKey)
{
	return MouseStatus.rgbButtons[nKey] ? true:false;
}
//=============================================================================
// �}�E�X�̃g���K�[�擾
//=============================================================================
bool CMouse::Trigger(int nKey)
{
	return TriggerMouse[nKey] ? true:false;
}
//=============================================================================
// �}�E�X�̃����[�X�擾
//=============================================================================
bool CMouse::Release(int nKey)
{
	return ReleaseMouse[nKey] ? true:false;
}
//=============================================================================
// �}�E�X�̐��������̈ړ��ʎ擾
//=============================================================================
float CMouse::MoveX(void)
{
	return _MoveX;
}
//=============================================================================
// �}�E�X�̐��������̈ړ��ʎ擾
//=============================================================================
float CMouse::MoveY(void)
{
	return _MoveY;
}