#include "athena/iphone/IosAudioClip.h"

using namespace Athena;

CIosAudioClip::CIosAudioClip(const char* path)
: m_player(nil)
{
	NSString* audioFilePath = [[NSString alloc] initWithUTF8String: path];
	NSURL* audioFileUrl = [NSURL fileURLWithPath: audioFilePath];
	[audioFilePath release];
	NSError* error = nil;
	m_player = [[AVAudioPlayer alloc] initWithContentsOfURL: audioFileUrl error: &error];
	if(error != nil)
	{
		NSLog(@"Failed to create audio clip '%s'.", path);
		[m_player release];
		m_player = nil;
	}
}

CIosAudioClip::~CIosAudioClip()
{
	if(m_player)
	{
		[m_player release];
	}
}

void CIosAudioClip::Play(bool loop)
{
	if(m_player == nil) return;
	m_player.numberOfLoops = loop ? -1 : 0;
	[m_player play];
}

void CIosAudioClip::Stop()
{
	if(m_player == nil) return;
	[m_player stop];
}
