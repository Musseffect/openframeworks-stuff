#include "treeMapper.h"



void treeMapper::draw(float scaleW, float scaleH)
{

	std::stack<stackDrawItem*> stack;
	stack.push(new stackDrawItem{ root ,0,w,0,h,false });
	while (!stack.empty())
	{
		stackDrawItem* current = stack.top();
		stack.pop();
		if (current->node->border == -1)
		{
			glm::vec3 meanColor = current->node->mean;
			ofColor color(meanColor.x, meanColor.y, meanColor.z);
			ofSetColor(color);
			ofDrawRectangle(current->left*scaleW, current->bottom*scaleH, (current->right - current->left)*scaleW, (current->top - current->bottom)*scaleH);
		}
		else
		{
			if (current->swap)
			{
				stack.push(new stackDrawItem{ current->node->left, current->left,current->node->border,current->bottom,current->top,!current->swap });
				stack.push(new stackDrawItem{ current->node->right,current->node->border,current->right,current->bottom,current->top,!current->swap });
			}
			else
			{
				stack.push(new stackDrawItem{ current->node->left, current->left,current->right,current->bottom,current->node->border,!current->swap });
				stack.push(new stackDrawItem{ current->node->right,current->left,current->right,current->node->border,current->top,!current->swap });
			}
		}
		delete current;
	}

}


inline void treeMapper::create(int width, int height, ofImage& image)
{
	//build summation tables
	lvec3 *summedTable = new lvec3[width*height];
	lvec3 *summedSqrTable = new lvec3[width*height];
	for (int j = 0; j < height; j++)
	{
		int index = j * width;
		for (int i = 0; i < width; i++)
		{
			ofColor color = image.getColor(i, j);
			lvec3 value(color.r, color.g, color.b);
			lvec3  valuesqr = value * value;
			if (i > 0 && j > 0)
			{
				value -= summedTable[i - 1 + index - width];
				valuesqr -= summedSqrTable[i - 1 + index - width];
			}
			if (i > 0)
			{ 
				value += summedTable[i - 1 + index];
				valuesqr += summedSqrTable[i - 1 + index];
			}
			if (j > 0)
			{
				value += summedTable[i + index - width];
				valuesqr += summedSqrTable[i + index - width];
			}
			summedTable[i + index] = value;
			summedSqrTable[i + index] = valuesqr;
		}
	}
	root = new kdNode{-1,glm::vec3(),nullptr,nullptr};
	lvec3 _t = summedTable[width*height - 1];
	float _n = float(width*height);
	root->mean = glm::vec3(_t.x / _n, _t.y / _n, _t.z / _n);
	//build kd-tree
	std::stack<stackItem*> buildStack;
	lvec3 s1 = _t;
	lvec3 s2 = summedSqrTable[width*height - 1];
	glm::vec3 variance3(double(s2.x / _n) - pow(double(s1.x / _n),2.0) ,
		double(s2.y / _n) - pow(double(s1.y / _n), 2.0),
		double(s2.z / _n) - pow(double(s1.z / _n), 2.0)
	);
	float variance = length(variance3);
	buildStack.push(new stackItem{ root,0,width,0,height,false,variance });
	//std::vector<kdNode*> leafNodes;
	while (!buildStack.empty())
	{
		stackItem* current = buildStack.top();
		buildStack.pop();
		float minSum = FLT_MAX;
		int index = -1;
		float lv = 0;
		float rv = 0;
		glm::vec3 lMean;
		glm::vec3 rMean;
		if (current->swap)
		{

			for (int i = current->left + 1; i < current->right; i++)//x
			{
				float leftVariance = 0.f;
				float rightVariance = 0.f;

				glm::vec3 _lMean;
				glm::vec3 _rMean;
				varianceAndMean(summedTable, summedSqrTable, width, current->left, i, current->bottom, current->top, leftVariance, _lMean);
				varianceAndMean(summedTable, summedSqrTable, width, i, current->right, current->bottom, current->top, rightVariance, _rMean);
				float sum = abs(leftVariance+rightVariance);
				if (sum < minSum)
				{
					index = i;
					minSum = sum;
					lv = leftVariance;
					rv = rightVariance;
					lMean = _lMean;
					rMean = _rMean;
				}
				//if sum of variances is less than min set index
			}
			if (index != -1)
			{
				current->node->border = index;
				current->node->left = new kdNode{ -1,lMean,nullptr,nullptr };
				current->node->right = new kdNode{ -1,rMean,nullptr,nullptr };
				if (lv != 0.f && (current->top - current->bottom > 2) && (index - current->left> 2))
				{
					buildStack.push(new stackItem{ current->node->left,current->left,index,current->bottom,current->top,
						!current->swap,lv });
				}
				if (rv != 0.f && (current->top - current->bottom > 2) && (current->right-index > 2))
				{
					buildStack.push(new stackItem{ current->node->right,index,current->right,current->bottom,current->top,
						 !current->swap ,rv });
				}
				//if variance of a part equals zero dont push it in stack
				//if right-left<2 dont push

			}
		}
		else
		{
			for (int i = current->bottom+1; i < current->top; i++)//y
			{
				float leftVariance = 0.f;
				float rightVariance = 0.f;

				glm::vec3 _lMean;
				glm::vec3 _rMean;
				varianceAndMean(summedTable, summedSqrTable, width, current->left, current->right, current->bottom, i, leftVariance, _lMean);
				varianceAndMean(summedTable, summedSqrTable, width, current->left, current->right, i, current->top, rightVariance, _rMean);
				float sum = abs(leftVariance+rightVariance);
				if (sum < minSum)
				{
					index = i;
					minSum = sum;
					lv = leftVariance;
					rv = rightVariance;
					lMean = _lMean;
					rMean = _rMean;
				}
				//if sum of variances is less than min set index
			}
			if (index != -1)
			{
				current->node->border = index;
				current->node->left = new kdNode{ -1,lMean,nullptr,nullptr };
				current->node->right = new kdNode{ -1,rMean,nullptr,nullptr };
				if (lv != 0.f && (current->right - current->left > 2) && (index - current->bottom > 2))
				{
					buildStack.push(new stackItem{ current->node->left,current->left,current->right,current->bottom,index,
						!current->swap ,lv });
				}
				if (rv != 0.f && (current->right - current->left > 2) && (current->top-index > 2))
				{
					buildStack.push(new stackItem{ current->node->right,current->left,current->right,index,current->top,
						!current->swap ,rv });
				}
				//if variance of a part equals zero dont push it in stack
				//if right-left<2 dont push
			}
		}
		delete current;
		//for each node
		//find splitting that creates minimum difference between variances
		//split
		//continue


	}
	delete[]summedTable;
	delete[]summedSqrTable;

}

inline void treeMapper::destroy()
{
	//delete tree
	std::stack<kdNode*> stack;
	stack.push(root);
	while (!stack.empty())
	{
		kdNode * current = stack.top();
		stack.pop();
		if (current->left != nullptr)
			stack.push(current->right);
		if (current->left != nullptr)
			stack.push(current->right);
		delete current;
	}
}

treeMapper::treeMapper(int width, int height, ofImage& image)
{
	w = width;
	h = height;
	create(width, height, image);
}

inline void treeMapper::varianceAndMean(lvec3 * sT, lvec3 * sST, int width, int l, int r, int b, int t, float & variance, glm::vec3& mean)
{
	lvec3 sum = sT[(t - 1)*width + r-1];
	lvec3 sqrSum = sST[(t - 1)*width + r-1];
	if (b > 0)
	{
		sum -= sT[(b - 1)*width + r-1];
		sqrSum -= sST[(b - 1)*width + r-1];
		if (l > 0)
		{
			sum += sT[(b - 1)*width + (l - 1)];
			sqrSum += sST[(b - 1)*width + (l - 1)];
		}
	}
	if (l > 0)
	{
		sum -= sT[(t - 1)*width + (l - 1)];
		sqrSum -= sST[(t - 1)*width + (l - 1)];
	}

	//calc variance
	float n = float((r - l)*(t - b));
	//build kd-tree
	glm::vec3 variance3(double(sqrSum.x/n) - pow(double(sum.x / n),2.0),
		double(sqrSum.y / n) - pow(double(sum.y / n), 2.0),
		double(sqrSum.z / n) - pow(double(sum.z / n), 2.0)
	);
	variance = length(variance3);//variance from [left,i)
	mean = glm::vec3(sum.x / n, sum.y / n, sum.z / n);

}

treeMapper::~treeMapper()
{

}
