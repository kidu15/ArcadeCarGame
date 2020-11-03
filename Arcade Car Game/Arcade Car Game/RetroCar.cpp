// Retro Arcade Car game, basic in C++ using olcConsoleGameEngine 
#include <iostream>
using namespace std; 

#include "olcConsoleGameEngine.h"

class ArcadeCar : public olcConsoleGameEngine
{
public:
	ArcadeCar()
	{
		m_sAppName = L"ArcadeCar";
	}
private:
	float fCarPos = 0.0f; 
	float fDistance = 0.0f;
	float fSpeed = 0.0f; 
	float fCurvature = 0.0f; 
	float fPlayerCurvature = 0.0f; 
	float fTrackCurvature = 0.0f; 

	vector<pair<float, float>> vectrack; // curvature, distance 

protected:
	virtual	bool OnUserCreate()
	{
		vectrack.push_back(make_pair(0.0f, 10.0f)); //0 curvature for 10 distance 
		vectrack.push_back(make_pair(0.0f, 200.0f)); //0 curvature for 200 distance 
		vectrack.push_back(make_pair(1.0f, 200.0f)); //1 curvature for 200 distance 
		vectrack.push_back(make_pair(0.0f, 400.0f)); //1 curvature for 200 distance 
		vectrack.push_back(make_pair(-1.0f, 200.0f)); //1 curvature for 200 distance 
		vectrack.push_back(make_pair(0.0f, 200.0f)); //1 curvature for 200 distance 

		return true; 
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		//controlling the speed and distance of the car 

		if (m_keys[VK_UP].bHeld)
			fSpeed += 2.0f * fElapsedTime;
		else
			fSpeed -= 1.0f * fElapsedTime; 

		if (m_keys[VK_LEFT].bHeld)
			fPlayerCurvature -= 0.7f * fElapsedTime; 

		if (m_keys[VK_RIGHT].bHeld)
			fPlayerCurvature += 0.7f * fElapsedTime; 

		if (fabs(fPlayerCurvature - fTrackCurvature) >= 0.8f)
			fSpeed -= 5.0f * fElapsedTime; 

		//controlling that the speed can't be -1
		if (fSpeed < 0.0f) fSpeed = 0.0f; 
		if (fSpeed > 2.0f) fSpeed = 2.0f;


		fDistance += (fSpeed * 70.0f) * fElapsedTime;
		// where is the car in the track? 

		float fOffset = 0; 
		int nTrackSection = 0; 

		//find position 

		while (nTrackSection < vectrack.size() && fOffset <= fDistance)
		{
			fOffset += vectrack[nTrackSection].second;
			nTrackSection++; 
		}

		float fTargetCurvature = vectrack[nTrackSection - 1].first; 
		float fTrackCurveDiff = (fTargetCurvature - fCurvature) * fElapsedTime * fSpeed; 
		fCurvature += fTrackCurveDiff; 

		fTrackCurvature += (fCurvature) * fElapsedTime * fSpeed; 

		//erase screen 
		Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ', 0);

		for (int y = 0; y < ScreenHeight() / 2; y++)
		{
			for (int x = 0; x < ScreenWidth(); x++)
			{
				// adding perspective 
				float fPerspective = (float)y / (ScreenHeight() / 2.0f); 

				float fMiddlePoint = 0.5f + fCurvature * powf((1.0f - fPerspective),3); 
				float fRoadWidth = 0.1f + fPerspective*0.8f; //60% of the screen will be the road
				float fClipWidth = fRoadWidth * 0.10f; //the sides of the road will be 15% of the road 

				// we assume the road is symmetrical and has a middle point 
				fRoadWidth *= 0.5f;

				// boundaries between grass, clip and road in the X axys 

				int nLeftGrass = (fMiddlePoint - fRoadWidth - fClipWidth) * ScreenWidth();
				int nLeftClip = (fMiddlePoint - fRoadWidth) * ScreenWidth(); 
				int nRightGrass = (fMiddlePoint + fRoadWidth + fClipWidth) * ScreenWidth();
				int nRightClip = (fMiddlePoint + fRoadWidth) * ScreenWidth();

				// for writing in the bottom side of the screen 
				int nRow = ScreenHeight() / 2 + y;

				//changing the grass color according to a function and the distance travelled to make the illusion of movement

				int nGrassColour = sinf(20.0f * powf(1.0f - fPerspective, 3) + fDistance * 0.1f) > 0.0f ? FG_GREEN : FG_DARK_GREEN; 

				//changing the clip color according to a function and the distance travelled to make the illusion of movement

				int nClipColour = sinf(80.0f * powf(1.0f - fPerspective, 2) + fDistance * 0.1f) > 0.0f ? FG_WHITE : FG_DARK_RED;

				if ((x >= 0) && (x < nLeftGrass))
				{
					Draw(x, nRow, PIXEL_SOLID, nGrassColour);
				}
				if ((x >= nLeftGrass) && (x < nLeftClip))
				{
					Draw(x, nRow, PIXEL_SOLID, nClipColour);
				}
				if ((x >= nLeftClip) && (x < nRightClip))
				{
					Draw(x, nRow, PIXEL_SOLID, FG_GREY);
				}
				if ((x >= nRightClip) && (x < nRightGrass))
				{
					Draw(x, nRow, PIXEL_SOLID, nClipColour);
				}
				if ((x >= nRightGrass) && (x < ScreenWidth()))
				{
					Draw(x, nRow, PIXEL_SOLID, nGrassColour);
				}
			}
		}

		//draw car  // -1 left, 0 center, 1 right 
		fCarPos = fPlayerCurvature - fTrackCurvature; 
		int nCarPos = ScreenWidth() / 2 + ((int)(ScreenWidth() * fCarPos) / 2.0f) - 7; 
		
		DrawStringAlpha(nCarPos, 80, L"   ||====||   ");
		DrawStringAlpha(nCarPos, 81, L"      ==      ");
		DrawStringAlpha(nCarPos, 82, L"     ====     ");
		DrawStringAlpha(nCarPos, 83, L"     ====     ");
		DrawStringAlpha(nCarPos, 84, L"|||  ====  |||");
		DrawStringAlpha(nCarPos, 85, L"|||  ====  |||");
		DrawStringAlpha(nCarPos, 86, L"|||========|||");


		return true; 
	}
};

int main()
{
	// Use olcConsoleGameEngine derived app
	ArcadeCar game;

	// Create a console with resolution 160x100 characters
	// Each character occupies 8x8 pixels
	game.ConstructConsole(160, 100, 8, 8);

	// Start the engine!
	game.Start();

	return 0;
}
