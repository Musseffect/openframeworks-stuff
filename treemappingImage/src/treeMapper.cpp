#include "treeMapper.h"



void treeMapper::draw(float scaleW, float scaleH,int shiftX,int shiftY)
{
	std::stack<stackDrawItem*> stack;
	stack.push(new stackDrawItem{ root ,0,w,0,h });
	while (!stack.empty())
	{
		stackDrawItem* current = stack.top();
		stack.pop();
		if (current->node->border == -1)
		{
			glm::vec3 averageColor = current->node->average;
			ofColor color(averageColor.x, averageColor.y, averageColor.z);
			ofSetColor(color);
			ofDrawRectangle(current->left*scaleW+shiftX, current->bottom*scaleH+shiftY, (current->right - current->left)*scaleW, (current->top - current->bottom)*scaleH);
		}
		else
		{
			if (current->node->direction)
			{
				stack.push(new stackDrawItem{ current->node->left, current->left,current->node->border,current->bottom,current->top });
				stack.push(new stackDrawItem{ current->node->right,current->node->border,current->right,current->bottom,current->top });
			}
			else
			{
				stack.push(new stackDrawItem{ current->node->left, current->left,current->right,current->bottom,current->node->border });
				stack.push(new stackDrawItem{ current->node->right,current->left,current->right,current->node->border,current->top });
			}
		}
		delete current;
	}
}


inline void treeMapper::create(int width, int height, ofImage& image, float minVariance, int maxDepth, int minSizeX, int minSizeY,float maxRatio,int objFunc)
{
	//minVariance *= 65025.f;
	//build summation area tables for values and values^2
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
	root = new kdNode{-1,nullptr,nullptr,glm::vec3(),false};
	lvec3 _t = summedTable[width*height - 1];
	float _n = float(width*height);
	root->average = glm::vec3(_t.x / _n, _t.y / _n, _t.z / _n);
	//build kd-tree
	std::stack<stackItem*> buildStack;
	lvec3 s1 = _t;
	lvec3 s2 = summedSqrTable[width*height - 1];
	glm::vec3 variance3(double(s2.x / _n) - pow(double(s1.x / _n),2.0) ,
		double(s2.y / _n) - pow(double(s1.y / _n), 2.0),
		double(s2.z / _n) - pow(double(s1.z / _n), 2.0)
	);
	int nodes = 1;
	float variance = length(variance3);
	//recursive creation of kd-tree(i guess its kd-tree) in such way, that choosen vertical or horizontal line optimally(based on some expression from
	//resulted variances for two parts) divides current rectangle
	buildStack.push(new stackItem{ root,0,width,0,height,variance,0 });
	while (!buildStack.empty())
	{
		stackItem* current = buildStack.top();
		buildStack.pop();
		if (current->depth > maxDepth)
		{
			delete current;
			continue;
		}
		float conditionVariable = FLT_MAX;
		if(objFunc==objectiveFunction::maxSumOfValuesDifference)
			conditionVariable = 0.f;
		int index = -1;
		float lv = 0;
		float rv = 0;
		glm::vec3 lMean;
		glm::vec3 rMean;
		bool direction = true;// vertical(false) - horizontal(true) line
		if (current->right - current->left > 2*minSizeX)//min width property
		{
			for (int i = current->left + minSizeX; i+minSizeX <= current->right; i++)//x
			{
				float leftVariance = 0.f;
				float rightVariance = 0.f;

				glm::vec3 _lMean;
				glm::vec3 _rMean;
				varianceAndMean(summedTable, summedSqrTable, width, current->left, i, current->bottom, current->top, leftVariance, _lMean);
				varianceAndMean(summedTable, summedSqrTable, width, i, current->right, current->bottom, current->top, rightVariance, _rMean);
				bool condition = false;
				float expressionResult=0.f;
				int lSquare=(i-current->left)*(current->top-current->bottom);
				int rSquare= (current->right-i)*(current->top - current->bottom);
				switch (objFunc)
				{
					case minMax:
						condition = (conditionVariable> (expressionResult = max(leftVariance,rightVariance)));
						break;
					case minSum:
						condition = (conditionVariable > (expressionResult = (leftVariance + rightVariance)));
						break;
					case minProportionalSum:
						condition = (conditionVariable > (expressionResult = (leftVariance*lSquare + rightVariance*rSquare)));
						break;
					case minProportionalSum2:
						condition = (conditionVariable >
							(expressionResult = (leftVariance / lSquare + rightVariance / rSquare)));
						break;
					case minMult:
						condition = (conditionVariable > (expressionResult = (leftVariance * rightVariance)));
						break;
					case minProportinalMult:
						condition = (conditionVariable > 
							(expressionResult = (leftVariance / lSquare * rightVariance / rSquare)));
						break;
					case maxSumOfValuesDifference:
						condition = (conditionVariable< (expressionResult = abs(glm::distance(_lMean*lSquare,_rMean*rSquare))));
						break;
					case minSumOfValuesDifference:
						condition = (conditionVariable > (expressionResult = abs(glm::distance(_lMean*lSquare, _rMean*rSquare))));
						break;
					default:
						break;
				}
				if (condition)
				{
					index = i;
					conditionVariable = expressionResult;
					direction = true;
					lv = leftVariance;
					rv = rightVariance;
					lMean = _lMean;
					rMean = _rMean;
				}
			}
		}
		if (current->top - current->bottom > 2*minSizeY)//min height property
		{
			for (int i = current->bottom + minSizeY; i+minSizeY < current->top; i++)//y
			{
				float leftVariance = 0.f;
				float rightVariance = 0.f;

				glm::vec3 _lMean;
				glm::vec3 _rMean;
				varianceAndMean(summedTable, summedSqrTable, width, current->left, current->right, current->bottom, i, leftVariance, _lMean);
				varianceAndMean(summedTable, summedSqrTable, width, current->left, current->right, i, current->top, rightVariance, _rMean); 
				bool condition = false;
				float expressionResult = 0.f;
				int lSquare = (current->right - current->left)*(i - current->bottom);
				int rSquare = (current->right - current->left)*(current->top - i);
				switch (objFunc)
				{
				case minMax:
					condition = (conditionVariable > (expressionResult = max(leftVariance, rightVariance)));
					break;
				case minSum:
					condition = (conditionVariable > (expressionResult = (leftVariance + rightVariance)));
					break;
				case minProportionalSum:
					condition = (conditionVariable > (expressionResult = (leftVariance*lSquare + rightVariance * rSquare)));
					break;
				case minProportionalSum2:
					condition = (conditionVariable >
						(expressionResult = (leftVariance / lSquare + rightVariance / rSquare)));
					break;
				case minMult:
					condition = (conditionVariable > (expressionResult = (leftVariance * rightVariance)));
					break;
				case minProportinalMult:
					condition = (conditionVariable >
						(expressionResult = (leftVariance / lSquare * rightVariance / rSquare)));
					break;
				case maxSumOfValuesDifference:
					condition = (conditionVariable < (expressionResult = abs(glm::distance(_lMean*lSquare, _rMean*rSquare))));
					break;
				case minSumOfValuesDifference:
					condition = (conditionVariable > (expressionResult = abs(glm::distance(_lMean*lSquare, _rMean*rSquare))));
					break;
				default:
					break;
				}
				if (condition)
				{
					index = i;
					conditionVariable = expressionResult;
					direction = false;
					lv = leftVariance;
					rv = rightVariance;
					lMean = _lMean;
					rMean = _rMean;
				}
				//if sum of variances is less than min set index
			}
		}
		if (index != -1)//if optimal division was found
		{
			current->node->direction = direction;
			current->node->border = index;
			current->node->left = new kdNode{ -1,nullptr,nullptr,lMean,current->node->direction };
			current->node->right = new kdNode{ -1,nullptr,nullptr,rMean,current->node->direction };
			nodes+=2;
			if (direction)
			{
				if (lv > minVariance&& lv/current->variance<maxRatio)
				{
					buildStack.push(new stackItem{ current->node->left,current->left,index,current->bottom,current->top,lv,current->depth + 1 });
				}
				if (rv > minVariance&&rv / current->variance < maxRatio)
				{
					buildStack.push(new stackItem{ current->node->right,index,current->right,current->bottom,current->top,rv,current->depth + 1 });
				}
			}
			else
			{
				if (lv > minVariance&&lv / current->variance < maxRatio)
				{
					buildStack.push(new stackItem{ current->node->left,current->left,current->right,current->bottom,index,lv,current->depth + 1 });
				}
				if (rv > minVariance&&rv / current->variance < maxRatio)
				{
					buildStack.push(new stackItem{ current->node->right,current->left,current->right,index,current->top,rv,current->depth + 1 });
				}
			}
			//if variance of a part equals zero dont push it in stack
			//if right-left<2 dont push
		}
		delete current;
		//for each node
		//find splitting that creates minimum difference between variances
		//split
		//continue


	}
	delete[]summedTable;
	delete[]summedSqrTable;
	std::cout << nodes<<" created\n";
}

void treeMapper::destroy()
{
	//delete tree
	std::stack<kdNode*> stack;
	stack.push(root);
	while (!stack.empty())
	{
		kdNode * current = stack.top();
		stack.pop();
		if (current->left != nullptr)
			stack.push(current->left);
		if (current->left != nullptr)
			stack.push(current->right);
		delete current;
	}
#ifndef NDEBUG
	std::cout << "tree destroyed\n";
#endif
}

treeMapper::treeMapper(int width, int height, ofImage& image, float minVariance,  int maxDepth, int minSizeX, int minSizeY,float maxRatio,int objFunc)
{
	w = width;
	h = height;
	create(width, height, image,minVariance, maxDepth, minSizeX, minSizeY,maxRatio,objFunc);
}

inline void treeMapper::varianceAndMean(lvec3 * sT, lvec3 * sST, int width, int l, int r, int b, int t, float & variance, glm::vec3& average)
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
	glm::vec3 variance3(double(sqrSum.x/n) - pow(double(sum.x / n),2.0),
		double(sqrSum.y / n) - pow(double(sum.y / n), 2.0),
		double(sqrSum.z / n) - pow(double(sum.z / n), 2.0)
	);
	variance = (abs(variance3.x)+ abs(variance3.y)+ abs(variance3.z))/3.f;//variance from [left,i)
	average = glm::vec3(sum.x / n, sum.y / n, sum.z / n);

}

treeMapper::~treeMapper()
{
	destroy();
}
