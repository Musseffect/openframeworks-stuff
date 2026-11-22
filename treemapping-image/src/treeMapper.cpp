#include "treeMapper.h"



void treeMapper::draw(float scaleW, float scaleH, int shiftX, int shiftY)
{
	std::stack<stackDrawItem> stack;
	stack.push(stackDrawItem{ root, 0, w, 0, h });
	while (!stack.empty())
	{
		stackDrawItem current = stack.top();
		stack.pop();
		if (current.node->border == -1)
		{
			fvec3 averageColor = current.node->average;
			ofColor color(averageColor.x, averageColor.y, averageColor.z);
			ofSetColor(color);
			ofDrawRectangle(current.left*scaleW + shiftX, current.bottom*scaleH + shiftY, (current.right - current.left)*scaleW, (current.top - current.bottom)*scaleH);
		}
		else
		{
			if (current.node->direction)
			{
				stack.push(stackDrawItem{ current.node->left, current.left, current.node->border, current.bottom, current.top });
				stack.push(stackDrawItem{ current.node->right, current.node->border, current.right, current.bottom, current.top });
			}
			else
			{
				stack.push(stackDrawItem{ current.node->left, current.left,current.right,current.bottom,current.node->border });
				stack.push(stackDrawItem{ current.node->right,current.left,current.right,current.node->border,current.top });
			}
		}
	}
}

void treeMapper::drawDepth(float scaleW, float scaleH, int shiftX, int shiftY)
{
	std::stack<stackDrawItem> stack;
	stack.push(stackDrawItem{ root ,0,w,0,h });
	while (!stack.empty())
	{
		stackDrawItem current = stack.top();
		stack.pop();
		if (current.node->border == -1)
		{
			float fColor = ((float)current.node->depth / maxDepth);
			ofColor color(fColor * 255.0, 255);
			ofSetColor(color);
			ofDrawRectangle(current.left*scaleW + shiftX, current.bottom*scaleH + shiftY, (current.right - current.left)*scaleW, (current.top - current.bottom)*scaleH);
		}
		else
		{
			if (current.node->direction)
			{
				stack.push(stackDrawItem{ current.node->left, current.left,current.node->border,current.bottom,current.top });
				stack.push(stackDrawItem{ current.node->right,current.node->border,current.right,current.bottom,current.top });
			}
			else
			{
				stack.push(stackDrawItem{ current.node->left, current.left,current.right,current.bottom,current.node->border });
				stack.push(stackDrawItem{ current.node->right,current.left,current.right,current.node->border,current.top });
			}
		}
	}
}

inline void treeMapper::create(int width, int height, ofImage& image, float minVariance, int tMaxDepth, int minSizeX, int minSizeY, float maxRatio, int objFunc)
{
	//minVariance *= 65025.f;
	//build summation area tables for values and values^2
	lvec3 *summedTable = new lvec3[width * height];
	lvec3 *summedSqrTable = new lvec3[width * height];
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
	maxDepth = 0;
	root = new kdNode{ fvec3(),-1,nullptr,nullptr,0,false };
	lvec3 _t = summedTable[width*height - 1];
	float _n = float(width*height);
	root->average = fvec3(_t.x / _n, _t.y / _n, _t.z / _n);
	//build kd-tree
	std::stack<stackItem> buildStack;
	lvec3 s1 = _t;
	lvec3 s2 = summedSqrTable[width*height - 1];
	fvec3 variance3(double(s2.x / _n) - pow(double(s1.x / _n), 2.0),
		double(s2.y / _n) - pow(double(s1.y / _n), 2.0),
		double(s2.z / _n) - pow(double(s1.z / _n), 2.0)
	);
	int nodes = 1;
	float variance = length(variance3);
	//recursive creation of kd-tree(i guess its kd-tree) in such way, that choosen vertical or horizontal line optimally(based on some expression from
	//resulted variances for two parts) divides current rectangle
	buildStack.push(stackItem{ root, 0, width, 0, height, variance, 0 });
	while (!buildStack.empty())
	{
		stackItem current = buildStack.top();
		buildStack.pop();
		if (current.depth > tMaxDepth)
			continue;

		float conditionVariable = FLT_MAX;
		if (objFunc == objectiveFunction::maxSumOfValuesDifference)
			conditionVariable = 0.f;
		int index = -1;
		float lv = 0;
		float rv = 0;
		fvec3 lMean;
		fvec3 rMean;
		bool direction = true;// vertical(false) - horizontal(true) line
		if (current.right - current.left > 2 * minSizeX)//min width property
		{
			for (int i = current.left + minSizeX; i + minSizeX <= current.right; i++)//x
			{
				float leftVariance = 0.f;
				float rightVariance = 0.f;

				fvec3 _lMean;
				fvec3 _rMean;
				varianceAndMean(summedTable, summedSqrTable, width, current.left, i, current.bottom, current.top, leftVariance, _lMean);
				varianceAndMean(summedTable, summedSqrTable, width, i, current.right, current.bottom, current.top, rightVariance, _rMean);
				bool condition = false;
				float expressionResult = 0.f;
				int lSquare = (i - current.left)*(current.top - current.bottom);
				int rSquare = (current.right - i)*(current.top - current.bottom);
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
					direction = true;
					lv = leftVariance;
					rv = rightVariance;
					lMean = _lMean;
					rMean = _rMean;
				}
			}
		}
		if (current.top - current.bottom > 2 * minSizeY)//min height property
		{
			for (int i = current.bottom + minSizeY; i + minSizeY < current.top; i++)//y
			{
				float leftVariance = 0.f;
				float rightVariance = 0.f;

				fvec3 _lMean;
				fvec3 _rMean;
				varianceAndMean(summedTable, summedSqrTable, width, current.left, current.right, current.bottom, i, leftVariance, _lMean);
				varianceAndMean(summedTable, summedSqrTable, width, current.left, current.right, i, current.top, rightVariance, _rMean);
				bool condition = false;
				float expressionResult = 0.f;
				int lSquare = (current.right - current.left)*(i - current.bottom);
				int rSquare = (current.right - current.left)*(current.top - i);
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
			current.node->direction = direction;
			current.node->border = index;
			maxDepth = max(maxDepth, current.depth + 1);
			current.node->left = new kdNode{ lMean,-1,nullptr,nullptr,current.depth + 1,current.node->direction };
			current.node->right = new kdNode{ rMean,-1,nullptr,nullptr,current.depth + 1,current.node->direction };
			nodes += 2;
			if (direction)
			{
				if (lv > minVariance && lv / current.variance < maxRatio)
				{
					buildStack.push(stackItem{ current.node->left,current.left,index,current.bottom,current.top,lv,current.depth + 1 });
				}
				if (rv > minVariance && rv / current.variance < maxRatio)
				{
					buildStack.push(stackItem{ current.node->right,index,current.right,current.bottom,current.top,rv,current.depth + 1 });
				}
			}
			else
			{
				if (lv > minVariance && lv / current.variance < maxRatio)
				{
					buildStack.push(stackItem{ current.node->left,current.left,current.right,current.bottom,index,lv,current.depth + 1 });
				}
				if (rv > minVariance && rv / current.variance < maxRatio)
				{
					buildStack.push(stackItem{ current.node->right,current.left,current.right,index,current.top,rv,current.depth + 1 });
				}
			}
		}
	}
	delete[]summedTable;
	delete[]summedSqrTable;
	std::cout << nodes << " created\n";
}

void treeMapper::destroy()
{
	//delete tree
	std::stack<kdNode*> stack;
	stack.push(root);
	while (!stack.empty())
	{
		kdNode* current = stack.top();
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

treeMapper::treeMapper(int width, int height, ofImage& image, float minVariance, int maxDepth, int minSizeX, int minSizeY, float maxRatio, int objFunc)
{
	w = width;
	h = height;
	create(width, height, image, minVariance, maxDepth, minSizeX, minSizeY, maxRatio, objFunc);
}

inline void treeMapper::varianceAndMean(lvec3 * sT, lvec3 * sST, int width, int l, int r, int b, int t, float & variance, fvec3& average)
{
	lvec3 sum = sT[(t - 1)*width + r - 1];
	lvec3 sqrSum = sST[(t - 1)*width + r - 1];
	if (b > 0)
	{
		sum -= sT[(b - 1)*width + r - 1];
		sqrSum -= sST[(b - 1)*width + r - 1];
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
	fvec3 variance3(double(sqrSum.x / n) - pow(double(sum.x / n), 2.0),
		double(sqrSum.y / n) - pow(double(sum.y / n), 2.0),
		double(sqrSum.z / n) - pow(double(sum.z / n), 2.0)
	);
	variance = (abs(variance3.x) + abs(variance3.y) + abs(variance3.z)) / 3.f;//variance from [left,i)
	average = fvec3(sum.x / n, sum.y / n, sum.z / n);

}

treeMapper::~treeMapper()
{
	destroy();
}

fvec3 edgeDetectionMapper::computeAverage(const std::vector<fvec3>& summedTable, int x0, int x1, int y0, int y1) {
	fvec3 sum = summedTable[(y1 - 1) * w + x1 - 1];
	if (y0 > 0) {
		sum -= summedTable[y0 * w + x1 - 1];
		if (x0 > 0) {
			sum += summedTable[y0 * w + x0];
		}
	}
	if (x0 > 0) {
		sum -= summedTable[(y1 - 1) * w + x0];
	}
	return sum / float((x1 - x0) * (y1 - y0));
}

edgeDetectionMapper::edgeDetectionMapper(int width, int height, const ofImage& image, float splitThreshold, int maxDepth, int minSizeX, int minSizeY) : w(width), h(height) {
	std::vector<float> edgeImage;
	edgeImage.resize(width * height);
	const float kernelX[3][3] = { {1, 0, -1}, {2, 0, -2}, {1, 0, -1} };
	const float kernelY[3][3] = { {1, 2, 1}, {0, 0, 0}, {-1, -2, -1} };
	float maxValue = 0.0;
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			fvec3 valueX = fvec3(0.0f);
			fvec3 valueY = fvec3(0.0f);
			for (int l = -1; l < 2; l++) {
				for (int k = -1; k < 2; k++) {
					int x = glm::clamp(i + k, 0, width - 1);
					int y = glm::clamp(j + l, 0, height - 1);
					ofColor col = image.getColor(x, y);
					valueX += fvec3(col.r / 255.0f, col.g / 255.0f, col.b / 255.0f) * kernelX[k + 1][l + 1];
					valueY += fvec3(col.r / 255.0f, col.g / 255.0f, col.b / 255.0f) * kernelY[k + 1][l + 1];
				}
			}
			valueX = glm::abs(valueX);
			valueY = glm::abs(valueY);
			valueX /= 4.0;
			valueY /= 4.0;
			fvec3 value = glm::sqrt(valueX * valueX + valueY * valueY) / std::sqrt(2.0f);
			edgeImage[i + j * width] = std::max(std::max(value.x, value.y), value.z);
		}
	}
	std::cout << "maxValue: " << maxValue;
	createMaxEdgeDetection(image, edgeImage, splitThreshold, maxDepth, minSizeX, minSizeY);
};

void edgeDetectionMapper::createMaxEdgeDetection(const ofImage& image, const std::vector<float>& edgeImage, float splitThreshold, int maxDepth, int minWidth, int minHeight) {
	struct item { kdNode* node; int x0; int x1; int y0; int y1; int depth; };
	// summedTable for fast average
	std::vector<fvec3> summedTable(w * h);
	for (int j = 0; j < h; j++)
	{
		int index = j * w;
		for (int i = 0; i < w; i++)
		{
			ofColor color = image.getColor(i, j);
			fvec3 value(color.r / float(255), color.g / float(255), color.b / float(255));
			if (i > 0 && j > 0) {
				value -= summedTable[i - 1 + index - w];
			}
			if (i > 0) {
				value += summedTable[i - 1 + index];
			}
			if (j > 0) {
				value += summedTable[i + index - w];
			}
			summedTable[i + index] = value;
		}
	}
	std::stack<item> stack;
	root = new kdNode{ fvec3(), -1, nullptr, nullptr, 1, false };
	depth = 1;
	stack.push(item{ root, 0, w, 0, h, 0 });
	while (!stack.empty()) {
		item current = stack.top();
		stack.pop();
		current.node->average = computeAverage(summedTable, current.x0, current.x1, current.y0, current.y1);
		depth = std::max(current.depth, depth);
		if (current.depth + 1 > maxDepth)
			continue;
		float maxValue = 0.0;
		float minValue = 0.0;
		for (int y = current.y0; y < current.y1; ++y) {
			for (int x = current.x0; x < current.x1; ++x) {
				float value = edgeImage[x + y * w];
				maxValue = std::max(maxValue, value);
				minValue = std::min(minValue, value);
			}
		}
		if (maxValue < splitThreshold)
			continue;
		/*if (minValue > splitThreshold)
			continue;*/
		bool splitDirection = false;
		int splitCoord;
		int width = current.x1 - current.x0;
		int height = current.y1 - current.y0;
		kdNode* left = new kdNode{ fvec3(), -1, nullptr, nullptr, current.depth + 1, false };
		kdNode* right = new kdNode{ fvec3(), -1, nullptr, nullptr, current.depth + 1, false };
		int splitWidth = width / 2;
		int splitHeight = height / 2;
		if ((width > height || splitHeight < minHeight) && splitWidth >= minWidth) {
			splitDirection = true;
			splitCoord = (current.x0 + current.x1) / 2;
			stack.push(item{ left, current.x0, splitCoord, current.y0, current.y1, current.depth + 1 });
			stack.push(item{ right, splitCoord, current.x1, current.y0, current.y1, current.depth + 1 });
		}
		else if ((height >= width || splitWidth < minWidth) && splitHeight >= minHeight) {
			splitCoord = (current.y0 + current.y1) / 2;
			stack.push(item{ left, current.x0, current.x1, current.y0, splitCoord, current.depth + 1 });
			stack.push(item{ right, current.x0, current.x1, splitCoord, current.y1, current.depth + 1 });
		}
		else {
			delete left;
			delete right;
			continue;
		}
		current.node->left = left;
		current.node->right = right;
		current.node->border = splitCoord;
		current.node->direction = splitDirection;
	}
}

void edgeDetectionMapper::destroy()
{
	//delete tree
	std::stack<kdNode*> stack;
	stack.push(root);
	while (!stack.empty())
	{
		kdNode* current = stack.top();
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
void edgeDetectionMapper::draw()
{
	struct item { kdNode* node; int left; int right; int bottom; int top; int depth; };

	std::stack<item> stack;
	stack.push(item{ root, 0, w, 0, h });
	while (!stack.empty())
	{
		item current = stack.top();
		stack.pop();
		if (current.node->border == -1)
		{
			fvec3 averageColor = current.node->average * 255;
			ofColor color(averageColor.x, averageColor.y, averageColor.z);
			ofSetColor(color);
			ofDrawRectangle(current.left, current.bottom, (current.right - current.left), (current.top - current.bottom));
		}
		else
		{
			if (current.node->direction)
			{
				stack.push(item{ current.node->left, current.left, current.node->border, current.bottom, current.top });
				stack.push(item{ current.node->right, current.node->border, current.right, current.bottom, current.top });
			}
			else
			{
				stack.push(item{ current.node->left, current.left,current.right,current.bottom,current.node->border });
				stack.push(item{ current.node->right,current.left,current.right,current.node->border,current.top });
			}
		}
	}
}

void edgeDetectionMapper::drawDepth(float scaleW, float scaleH, int shiftX, int shiftY)
{
	struct item { kdNode* node; int left; int right; int bottom; int top; int depth; };

	std::stack<item> stack;
	stack.push(item{ root ,0,w,0,h });
	while (!stack.empty())
	{
		item current = stack.top();
		stack.pop();
		if (current.node->border == -1)
		{
			float fColor = ((float)current.node->depth / depth);
			ofColor color(fColor * 255.0, 255);
			ofSetColor(color);
			ofDrawRectangle(current.left*scaleW + shiftX, current.bottom*scaleH + shiftY, (current.right - current.left)*scaleW, (current.top - current.bottom)*scaleH);
		}
		else
		{
			if (current.node->direction)
			{
				stack.push(item{ current.node->left, current.left,current.node->border,current.bottom,current.top });
				stack.push(item{ current.node->right,current.node->border,current.right,current.bottom,current.top });
			}
			else
			{
				stack.push(item{ current.node->left, current.left,current.right,current.bottom,current.node->border });
				stack.push(item{ current.node->right,current.left,current.right,current.node->border,current.top });
			}
		}
	}
}
edgeDetectionMapper::~edgeDetectionMapper() {
	destroy();
}
