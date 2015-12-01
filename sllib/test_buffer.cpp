//#include "slbuffer.h"
//#include <stdio.h>
//int main(int argc, char** argv)
//{
//	sl::CBuffer _buffer;
//	_buffer.Attach(NULL, 4, 0);
//	int a = 4;
//	
//	if(_buffer.AppendData(a) == -1)
//		printf(" buffer1 is full\n");
//	if( _buffer.AppendChar('g') == -1)
//		printf(" buffer2 is full\n");
//	char data[] = "dfafda";
//	if( _buffer.Append(data, strlen(data) ) == -1 )
//		printf(" buffer3 is full\n");
//	printf("usedlen: %d\n", _buffer.GetUsedLen());
//
//}