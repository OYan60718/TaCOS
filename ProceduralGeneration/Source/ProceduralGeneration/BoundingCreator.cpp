// Generate bounding boxes for object detection
// Author: Chengyang Yan
// Date: March 2024

#include "BoundingCreator.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "ProceduralGenerationCharacter.h"

TArray<FVector> GetBoundingPoints(UStaticMeshComponent* Mesh, UWorld* TheWorld)
{
	FVector MinLocalBound;
	FVector MaxLocalBound;
	FVector WorldLocation;
	TArray<FVector> TransformedPoints;

	Mesh->GetLocalBounds(MinLocalBound, MaxLocalBound);

	FVector LocalBound = MaxLocalBound - MinLocalBound;
	LocalBound = LocalBound.GetAbs();

	WorldLocation = Mesh->GetComponentLocation();

	WorldLocation[2] = LocalBound[2] / 2 + WorldLocation[2];

	FVector BoxExtent = (LocalBound / 2) * Mesh->GetComponentScale();

	FTransform RotationTransform(Mesh->GetComponentRotation().Quaternion());

	TArray<FVector> VectorPermutations = GetVectorPermutations(BoxExtent);

	for (int32 i = 0; i < VectorPermutations.Num(); i++)
	{
		FVector TransformedVector = RotationTransform.TransformPosition(VectorPermutations[i]) + WorldLocation;
		TransformedPoints.Add(TransformedVector);
	}

	return TransformedPoints;
}

TArray<FVector> GetVectorPermutations(FVector ExtentBox)
{
	TArray<FVector> PermutationVector;
	FVector TempExtentBox;

	PermutationVector.Add(ExtentBox);

	TempExtentBox = ExtentBox;
	TempExtentBox[2] = ExtentBox[2] * -1;
	PermutationVector.Add(TempExtentBox);

	TempExtentBox = ExtentBox;
	TempExtentBox[1] = ExtentBox[1] * -1;
	PermutationVector.Add(TempExtentBox);

	TempExtentBox = ExtentBox;
	TempExtentBox[1] = ExtentBox[1] * -1;
	TempExtentBox[2] = ExtentBox[2] * -1;
	PermutationVector.Add(TempExtentBox);

	TempExtentBox = ExtentBox;
	TempExtentBox[0] = ExtentBox[0] * -1;
	TempExtentBox[1] = ExtentBox[1] * -1;
	PermutationVector.Add(TempExtentBox);

	TempExtentBox = ExtentBox;
	TempExtentBox[0] = ExtentBox[0] * -1;
	TempExtentBox[1] = ExtentBox[1] * -1;
	TempExtentBox[2] = ExtentBox[2] * -1;
	PermutationVector.Add(TempExtentBox);

	TempExtentBox = ExtentBox;
	TempExtentBox[0] = ExtentBox[0] * -1;
	PermutationVector.Add(TempExtentBox);

	TempExtentBox = ExtentBox;
	TempExtentBox[0] = ExtentBox[0] * -1;
	TempExtentBox[2] = ExtentBox[2] * -1;
	PermutationVector.Add(TempExtentBox);

	return PermutationVector;
}

bool WasComponentRenderedRecently(float tolerance, UStaticMeshComponent* Mesh)
{
	UWorld* World = Mesh->GetWorld();

	return (World) ? (World->GetTimeSeconds() - Mesh->GetLastRenderTimeOnScreen() <= tolerance) : false;
}

FString GetBoundingsOnScreen(FString ClassName, FString ClassLabel, UStaticMeshComponent* Mesh, UWorld* TheWorld, int ResX, int ResY)
{
	TArray<FVector> BoundingPointsWorld;

	BoundingPointsWorld = GetBoundingPoints(Mesh, TheWorld);

	TArray<float> xScreenLocations;
	TArray<float> yScreenLocations;
	FString BoxPositions;
	int RenderTargetRightX;
	int RenderTargetRightY;
	int RenderTargetLeftX;
	int RenderTargetLeftY;
	FVector2D Viewport = FVector2D(1, 1);

	for (int32 i = 0; i < BoundingPointsWorld.Num(); i++)
	{
		FVector2D ScreenPositions;
		const APlayerController* const PlayerController = Cast<const APlayerController>(UGameplayStatics::GetPlayerController(TheWorld, 0));
		PlayerController->ProjectWorldLocationToScreen(BoundingPointsWorld[i], ScreenPositions);

		xScreenLocations.Add(ScreenPositions[0]);
		yScreenLocations.Add(ScreenPositions[1]);
	}

	xScreenLocations.Sort(SortFloat);
	yScreenLocations.Sort(SortFloat);

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize( /*out*/Viewport);
	}

	RenderTargetLeftX = int(xScreenLocations[0] * ResX / Viewport[0]);
	RenderTargetLeftY = int(yScreenLocations[0] * ResY / Viewport[1]);
	RenderTargetRightX = int(xScreenLocations[7] * ResX / Viewport[0]);
	RenderTargetRightY = int(yScreenLocations[7] * ResY / Viewport[1]);

	if (RenderTargetLeftX < 0 || RenderTargetLeftX >= RenderTargetRightX || RenderTargetRightX > ResX)
	{
		BoxPositions = "";
	}
	else if (RenderTargetLeftY < 0 || RenderTargetLeftY >= RenderTargetRightY || RenderTargetRightY > ResY)
	{
		BoxPositions = "";
	}
	else
	{	
		BoxPositions += ClassName;
		BoxPositions += ",";
		BoxPositions += ClassLabel;
		BoxPositions += ",";
		BoxPositions += FString::FromInt(RenderTargetLeftX);
		BoxPositions += ",";
		BoxPositions += FString::FromInt(RenderTargetLeftY);
		BoxPositions += ",";
		BoxPositions += FString::FromInt(RenderTargetRightX);
		BoxPositions += ",";
		BoxPositions += FString::FromInt(RenderTargetRightY);
		BoxPositions += ",";
	}

	return BoxPositions;
}

inline bool SortFloat(float A, float B)
{
	return (A < B);
}