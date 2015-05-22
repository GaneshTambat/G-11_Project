#include "Wall.h"
using namespace std;

List<Wall> Wall::_HitList;

Wall::Wall(int priority) :Sprite(priority)
{
	SelfIterator = nullptr;
	SelfIterator = _HitList.Add(this);
}

Wall::~Wall()
{
	_HitList.Delete(SelfIterator);
}

Wall* Wall::Create(const D3DXVECTOR2& pos,const D3DXVECTOR2& size,TEX texId,int priority)
{
	Wall* wall = new Wall(priority);
	if (wall == nullptr){ return wall; }

	wall->_Pos = Vector3(pos,0);
	wall->_Size = Vector3(size,1.0f);
	wall->Texture = GetTexture(texId);
	float widthPer = size.x/size.y;
	wall->uv.z = widthPer;
	wall->uv.y = 0.01f;
	wall->uv.w = 0.99f;
	return wall;
}