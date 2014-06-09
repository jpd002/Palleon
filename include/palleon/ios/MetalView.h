#import <UIKit/UIKit.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

@interface MetalView : UIView
{
	
}

@property(nonatomic, retain) id<MTLDevice> device;
@property(nonatomic, retain) CAMetalLayer* renderLayer;

-(id)initWithFrame: (CGRect)frame;

@end
