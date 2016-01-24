//
//  ShareOnFacebook.cpp
//  interapptive
//
//  Created by minggo on 12-9-24.
//
//

#include "Share.h"
#include "cocos2d.h"

using namespace cocos2d;

// @credits http://www.cocos2d-iphone.org/forum/topic/1722


static UIImage* screenshotUIImageFromHeight(int height)
{
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    
	CGSize displaySize;
	CGSize winSize;
    
    displaySize.width = size.width;
    displaySize.height = size.height;
    
    winSize.width = size.width;
    winSize.height = size.height;
    
	
	//Create buffer for pixels
	GLuint bufferLength = displaySize.width * displaySize.height * 4;
	GLubyte* buffer = (GLubyte*)malloc(bufferLength);
	
	//Read Pixels from OpenGL
	glReadPixels(0, height, displaySize.width, displaySize.height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	//Make data provider with data.
	CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, buffer, bufferLength, NULL);
	
    displaySize.height -= height;
    winSize.height -= height;
    
	//Configure image
	int bitsPerComponent = 8;
	int bitsPerPixel = 32;
	int bytesPerRow = 4 * displaySize.width;
	CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
	CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
	CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
	CGImageRef iref = CGImageCreate(displaySize.width, displaySize.height, bitsPerComponent, bitsPerPixel, bytesPerRow, colorSpaceRef, bitmapInfo, provider, NULL, NO, renderingIntent);
	
	uint32_t* pixels = (uint32_t*)malloc(bufferLength);
	CGContextRef context = CGBitmapContextCreate(pixels, winSize.width, winSize.height, 8, winSize.width * 4, CGImageGetColorSpace(iref), kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
	
	CGContextTranslateCTM(context, 0, displaySize.height);
	CGContextScaleCTM(context, 1.0f, -1.0f);
	
	switch (CCDirector::sharedDirector()->getDeviceOrientation())
	{
		case CCDeviceOrientationPortrait: break;
		case CCDeviceOrientationPortraitUpsideDown:
			CGContextRotateCTM(context, CC_DEGREES_TO_RADIANS(180));
			CGContextTranslateCTM(context, -displaySize.width, -displaySize.height);
			break;
		case CCDeviceOrientationLandscapeLeft:
			CGContextRotateCTM(context, CC_DEGREES_TO_RADIANS(-90));
			CGContextTranslateCTM(context, -displaySize.height, 0);
			break;
			
		case CCDeviceOrientationLandscapeRight:
			CGContextRotateCTM(context, CC_DEGREES_TO_RADIANS(90));
			CGContextTranslateCTM(context, displaySize.height-displaySize.width, -displaySize.height);
			break;
	}
	
	CGContextDrawImage(context, CGRectMake(0.0f, 0.0f, displaySize.width, displaySize.height), iref);
	CGImageRef imageRef = CGBitmapContextCreateImage(context);
	UIImage *outputImage = [[[UIImage alloc] initWithCGImage:imageRef] autorelease];
	
	//Dealloc
	CGImageRelease(imageRef);
	CGDataProviderRelease(provider);
	CGImageRelease(iref);
	CGColorSpaceRelease(colorSpaceRef);
	CGContextRelease(context);
	free(buffer);
	free(pixels);
	
	return outputImage;
}

static UIImage* screenshotUIImage()
{
    return screenshotUIImageFromHeight(0);
}

void shareOnFaceBook()
{
    //take screenshot
    UIImage *image = screenshotUIImageFromHeight(20);

}

void shareOnTwitter()
{
    //take screenshot
    UIImage *image = screenshotUIImageFromHeight(20);
}
