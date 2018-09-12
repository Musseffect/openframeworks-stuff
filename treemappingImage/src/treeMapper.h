#pragma once
#include <glm/common.hpp>
#include <ofMain.h>

typedef glm::tvec3<long long> lvec3;





struct kdNode
{
	int border;
	kdNode *left;
	kdNode *right;
	glm::vec3 mean;
	bool swap;
};


class treeMapper
{
	enum objectiveFunction{minMax=0,minSum=1,minProportionalSum=2,minMult=3,minProportinalMult=4,maxMeanDifference=5};

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
public:
	treeMapper(int width, int height, ofImage& image,float minVariance,  int maxDepth, int minSizeX, int minSizeY,float maxRatio,int objFunc);
	///<summary>
	///Calculate variance and mean for given rectangle
	///</summary>
	static void varianceAndMean(lvec3 *sT, lvec3 *sST, int width, int l, int r, int b, int t, float & variance, glm::vec3& mean);
	
	///<summary>
	///Recursivly find leaf reactangle nodes, calculate coordinates for sides and draw with mean color
	///</summary>
	void draw(float scaleW, float scaleH,int shiftX,int shiftY);
	~treeMapper();
};