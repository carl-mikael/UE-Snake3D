// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayArea.h"

#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
APlayArea::APlayArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(SceneComponent);

	FloorMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("FloorMesh");
	FloorMesh->SetupAttachment(SceneComponent);
	
	WallMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("WallMesh");
	WallMesh->SetupAttachment(SceneComponent);
	
	GridSize = 10;
	TileSize = 100;
	TileScale = 0.1f;
	TileZOffset = -50.f;
	WallScale = 1.5f;
}

// Called when the game starts or when spawned
void APlayArea::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnTiles();
}

// Called every frame
void APlayArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayArea::SpawnTiles() const
{
	if (!IsValid(FloorMesh))
	{
		UE_LOG(LogTemp, Error, TEXT("Floor mesh is invalid"));
		return;
	}
	
	FVector BaseWorldPos = SceneComponent->GetComponentLocation();
	for (int x = 0; x < GridSize; x++)
	{
		for (int y = 0; y < GridSize; y++)
		{
			const bool bIsHorizontalWall = (x == 0 || x == GridSize-1);
			const bool bIsVerticalWall = (y == 0 || y == GridSize-1);
			if (bIsHorizontalWall || bIsVerticalWall)
			{
				const FVector WallPosition = BaseWorldPos + FVector(x * TileSize, y * TileSize, 0.f);
				FTransform WallTransform = FTransform(WallPosition);
				const FVector WallScaleVector = FVector(WallScale, WallScale, WallScale);
				WallTransform.SetScale3D(WallScaleVector);
				// if (bIsVerticalWall)
				// {
				// 	WallTransform.SetRotation(FQuat(FRotator(0.f, 90.f, 0.f)));
				// }
				WallMesh->AddInstance(WallTransform, true);
			}
			else
			{
				const FVector TilePosition = BaseWorldPos + FVector(x * TileSize, y * TileSize, TileZOffset);
				FTransform TileTransform = FTransform(TilePosition);
				const FVector TileScaleVector = FVector(TileScale, TileScale, TileScale);
				TileTransform.SetScale3D(TileScaleVector);
				FloorMesh->AddInstance(TileTransform, true);
			}
		}
	}
}