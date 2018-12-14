#pragma once

#include "d3dapp.h"

class DemoApp : public D3DApp
{
public:
	DemoApp(HINSTANCE);
	~DemoApp();

	bool init() override;
	void on_resize(int w, int h) override;
	void update_scene(float dt) override;
	void draw_scene() override;
};
