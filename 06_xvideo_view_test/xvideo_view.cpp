#include "xvideo_view.h"
#include "xsdl.h"
XVideoView* XVideoView::Creat(RenderType type = SDL)
{
	switch (type)
	{
	case XVideoView::SDL:
		return new XSDL();
		break;
	default:
		break;
	}
	return nullptr;
}
