//================================================================================
//	�v���C���[�N���X
//	Author:Tsukagoshi Toshiyuki
//================================================================================

//================================================================================
//	include
//================================================================================
#include "Player.h"
#include "Shader2D.h"
#include "Texture.h"
#include "Input\VC.h"

//================================================================================
//	�萔
//================================================================================
//�摜�T�C�Y
#define PLAYER_TEX_SIZE_WIDTH	(100)
#define PLAYER_TEX_SIZE_HEIGHT	(100)

//�A�j���[�V�����̕�����
#define PLAYER_PARTITION_STOP	(1)			
#define PLAYER_PARTITION_MOVE	(1)
#define PLAYER_PARTITION_EAT	(1)
#define PLAYER_PARTITION_GLAD	(1)

//�A�j���[�V�����̃X�s�[�h
#define PLAYER_ANIMATION_SPEED_STOP	(0.03f)
#define PLAYER_ANIMATION_SPEED_MOVE	(0.03f)
#define PLAYER_ANIMATION_SPEED_EAT	(0.03f)
#define PLAYER_ANIMATION_SPEED_GLAD	(0.03f)

//�ړ��X�s�[�h
#define PLAYER_SPEED		(0.5f)

//�����̌�����
#define PLAYER_INERTIA		(0.95f)

//�㉺�ɗh���ړ���
#define PLAYER_SWAY_HEIGHT		(0.02f)

//�㉺�ɗh���X�s�[�h
#define PLAYER_SWAY_SPEED		(0.04f)



//================================================================================
//	�R���X�g���N�^
//================================================================================
Player::Player(int Priority)
{
	//�X�e�[�^�X�̏�����
	_State = PLAYER_STATE_STOP;			//��~��ԂŐ���
	MaxPartition = 0;
	SwayCount = 0;


	AnimationPartition[PLAYER_STATE_STOP] = PLAYER_PARTITION_STOP;
	AnimationPartition[PLAYER_STATE_MOVE] = PLAYER_PARTITION_MOVE;
	AnimationPartition[PLAYER_STATE_EAT]  = PLAYER_PARTITION_EAT;
	AnimationPartition[PLAYER_STATE_GLAD] = PLAYER_PARTITION_GLAD;

	AnimationSpeed[PLAYER_STATE_STOP] = PLAYER_ANIMATION_SPEED_STOP;
	AnimationSpeed[PLAYER_STATE_MOVE] = PLAYER_ANIMATION_SPEED_MOVE;
	AnimationSpeed[PLAYER_STATE_EAT]  =	PLAYER_ANIMATION_SPEED_EAT;
	AnimationSpeed[PLAYER_STATE_GLAD] = PLAYER_ANIMATION_SPEED_GLAD;

}

//================================================================================
//	�v���C���[����
//================================================================================
Player* Player::Create(const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size, const D3DXCOLOR &color, int priority)
{
	Player* player = new Player(priority);
	player->_Pos = pos;
	player->_Size = D3DXVECTOR3(size.x, size.y, 0);
	player->_Color = color;

	player->SetTexture(GetTexture(TEX_EFFECT_GEAR));


	//�A�j���[�V���������̍ő�l
	for (int i = 0; i < PLAYER_STATE_MAX; i++)
	{
		player->MaxPartition = max(player->MaxPartition, player->AnimationPartition[i]);
	}

	return player;
}

//================================================================================
//	�X�V����
//================================================================================
void Player::Update()
{
	VC* vc = VC::Instance();

	PLAYER_STATE oldState;		//�O�t���[���̏��
	oldState = _State;

	//****************************************
	//	�ړ�
	//****************************************
	_Speed += D3DXVECTOR3(vc->LStickX(), vc->LStickY(), 0.0f) * PLAYER_SPEED;
	_Speed *= PLAYER_INERTIA;

	//�㉺�ɂӂ�ӂ�ړ�
	SwayCount += PLAYER_SWAY_SPEED;
	_Speed.y += sinf(SwayCount)*PLAYER_SWAY_HEIGHT;
		
	//****************************************
	//	�A�j���[�V�����֌W
	//****************************************

	//�O��Ə�Ԃ��ς���Ă�����A�j���[�V�����p�J�E���^�����Z�b�g
	if (oldState != _State)
	{
		AnimationCount = 0.0f;
	}
	//�ύX����Ă��Ȃ���΃J�E���^�̍X�V
	else
	{
		AnimationCount += AnimationSpeed[_State];
	}

	//�e�N�X�`��UV�̕ύX
	SetUV(D3DXVECTOR4((float)(PLAYER_TEX_SIZE_WIDTH*(int)AnimationCount),(float)(PLAYER_TEX_SIZE_HEIGHT*_State), 1.0f / (float)MaxPartition, 1.0f));

	Sprite::Update();
}

//================================================================================
//	�`�揈��
//================================================================================
void Player::Draw()
{

	Sprite::Draw();
}

//================================================================================
//	�X�e�[�g�̐ݒ�
//================================================================================
void Player::SetState(int State)
{
	
}