#include "Shapes.h"

void QueueRectangle(RECT& r, DWORD color)
{
	QueueTransformClipLine(r.left, r.top, r.right, r.top, color, color);
	QueueTransformClipLine(r.right, r.top, r.right, r.bottom, color, color);
	QueueTransformClipLine(r.right, r.bottom, r.left, r.bottom, color, color);
	QueueTransformClipLine(r.left, r.bottom, r.left, r.top, color, color);
}