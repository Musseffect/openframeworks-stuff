#pragma once
#include <glm/common.hpp>
#include <ofMain.h>

typedef glm::tvec3<long long> lvec3;



struct kdNode
{
	int border;
	glm::vec3 mean;
	kdNode *left;
	kdNode *right;
};

class treeMapper
{
	kdNode*root;
	int w;
	int h;
	struct stackItem { kdNode* node; int left; int right; int bottom; int top; bool swap; float variance; };
	struct stackDrawItem { kdNode* node; int left; int right;  int bottom; int top; bool swap; };
	void create(int width, int height, ofImage& image);
	void destroy();
public:
	treeMapper(int width, int height, ofImage& image);
	static void varianceAndMean(lvec3 *sT, lvec3 *sST, int width, int l, int r, int b, int t, float & variance, glm::vec3& mean);

	void draw(float scaleW, float scaleH);
	~treeMapper();
};