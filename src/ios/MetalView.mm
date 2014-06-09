#import "palleon/ios/MetalView.h"

@implementation MetalView

@synthesize device;
@synthesize renderLayer;

-(id)initWithFrame: (CGRect)frame
{
	self = [super initWithFrame: frame];
	if(self)
	{
		self.device = MTLCreateSystemDefaultDevice();
				
		self.renderLayer = [CAMetalLayer layer];
		self.renderLayer.device = self.device;
		self.renderLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
		self.renderLayer.framebufferOnly = YES;
		
		[self.renderLayer setFrame: self.layer.frame];
		[self.layer addSublayer: self.renderLayer];
		
		self.opaque = YES;
		self.backgroundColor = nil;
		self.contentScaleFactor = [UIScreen mainScreen].scale;
	}
	return self;
}

-(void)dealloc
{
	[super dealloc];
}

-(void)layoutSubviews
{
	[super layoutSubviews];
	[self.renderLayer setFrame: self.layer.frame];
	
	CGSize drawableSize = self.bounds.size;
	drawableSize.width *= self.contentScaleFactor;
	drawableSize.height *= self.contentScaleFactor;
	self.renderLayer.drawableSize = drawableSize;
}

@end
