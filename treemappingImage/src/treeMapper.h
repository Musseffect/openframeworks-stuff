#pragma once
#include <ofImage.h>
#include <glm/common.hpp>

typedef glm::tvec3<long> lvec3;


class treeMapper
{
	struct stackItem { kdNode* current; int left; int right; int top; int bottom; bool swap; float variance; };
public:
	treeMapper();
	static void varianceAndMean(lvec3 *sT, lvec3 *sST,int width, int l, int r, int b, int t, float& variance, lvec3 mean)
	{
		lvec3 leftSum = sT[(t - 1)*width + r];
		lvec3 leftSqrSum = sST[(t - 1)*width + r];
		if (b > 0)
		{
			leftSum -= sT[(current.bottom - 1)*width + i - 1];
			leftSqrSum -= sST[(current.bottom - 1)*width + i - 1];
			if (l > 0)
			{
				leftSum += summedTable[(current.bottom - 1)*width + (current.left - 1)];
				leftSqrSum += summedSqrTable[(current.bottom - 1)*width + (current.left - 1)];
			}
		}
		if (l > 0)
		{
			leftSum -= summedTable[(current.top - 1)*width + (current.left - 1)];
			leftSqrSum -= summedTable[(current.top - 1)*width + (current.left - 1)];
		}

		//calc variance
		float ln = float((i - current.left)*(current.top - current.bottom));
		//build kd-tree
		glm::vec3 leftVariance3((leftSqrSum.x - leftSum.x / ln * leftSum.x) / ln,
			(leftSqrSum.y - leftSum.y / ln * leftSum.y) / ln,
			(leftSqrSum.z - leftSum.z / ln * leftSum.z) / ln
		);

		float leftVariance = length(leftVariance3);//variance from [left,i)

	
	}
	void create(int width,int height,ofImage image)
	{
		//build summation tables
		lvec3 *summedTable = new lvec3[width*height];
		lvec3 *summedSqrTable = new lvec3[width*height];
		for (int j = 0; j < height; j++)
		{
			int index = j * width;
			for (int i = 0; i < width; i++)
			{
				ofColor color= image.getColor(i,j);
				lvec3 value(color.r,color.g,color.b);
				lvec3  valuesqr= value*value;
				if(i > 0) value += summedTable[i - 1 + index];
				if(j > 0) value += summedTable[i + index-width];
				summedTable[i+index] = value;
				summedSqrTable[i + index] = value;
			}
		}
		kdNode *root = new kdNode();
		lvec3 _t = summedTable[width*height - 1];
		float _n = float(width*height);
		root->mean = glm::vec3(_t.x/_n, _t.y / _n, _t.z / _n);
		//build kd-tree
		std::stack<stackItem> buildStack;
		lvec3 s1 = _t;
		lvec3 s2 = summedSqrTable[width*height-1];
		glm::vec3 variance3((s2.x-s1.x/_n*s1.x) / _n,
			(s2.y - s1.y / _n * s1.y) / _n,
			(s2.z - s1.z / _n * s1.z) / _n
		);
		float variance = length(variance3);
		buildStack.push(stackItem{root,0,width,height,0,false,variance});
		std::vector<kdNode*> leafNodes;
		while (!buildStack.empty())
		{
			stackItem current = buildStack.top();
			buildStack.pop();
			float minSum = FLT_MAX;
			int index = -1;
			float lv=0;
			float rv=0;
			glm::vec3 lMean;
			glm::vec3 rMean;
			if (current.swap)
			{

				for (int i = current.left + 1; i < current.right; i++)//x
				{
					//left sum
					lvec3 leftSum = summedTable[(current.top - 1)*width + i - 1];
					lvec3 leftSqrSum = summedSqrTable[(current.top - 1)*width + i - 1];
					if (current.bottom > 0)
					{
						leftSum -= summedTable[(current.bottom - 1)*width + i - 1];
						leftSqrSum -= summedSqrTable[(current.bottom - 1)*width + i - 1];
						if (current.left > 0)
						{
							leftSum += summedTable[(current.bottom - 1)*width + (current.left - 1)];
							leftSqrSum += summedSqrTable[(current.bottom - 1)*width + (current.left - 1)];
						}
					}
					if (current.left > 0)
					{
						leftSum -= summedTable[(current.top - 1)*width + (current.left - 1)];
						leftSqrSum -= summedTable[(current.top - 1)*width + (current.left - 1)];
					}

					//right sum
					lvec3 rightSum = summedTable[(current.top-1)*width+(current.right-1)];
					lvec3 rightSqrSum = summedTable[(current.top - 1)*width + (current.right - 1)];
					if (current.bottom > 0)
					{
						rightSum -= summedTable[(current.bottom - 1)*width + (current.right - 1)];
						rightSum += summedTable[(current.bottom - 1)*width + i - 1];
						rightSqrSum -= summedSqrTable[(current.bottom - 1)*width + (current.right - 1)];
						rightSqrSum += summedSqrTable[(current.bottom - 1)*width + i - 1];
					}
					rightSum -= summedTable[(current.top - 1)*width + i-1];
					rightSqrSum -= summedTable[(current.top - 1)*width + i - 1];

					//calc variance
					float ln = float((i-current.left)*(current.top-current.bottom));
					float rn = float((current.right-i)*(current.top - current.bottom));
					//build kd-tree
					glm::vec3 leftVariance3((leftSqrSum.x - leftSum.x / ln * leftSum.x) / ln,
						(leftSqrSum.y - leftSum.y / ln * leftSum.y) / ln,
						(leftSqrSum.z - leftSum.z / ln * leftSum.z) / ln
					);

					float leftVariance=length(leftVariance3);//variance from [left,i)

					glm::vec3 rightVariance3((rightSqrSum.x - rightSum.x / rn * rightSum.x) / rn,
						(rightSqrSum.y - rightSum.y / rn * rightSum.y) / rn,
						(rightSqrSum.z - rightSum.z / rn * rightSum.z) / rn
					);
					float rightVariance=length(rightVariance3);//variance from [i,right)

					float sum = leftVariance + rightVariance;
					if (sum < minSum&& leftVariance<current.variance&&rightVariance<current.variance)
					{
						index = i;
						minSum = sum;
						lv = leftVariance;
						rv = rightVariance;
						lMean = glm::vec3(leftSum.x/ln, leftSum.y / ln, leftSum.z / ln);
						rMean = glm::vec3(rightSum.x / ln, rightSum.y / ln, rightSum.z / ln);
					}
					//if sum of variances is less than min set index
				}
				if (index != -1)
				{
					current.current->border = index;
					current.current->left = new kdNode{ -1,lMean,nullptr,nullptr };
					current.current->right = new kdNode{ -1,rMean,nullptr,nullptr };
					if(!(lv == 0.f||index-current.left<2))
					{
						buildStack.push(stackItem{ current.current->left,current.left,index,current.top,current.bottom,!current.swap,lv });
					}
					if (!(rv == 0.f||current.right-index<2))
					{
						buildStack.push(stackItem{ current.current->right,index,current.right,current.top,current.bottom,!current.swap,rv });
					}
					//if variance of a part equals zero dont push it in stack
					//if right-left<2 dont push
				
				}
			}else
			{
				for (int i = current.bottom; i < current.top; i++)//y
				{
					//left sum
					lvec3 leftSum = summedTable[(current.top - 1)*width + i - 1];
					lvec3 leftSqrSum = summedSqrTable[(current.top - 1)*width + i - 1];
					if (current.bottom > 0)
					{
						leftSum -= summedTable[(current.bottom - 1)*width + i - 1];
						leftSqrSum -= summedSqrTable[(current.bottom - 1)*width + i - 1];
						if (current.left > 0)
						{
							leftSum += summedTable[(current.bottom - 1)*width + (current.left - 1)];
							leftSqrSum += summedSqrTable[(current.bottom - 1)*width + (current.left - 1)];
						}
					}
					if (current.left > 0)
					{
						leftSum -= summedTable[(current.top - 1)*width + (current.left - 1)];
						leftSqrSum -= summedTable[(current.top - 1)*width + (current.left - 1)];
					}

					//right sum
					lvec3 rightSum = summedTable[(current.top - 1)*width + (current.right - 1)];
					lvec3 rightSqrSum = summedTable[(current.top - 1)*width + (current.right - 1)];
					if (current.bottom > 0)
					{
						rightSum -= summedTable[(current.bottom - 1)*width + (current.right - 1)];
						rightSum += summedTable[(current.bottom - 1)*width + i - 1];
						rightSqrSum -= summedSqrTable[(current.bottom - 1)*width + (current.right - 1)];
						rightSqrSum += summedSqrTable[(current.bottom - 1)*width + i - 1];
					}
					rightSum -= summedTable[(current.top - 1)*width + i - 1];
					rightSqrSum -= summedTable[(current.top - 1)*width + i - 1];

					//calc variance
					float ln = float((i - current.left)*(current.top - current.bottom));
					float rn = float((current.right - i)*(current.top - current.bottom));
					//build kd-tree
					glm::vec3 leftVariance3((leftSqrSum.x - leftSum.x / ln * leftSum.x) / ln,
						(leftSqrSum.y - leftSum.y / ln * leftSum.y) / ln,
						(leftSqrSum.z - leftSum.z / ln * leftSum.z) / ln
					);

					float leftVariance = length(leftVariance3);//variance from [left,i)

					glm::vec3 rightVariance3((rightSqrSum.x - rightSum.x / rn * rightSum.x) / rn,
						(rightSqrSum.y - rightSum.y / rn * rightSum.y) / rn,
						(rightSqrSum.z - rightSum.z / rn * rightSum.z) / rn
					);
					float rightVariance = length(rightVariance3);//variance from [i,right)

					float sum = leftVariance + rightVariance;
					if (sum < minSum&& leftVariance < current.variance&&rightVariance < current.variance)
					{
						index = i;
						minSum = sum;
						lv = leftVariance;
						rv = rightVariance;
						lMean = glm::vec3(leftSum.x / ln, leftSum.y / ln, leftSum.z / ln);
						rMean = glm::vec3(rightSum.x / ln, rightSum.y / ln, rightSum.z / ln);
					}
					//if sum of variances is less than min set index

				}
			}
			//for each node
			//find splitting that creates minimum difference between variances
			//split
			//continue
		
		
		}

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
		delete[]summedTable;
		delete[]summedSqrTable;
	}
	~treeMapper();
};




struct kdNode
{
	int border;
	glm::vec3 mean;
	kdNode *left;
	kdNode *right;
};