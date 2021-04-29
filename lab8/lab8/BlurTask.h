#pragma once
#include "ITask.h"
#include "bmp.h"


struct BlurParams {
	bitmap* bmp;
	uint32_t startHeight;
	uint32_t endHeight;
	uint32_t startWidth;
	uint32_t endWidth;
};

class BlurTask : public ITask
{
public:
	BlurTask(int radius, const BlurParams& blurPrms);
	void Execute() override;

private:
	BlurParams m_params;
	int m_radius;

};
