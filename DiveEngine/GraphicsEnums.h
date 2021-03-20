#pragma once

enum class eDepthStencilStates : uint32_t
{
	Default = 0,
	Shaodw,
	WriteOnly,
	Count
};

enum DSSTATE
{
	DSSTATE_DEFAULT,
	DSSTATE_SHADOW,
	DSSTATE_WRITE_ONLY,
	DSSTATE_COUNT
};