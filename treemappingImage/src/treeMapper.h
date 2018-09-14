#pragma once
#include <glm/common.hpp>
#include <ofMain.h>

typedef glm::tvec3<long long> lvec3;





struct kdNode
{
	int border;
	kdNode *left;
	kdNode *right;
	glm::vec3 average;
	bool direction;
};


class treeMapper
{
	enum objectiveFunction{
		minProportionalSum=0,
		maxSumOfValuesDifference = 1,
		minSumOfValuesDifference = 2,
		minProportionalSum2 = 3,
		minMult = 4,
		minProportinalMult = 5,
		minMax=6,
		minSum=7
	};

	kdNode*root;
	int w;
	int h;
	struct stackItem { kdNode* node; int left; int right; int bottom; int top; float variance; int depth; };
	struct stackDrawItem { kdNode* node; int left; int right;  int bottom; int top; };
	///<summary>
	///Build kd-tree from image
	///</summary>
	void create(int width, int height, ofImage& image, float minVariance,int maxDepth,int minSizeX,int minSizeY, float maxRatio,int objFunc);
	///<summary>
	///Recursive deletion of kd-tree nodes
	///</summary>
	void destroy();
	treeMapper(const treeMapper& r);
	///<summary>
	///Calculate variance and average for given rectangle
	///</summary>
	static void varianceAndMean(lvec3 *sT, lvec3 *sST, int width, int l, int r, int b, int t, float & variance, glm::vec3& average);
public:
	treeMapper(int width, int height, ofImage& image,float minVariance,  int maxDepth, int minSizeX, int minSizeY,float maxRatio,int objFunc);
	///<summary>
	///Recursivly find leaf reactangle nodes, calculate coordinates for sides and draw with average color
	///</summary>
	void draw(float scaleW, float scaleH,int shiftX,int shiftY);
	~treeMapper();
}; 