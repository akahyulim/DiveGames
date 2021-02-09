#pragma once


// freeImage를 이용한다.
// load 함수가 메인이다.
// file path와 texture 객체를 전달받으면
// file을 임포트한 후 texture에 전달한다.
// 옵션에 따라 mipmap도 생성한다.

// 엔진은 native format만 지원한다.
// 즉, 임포트한 데이터를 texture format으로 serialze, desireialize 해야 한다.

using namespace Dive;

class ImageImporter
{
public:

	bool Load(const std::string& filepath, Texture* texture, bool generateMipmaps = true);

private:
private:
};