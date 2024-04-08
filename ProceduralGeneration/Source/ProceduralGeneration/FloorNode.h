// Set each room as a node
// Modified with code from Druid mechanics at https://www.youtube.com/watch?v=dXK-bLbmwAo
// Author: Chengyang Yan
// Date: March 2024

#pragma once

struct FCornerCoordinates
{
	float UpperLeftX;
	float UpperLeftY;
	float LowerRightX;
	float LowerRightY;
};

class FloorNode
{
public:
	FloorNode();
	FloorNode(const FCornerCoordinates& Coordinates);
	~FloorNode();

	FORCEINLINE FCornerCoordinates GetCornerCoordinates() const { return CornerCoordinates;  }
	FORCEINLINE void SetCornerCoordinates(FCornerCoordinates Coordinates) { CornerCoordinates = Coordinates;  }

	FORCEINLINE static int32 GetNodeCount() { return FloorNodeCount; }

private:
	FCornerCoordinates CornerCoordinates;

	static int32 FloorNodeCount;
};