#pragma once
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <ofMain.h>

typedef glm::tvec3<long long> lvec3;
using fvec3 = glm::vec3;

struct kdNode
{
	fvec3 average;
	int border;
	kdNode* left;
	kdNode* right;
	int depth;
	bool direction;
};

class edgeDetectionMapper
{
	kdNode* root;
	int w;
	int h;
	int depth;
	fvec3 computeAverage(const std::vector<fvec3>& summedTable, int x0, int x1, int y0, int y1);
public:
	edgeDetectionMapper(int width, int height, const ofImage& image, float splitThreshold, int maxDepth, int minSizeX, int minSizeY);
	void createMaxEdgeDetection(const ofImage& image, const std::vector<float>& edgeImage, float splitThreshold, int maxDepth, int minSizeX, int minSizeY);
	void draw();
	void drawDepth(float scaleW, float scaleH, int shiftX, int shiftY);
	void destroy();
	~edgeDetectionMapper();
};


class treeMapper
{
	kdNode* root;
	int w;
	int h;
	int maxDepth;
	struct stackItem { kdNode* node; int left; int right; int bottom; int top; float variance; int depth; };
	struct stackDrawItem { kdNode* node; int left; int right;  int bottom; int top; };

	///<summary>
	///Build kd-tree from image
	///</summary>
	void create(int width, int height, ofImage& image, float minVariance, int maxDepth, int minSizeX, int minSizeY, float maxRatio, int objFunc);
	///<summary>
	///Recursive deletion of kd-tree nodes
	///</summary>
	void destroy();
	treeMapper(const treeMapper& r);
	///<summary>
	///Calculate variance and average for given rectangle
	///</summary>
	static void varianceAndMean(lvec3 *sT, lvec3 *sST, int width, int l, int r, int b, int t, float & variance, fvec3& average);
public:
	enum objectiveFunction {
		minProportionalSum = 0,
		maxSumOfValuesDifference = 1,
		minSumOfValuesDifference = 2,
		minProportionalSum2 = 3,
		minMult = 4,
		minProportinalMult = 5,
		minMax = 6,
		minSum = 7
	};
	treeMapper(int width, int height, ofImage& image, float minVariance, int maxDepth, int minSizeX, int minSizeY, float maxRatio, int objFunc);
	///<summary>
	///Recursivly find leaf reactangle nodes, calculate coordinates for sides and draw with average color
	///</summary>
	void draw(float scaleW, float scaleH, int shiftX, int shiftY);
	///<summary>
	///Draw rectangle nodes with color, representing node's depth
	///</summary>
	void drawDepth(float scaleW, float scaleH, int shiftX, int shiftY);
	~treeMapper();
};