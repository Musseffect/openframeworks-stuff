#include "ofApp.h"

/*
let img;

function preload() {
  img = loadImage('Warframe0000.jpg');
  img.resize(10, 0);
}

function setup() {
  createCanvas(1200, 700);
  img.loadPixels();
  let initialPoints = [{
	x: 155,
	y: 155
  }, {
	x: 255,
	y: 313
  }, {
	x: 455,
	y: 113
  }, {
	x: 165,
	y: 273
  }];
  for (let i = 0; i < 140; i++) {
	initialPoints.push({
	  x: (Math.random() * img.width),
	  y: (Math.random() * img.height)
	});
  }
  pixelSorting(img, initialPoints);
  img.updatePixels();
}

function testDrawLine(x0, y0, x1, y1) {
  let pixels = [];
  let black = color(0);
  let dx = x1 - x0;
  let dy = y1 - y0;
  let ix0 = round(x0);
  let ix1 = round(x1);
  let iy0 = round(y0);
  let iy1 = round(y1);
  strokeWeight(10);
  if (abs(dx) > abs(dy)) {
	let derr = abs(dy / dx);
	let y = iy0;
	let xErr = abs(x0 - ix0);
	let err = abs(y0 - iy0) * Math.sign(dy / dx);
	for (let x = ix0; true; x += Math.sign(dx)) {
	  pixels.push({
		x,
		y
	  });
	  //point(x * 10, y * 10);
	  set(x, y, black);
	  err += derr;
	  if (err >= 0.5) {
		y += Math.sign(dy);
		err -= 1.0;
	  }
	  if (x == ix1)
		break;
	}
  } else {
	let derr = abs(dx / dy);
	let x = ix0;
	let yErr = abs(y0 - iy0);
	let err = abs(x0 - ix0) * Math.sign(dx / dy);
	for (let y = iy0; true; y += Math.sign(dy)) {
	  pixels.push({
		x,
		y
	  });
	  //point(x * 10, y * 10);
	  set(x, y, black);
	  err += derr;
	  if (err >= 0.5) {
		x += Math.sign(dx);
		err -= 1;
	  }
	  if (y == iy1)
		break;
	}
  }
  updatePixels();
  return pixels;
}


function drawLine(x0, y0, x1, y1) {
  let pixels = [];
  let black = color(0);
  let dx = x1 - x0;
  let dy = y1 - y0;
  if (abs(dx) > abs(dy)) {
	let dx = x1 - x0;
	let dy = y1 - y0;
	let derr = abs(dy / dx);
	let ix0 = round(x0);
	let ix1 = round(x1);
	let iy0 = round(y0);
	let y = iy0;
	let err = abs(y0 - iy0);
	for (let x = ix0; true; x += Math.sign(dx)) {
	  pixels.push({
		x,
		y
	  });
	  //set(x, y, black);
	  err += derr;
	  if (err >= 0.5) {
		y += Math.sign(dy);
		err -= 1.0;
	  }
	  if (x == ix1)
		break;
	}
  } else {
	let dx = x1 - x0;
	let dy = y1 - y0;
	let derr = abs(dx / dy);
	let iy0 = round(y0);
	let iy1 = round(y1);
	let ix0 = round(x0);
	let x = ix0;
	let err = abs(x0 - ix0);
	for (let y = iy0; true; y += Math.sign(dy)) {
	  pixels.push({
		x,
		y
	  });
	  //set(x, y, black);
	  err += derr;
	  if (err >= 0.5) {
		x += Math.sign(dx);
		err -= 1;
	  }
	  if (y == iy1)
		break;
	}
  }
  //updatePixels();
  return pixels;
}

function inBounds(point, image) {
  return point.x > -0.5 && point.y > -0.5 &&
	point.x < image.width - 0.5 && point.y < image.height - 0.5;
}

function field(point) {
  return {
	x: sin(point.y / height * 5 + 4. * cos(point.x * 5.1 / width)) * width,
	y: cos(point.x / width * 5 - point.y * 3 / height) * height
  };
}

function addVec(v1, v2) {
  return {
	x: v1.x + v2.x,
	y: v1.y + v2.y
  };
}

function scaleVec(v, s) {
  return {
	x: v.x * s,
	y: v.y * s
  };
}

function integrate(point, step) {
  let k1 = field(point);
  let k2 = field(addVec(point, scaleVec(k1, step / 2)));
  let k3 = field(addVec(point, scaleVec(k2, step / 2)));
  let k4 = field(addVec(point, scaleVec(k3, step)));
  return addVec(point, scaleVec(addVec(addVec(k1, k4), scaleVec(addVec(k2, k3), 2)), step / 6));
}

function sortColors(colors, start, end) {
  if (start == end)
	return colors;
  let items = colors.splice(start, end - start + 1);
  items.sort(function(a, b) {
	let la = lightness(a);
	let lb = lightness(b);
	if (la < lb)
	  return -1;
	if (lb > la)
	  return 1;
	return 0;
  });
  let beginning = colors.splice(0, start + 1);
  return beginning.concat(items).concat(colors);
}

function sortPixels(pixels) {
  let colors = pixels.map(function(item) {
	return img.get(item.x, item.y);
  });
  const lowThreshold = 40.5;
  const highThreshold = 10.75;
  for (let i = 0; i < colors.length; i++) {
	let color = colors[i];
	let l0 = brightness(color);
	if (l0 > lowThreshold) {
	  let j = i + 1;
	  while (j < colors.length) {
		let color = colors[j];
		let l = brightness(color);
		if (l < highThreshold) {
		  j--;
		  break;
		}
		j++;
	  }
	  //if(i!=j)
	  // print("i, j" + i + "," + j);
	  j = min(colors.length - 1, j);
	  colors = sortColors(colors, i, j);
	  i = j + 1;
	}
  }
  for (let i = 0; i < pixels.length; i++) {
	let pixel = pixels[i];
	let tcolor = colors[i];
	//print(`pixel ${pixel.x}, ${pixel.y}`);
	img.set(pixel.x, pixel.y, tcolor);
	//img.set(pixel.x, pixel.y, color(255, 255, 0));
  }
}

function removeRepetitions(pixels, image) {
  let firstItem = pixels[0];
  for (let i = 1; i < pixels.length; i++) {
	if (abs(pixels[i].x - pixels[i - 1].x) > 1 || abs(pixels[i].y - pixels[i - 1].y) > 1) {
	  //print(`${pixels[i].x}, ${pixels[i-1].x}: ${pixels[i].y}, ${pixels[i-1].y}`);
	}
	if (pixels[i].x < 0 || pixels[i].y < 0 ||
	  pixels[i].x > image.width - 1 || pixels[i].y > image.height - 1 ||
	  (firstItem.x == pixels[i].x && firstItem.y == pixels[i].y)) {
	  pixels.splice(i, 1);
	  i--;
	  continue;
	}
	firstItem = pixels[i];
  }
  return pixels;
}

function pixelSorting(image, initialPoints) {
  let dt = 0.03;
  initialPoints.forEach(function(point) {
	let currentPoint = point;
	let pixels = [];
	let i = 0;
	let j = 0;
	while (inBounds(currentPoint, image) && i < 2225) {
	  let newPoint = integrate(currentPoint, dt);
	  i++;
	  if (Math.round(newPoint.x) == Math.round(currentPoint.x) &&
		Math.round(newPoint.y) == Math.round(currentPoint.y)) {
		j++;
		currentPoint = newPoint;
		continue;
	  }
	  //print(`current (${currentPoint.x}, ${currentPoint.y})`);
	  //print(`new (${newPoint.x}, ${newPoint.y})`);
	  let nextPixels = drawLine(currentPoint.x, currentPoint.y,
		newPoint.x, newPoint.y);
	  if (nextPixels.length > 0)
		pixels = pixels.concat(nextPixels);
	  currentPoint = newPoint;
	  if (Math.round(currentPoint.x) == Math.round(point.x) &&
		Math.round(currentPoint.y) == Math.round(point.y)) {
		print("Back to initial");
		break;
	  }
	}
	//print(j);
	//printPixels(pixels);
	pixels = removeRepetitions(pixels, image);
	//print("after");
	//printPixels(pixels);
	sortPixels(pixels);
  });
}

function printPixels(pxls) {
  pxls.forEach(function(item) {
	print(`x: ${item.x}, y: ${item.y}`);
  });
}

function draw() {
  background(255);
  image(img, 0, 0);
	//printPixels(pxl);
	//testDrawLine(0.11070309586495, 0.7293199127548, 3.3772868275204, 7.3434095138263);
}
*/

//--------------------------------------------------------------
void ofApp::setup(){

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
