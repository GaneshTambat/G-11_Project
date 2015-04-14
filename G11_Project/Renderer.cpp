//=============================================================================
// �C���N���[�h
//=============================================================================
#include "Renderer.h"
#include "Fade.h"
#include "Camera.h"
#include "Camera2D.h"
#include "Manager.h"
#include "ShaderManager.h"
#include "Shader2D.h"
#include "Shader3D.h"
#include "Shimmer.h"

#include "Sprite.h"
#include "Polygon.h"
#include "Shaim3DEffect.h"
#include "ShimmerParticle2D.h"
#include "Orbit2D.h"
#include "Font.h"

#include "ScreenRender.h"
#include "BlurFilter.h"
#include "Loading.h"
//#include "Model.h"
#include "Light.h"

//=============================================================================
// �O���[�o���ϐ�
//=============================================================================
LPDIRECT3D9				CRenderer::g_D3D = NULL;
LPDIRECT3DDEVICE9		CRenderer::Device = NULL;
ShaderManager*			CRenderer::_ShaderManager = nullptr;
LPDIRECT3DVERTEXBUFFER9 CRenderer::VtxBuff[2] = { nullptr };
CFade*					CRenderer::Fade = nullptr;
ScreenRender*			CRenderer::Screen = nullptr;
//=============================================================================
// �R���X�g���N�^
//=============================================================================
CRenderer::CRenderer()
{

}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CRenderer::~CRenderer()
{
	
}
//=============================================================================
// ������
//=============================================================================
HRESULT CRenderer::Init(HWND hWnd,BOOL bWindow)
{
	D3DPRESENT_PARAMETERS d3dpp;
    D3DDISPLAYMODE d3ddm;
	Window* window = Window::Instance();

	// Direct3D�I�u�W�F�N�g�̐���
	g_D3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(g_D3D == NULL)
	{
		return E_FAIL;
	}

	// ���݂̃f�B�X�v���C���[�h���擾
    if(FAILED(g_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// �f�o�C�X�̃v���[���e�[�V�����p�����[�^�̐ݒ�
	ZeroMemory(&d3dpp, sizeof(d3dpp));							// ���[�N���[���N���A
	d3dpp.BackBufferCount			= 1;						// �o�b�N�o�b�t�@�̐�
	d3dpp.BackBufferWidth			= (UINT)window->WindowSize().x;				// �Q�[����ʃT�C�Y(��)
	d3dpp.BackBufferHeight			= (UINT)window->WindowSize().y;			// �Q�[����ʃT�C�Y(����)
	d3dpp.BackBufferFormat			= D3DFMT_UNKNOWN;			// �o�b�N�o�b�t�@�̃t�H�[�}�b�g�͌��ݐݒ肳��Ă�����̂��g��
	d3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;	// �f���M���ɓ������ăt���b�v����
	d3dpp.Windowed					= bWindow;					// �E�B���h�E���[�h
	d3dpp.EnableAutoDepthStencil	= TRUE;						// �f�v�X�o�b�t�@�i�y�o�b�t�@�j�ƃX�e���V���o�b�t�@���쐬

	if(FAILED(g_D3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,D3DFMT_DXT1,D3DFMT_R32F,D3DFMT_D24S8)))
	{
		d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;				// �f�v�X�o�b�t�@�Ƃ���16bit���g��
	}
	else
	{
		d3dpp.AutoDepthStencilFormat	= D3DFMT_D24S8;				// �f�v�X�o�b�t�@�Ƃ���24bit���g��
	}
	

	if(bWindow)
	{// �E�B���h�E���[�h
		d3dpp.BackBufferFormat           = D3DFMT_UNKNOWN;					// �o�b�N�o�b�t�@
		d3dpp.FullScreen_RefreshRateInHz = 0;								// ���t���b�V�����[�g
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;	// �C���^�[�o��
	}
	else
	{// �t���X�N���[�����[�h
		d3dpp.BackBufferFormat           = D3DFMT_R5G6B5;					// �o�b�N�o�b�t�@
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;			// ���t���b�V�����[�g
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;		// �C���^�[�o��
	}

	// �f�o�C�X�I�u�W�F�N�g�̐���
	// [�f�o�C�X�쐬����]<�`��>��<���_����>���n�[�h�E�F�A�ōs�Ȃ�
	if(FAILED(g_D3D->CreateDevice(D3DADAPTER_DEFAULT,						// �f�B�X�v���C�A�_�v�^
									D3DDEVTYPE_HAL,							// �f�B�X�v���C�^�C�v
									hWnd,									// �t�H�[�J�X����E�C���h�E�ւ̃n���h��
									D3DCREATE_HARDWARE_VERTEXPROCESSING,	// �f�o�C�X�쐬����̑g�ݍ��킹
									&d3dpp,									// �f�o�C�X�̃v���[���e�[�V�����p�����[�^
									&Device)))							// �f�o�C�X�C���^�[�t�F�[�X�ւ̃|�C���^
	{
		// ��L�̐ݒ肪���s������
		// [�f�o�C�X�쐬����]<�`��>���n�[�h�E�F�A�ōs���A<���_����>��CPU�ōs�Ȃ�
		if(FAILED(g_D3D->CreateDevice(D3DADAPTER_DEFAULT, 
										D3DDEVTYPE_HAL, 
										hWnd, 
										D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
										&d3dpp,
										&Device)))
		{
			// ��L�̐ݒ肪���s������
			// [�f�o�C�X�쐬����]<�`��>��<���_����>��CPU�ōs�Ȃ�
			if(FAILED(g_D3D->CreateDevice(D3DADAPTER_DEFAULT, 
											D3DDEVTYPE_REF,
											hWnd, 
											D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
											&d3dpp,
											&Device)))
			{
				// ���������s
				return E_FAIL;
			}
		}
	}
	window->SetDevice(Device);
	window->Show();

	// �����_�[�X�e�[�g�p�����[�^�̐ݒ�
	Device->SetRenderState(D3DRS_CLIPPING,TRUE);	//�N���b�s���O�L��

	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// ���ʂ��J�����O
	Device->SetRenderState(D3DRS_ZENABLE, TRUE);						// Z�o�b�t�@���g�p
	Device->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);				// ���u�����h���s��
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// ���\�[�X�J���[�̎w��
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// ���f�X�e�B�l�[�V�����J���[�̎w��
	
	// �T���v���[�X�e�[�g�p�����[�^�̐ݒ�
	//Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// �e�N�X�`���A�h���b�V���O���@(U�l)��ݒ�
	//Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// �e�N�X�`���A�h���b�V���O���@(V�l)��ݒ�
	//Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	// �e�N�X�`���k���t�B���^���[�h��ݒ�
	//Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// �e�N�X�`���g��t�B���^���[�h��ݒ�

	// �e�N�X�`���X�e�[�W�X�e�[�g�̐ݒ�
	Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);	// �A���t�@�u�����f�B���O����
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// �ŏ��̃A���t�@����
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);	// �Q�Ԗڂ̃A���t�@����


	window->InitVD();
	CCamera* camera = CCamera::Create(D3DXVECTOR3(0,300.0f,-500.0f),D3DXVECTOR3(0,0,0));
	camera->SetSize(D3DXVECTOR2(THREE_D_AREA_WIDTH,THREE_D_AREA_HEIGHT));

	CCamera2D* camera2D = CCamera2D::Create(D3DXVECTOR3(0.0f,0.0f,0));

	if (FAILED(Device->CreateVertexBuffer(sizeof(CUSTOM_VTX)* 4 * 2,D3DUSAGE_WRITEONLY,FVF_CUSTOM,D3DPOOL_MANAGED,&VtxBuff[0],NULL)))
	{
		return E_FAIL;
	}
	if (FAILED(Device->CreateVertexBuffer(sizeof(VERTEX_2D)* 4 * 2,D3DUSAGE_WRITEONLY,FVF_VERTEX_2D,D3DPOOL_MANAGED,&VtxBuff[1],NULL)))
	{
		return E_FAIL;
	}

	_ShaderManager = ShaderManager::Instance();
	_ShaderManager->Init();
	
	Init3D();
	Init2D();

	GetDevice()->CreateTexture((UINT)window->WindowSize().x,(UINT)window->WindowSize().y,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&Texture[0],NULL);
	Texture[0]->GetSurfaceLevel(0,&Surface[0]);

	GetDevice()->CreateTexture((UINT)window->WindowSize().x,(UINT)window->WindowSize().y,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&Texture[1],NULL);
	Texture[1]->GetSurfaceLevel(0,&Surface[1]);



	Screen = new ScreenRender;
	Screen->Init();
	Loading::SetRenderer(Screen);

	String::Initialize();

	Fade = CFade::Instance();
	Fade->Init();

	return S_OK;
}

void CRenderer::Init2D(void)
{
	Shaim3DEffect::Initialize();
	VERTEX_2D* Vtx2;
	Window* window = Window::Instance();

	VtxBuff[1]->Lock(0,0,(void**)&Vtx2,0);

	Vtx2[0].vtx = D3DXVECTOR3(-0.5f,-0.5f,0);
	Vtx2[1].vtx = D3DXVECTOR3(0.5f,-0.5f,0);
	Vtx2[2].vtx = D3DXVECTOR3(-0.5f,0.5f,0);
	Vtx2[3].vtx = D3DXVECTOR3(0.5f,0.5f,0);

	Vtx2[0].tex = D3DXVECTOR2(0,0);
	Vtx2[1].tex = D3DXVECTOR2(1,0);
	Vtx2[2].tex = D3DXVECTOR2(0,1);
	Vtx2[3].tex = D3DXVECTOR2(1,1);

	VtxBuff[1]->Unlock();

	Device->CreateTexture((UINT)window->WindowSize().x,(UINT)window->WindowSize().y,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&Texture2D,NULL);
	Texture2D->GetSurfaceLevel(0,&Surface2D);
}

//=============================================================================
//3D�I�u�W�F�N�g�̏�����
//=============================================================================
void CRenderer::Init3D(void)
{
	Window* window = Window::Instance();
	D3DXMATRIX Mtx;
	D3DXMatrixIdentity(&WorldMtx);
	D3DXMatrixScaling(&Mtx,THREE_D_AREA_WIDTH,THREE_D_AREA_HEIGHT,1.0f);
	D3DXMatrixMultiply(&WorldMtx,&WorldMtx,&Mtx);
	D3DXMatrixTranslation(&Mtx,THREE_D_AREA_WIDTH/2.0f,THREE_D_AREA_HEIGHT/2.0f,0);
	D3DXMatrixMultiply(&WorldMtx,&WorldMtx,&Mtx);

	D3DXVECTOR4 vec = -D3DXVECTOR4(0.15f,-0.8f,0.05f,0.0f);
	D3DXVec4Normalize(&vec,&vec);

	Shader3D::Instance()->SetFloatArray(Shader3D::LIGHT_VEC,vec,4);
	Shader3D::Instance()->SetFloatArray(Shader3D::LIGHT_COLOR,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),4);
	CUSTOM_VTX* Vtx;
	VtxBuff[0]->Lock(0,0,(void**)&Vtx,0);

	Vtx[0].vtx = D3DXVECTOR3(-0.5f,0.5f,0);
	Vtx[1].vtx = D3DXVECTOR3(0.5f,0.5f,0);
	Vtx[2].vtx = D3DXVECTOR3(-0.5f,-0.5f,0);
	Vtx[3].vtx = D3DXVECTOR3(0.5f,-0.5f,0);

	Vtx[0].nor = D3DXVECTOR3(0,0,-1.0f);
	Vtx[1].nor = D3DXVECTOR3(0,0,-1.0f);
	Vtx[2].nor = D3DXVECTOR3(0,0,-1.0f);
	Vtx[3].nor = D3DXVECTOR3(0,0,-1.0f);

	Vtx[0].tex = D3DXVECTOR2(0,0);
	Vtx[1].tex = D3DXVECTOR2(1,0);
	Vtx[2].tex = D3DXVECTOR2(0,1);
	Vtx[3].tex = D3DXVECTOR2(1,1);

	VtxBuff[0]->Unlock();

	GetDevice()->CreateTexture((UINT)window->WindowSize().x,(UINT)window->WindowSize().y,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&Texture3D,NULL);
	Texture3D->GetSurfaceLevel(0,&Surface3D);
}
//=============================================================================
// �I������
//=============================================================================
void CRenderer::Uninit(void)
{
	String		::ReleaseAll();
	Sprite		::ReleaseAll();
	CPolygon	::ReleaseAll();
	Shaim3DEffect::ReleaseAll();
	Orbit2D	::ReleaseAll();
	
	//CModel::ReleaseAll();

	CFade::Finalize();
	_ShaderManager->Uninit();
	_ShaderManager = nullptr;
	SafeDelete(Screen);
	SafeRelease(VtxBuff[0]);
	SafeRelease(VtxBuff[1]);
	
	SafeRelease(Surface[0]);
	SafeRelease(Surface[1]);
	SafeRelease(Surface3D);
	SafeRelease(Surface2D);
	
	SafeRelease(Texture[0]);
	SafeRelease(Texture[1]);
	SafeRelease(Texture3D);
	SafeRelease(Texture2D);

	SafeRelease(g_D3D);
	SafeRelease(Device);

}
//=============================================================================
// �X�V
//=============================================================================
void CRenderer::Update(void)
{
	String		::UpdateAll();
	Sprite		::UpdateAll();
	CPolygon	::UpdateAll();
	Shaim3DEffect::UpdateAll();
	ShimmerParticle2D::UpdateAll();//�z���p�[�e�B�N�����X�V����
	Orbit2D	::UpdateAll();

	Fade->Update();

	//CModel		::UpdateAll();
}
//=============================================================================
// �`��
//=============================================================================
void CRenderer::Draw(void)
{
#ifdef _DEBUG
	static float AVtime = 0;
#endif
	LPDIRECT3DSURFACE9 OldSurface = nullptr;

	Device->GetRenderTarget(0,&OldSurface);
	
	if(SUCCEEDED(Device->BeginScene()))	//�`��J�n
	{
		//�J�������Z�b�g
		CCamera::Set(0);
		CCamera2D::Set(0);

		Render3D();
		Render2D();
		
		//�����_�����O���[�h��K�p���ăX�N���[����`��
		Device->SetRenderTarget(0,Surface[0]);
		Draw3DScreen(Texture3D);
		Device->SetRenderTarget(0,Surface[1]);

		if (ShimmerParticle2D::Num() > 0)
		{
			LPDIRECT3DTEXTURE9 tex = Shimmer::Instance()->Render(Texture[0]);
			Screen->RenderScreen(tex);
		}
		else
		{
			Screen->RenderScreen(Texture[0]);
		}
		Screen->RenderScreen(Texture2D,2,false);
		
		//�����_�����O���[�h��K�p���Ȃ����̂͂������ŕ`��
		Sprite::DrawAll(Sprite::LAYER_INTERFACE);
		Fade->Draw();

#ifdef _DEBUG
		DebugProc::Draw();
#endif

		//�Ō�ɂ܂Ƃ߂����̂�`�悷��
		Device->SetRenderTarget(0,OldSurface);

		SafeRelease(OldSurface);

		Screen->RenderWindow(Texture[1]);
		Device->EndScene();	//�`��I��
	}
	// �o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
	Device->Present(NULL,NULL,NULL,NULL);

#ifdef _DEBUG
	
#endif

}

//=============================================================================
//3D�|���S����`������
//=============================================================================
void CRenderer::SetStream3D(void)
{
	Device->SetStreamSource(0,VtxBuff[0],0,sizeof(CUSTOM_VTX));
	Window::Instance()->SetVtxDecl(VTX_DECL_CUSTOM);
}

//=============================================================================
//2D�|���S����`������
//=============================================================================
void CRenderer::SetStream2D(void)
{
	Device->SetStreamSource(0,VtxBuff[1],0,sizeof(VERTEX_2D));
	Window::Instance()->SetVtxDecl(VTX_DECL_2D);
}

//=============================================================================
//3D�I�u�W�F�N�g��`��
//=============================================================================
void CRenderer::Render3D(void)
{
	Device->SetRenderTarget(0,Surface3D);
	Device->Clear(0,NULL,(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),D3DCOLOR_RGBA(255,255,255,0),1.0f,0);

	SetStream3D();
	CPolygon::DrawAll();;
}

void CRenderer::Draw3DScreen(LPDIRECT3DTEXTURE9 tex)
{
	Device->Clear(0,NULL,(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),D3DCOLOR_RGBA(255,255,255,0),1.0f,0);
	Shader2D* shader = Shader2D::Instance();
	if (tex != nullptr)
	{
		shader->SetTexture(tex);
	}
	SetStream2D();
	shader->SetFloatArray(Shader2D::SIZE,D3DXVECTOR3(THREE_D_AREA_WIDTH,THREE_D_AREA_HEIGHT,1.0f),3);
	shader->SetMatrix(Shader2D::WORLD_MTX,WorldMtx);
	
	shader->SetFloatArray(Shader2D::DIFFUSE,WHITE(1.0f),4);
	
	shader->DrawBegin();

	shader->Draw(Shader2D::NORMAL,D3DPT_TRIANGLESTRIP);

	shader->DrawEnd();
}
//=============================================================================
//2D�I�u�W�F�N�g��`��
//=============================================================================
void CRenderer::Render2D(void)
{
	Device->SetRenderTarget(0,Surface2D);
	Device->Clear(0,NULL,(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),D3DCOLOR_RGBA(0,0,0,0),1.0f,0);

	SetStream2D();
	Shaim3DEffect::DrawAll();
	Orbit2D::DrawAll();
	for (int cnt = 0;cnt < Sprite::LAYER_INTERFACE;cnt++)
	{
		Sprite::DrawAll(cnt);
	}

	Shader2D::Instance()->SetFloatArray(Shader2D::UV,D3DXVECTOR4(0,0,1.0f,1.0f),4);
}

//=============================================================================
//�t�F�[�h��ݒ�
//=============================================================================
bool CRenderer::SetFade(float frame,CFade::FADE mode,D3DXCOLOR color)
{
	if (Fade)
	{
		if (Fade->Active() == false)
		{
			Fade->Start(frame,mode,color);
			return true;
		}
	}
	return false;
}
//=============================================================================
//�����_�����O���[�h��ύX
//=============================================================================
void CRenderer::SetRenderMode(short num)
{
	Screen->SetMode((int)num);
}
//=============================================================================
// FPS�`��
//=============================================================================
#ifdef _DEBUG
void CRenderer::DrawFPS(void)
{

}
#endif