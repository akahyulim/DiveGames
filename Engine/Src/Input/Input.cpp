#include "divepch.h"
#include "Input.h"

// 일단 static으로 event를 등록할 수 있나 확인해야 한다.
// 그리고 WndProc으로부터 event를 어떻게 전달할지 구현해야 한다.
// 스파르탄의 경우 msg별로 event를 만든 것이 아니고
// 매 프레임 WindowData라는 구조체를 채운 후 이벤트로 보낸다.
// 문제는 현재 이벤트를 사용하고 있지 않다는 것이다.
namespace Dive
{

}