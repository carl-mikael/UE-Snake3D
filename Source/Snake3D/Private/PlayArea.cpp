// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayArea.h"

#include "Food.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
APlayArea::APlayArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(SceneComponent);

	FloorMeshInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>("FloorMesh");
	FloorMeshInstances->SetupAttachment(SceneComponent);
	
	WallMeshInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>("WallMesh");
	WallMeshInstances->SetupAttachment(SceneComponent);
	
	FoodChildActorComponent = CreateDefaultSubobject<UChildActorComponent>("FoodChildActorComponent");
	FoodChildActorComponent->SetupAttachment(SceneComponent);
	
	GridSize = 10;
	TileSize = 100;
	TileScale = 0.1f;
	TileZOffset = -50.f;
	WallScale = 1.5f;
}

void APlayArea::OnFoodDestroyed(AActor* Food)
{
	SpawnFood();
}

// Called when the game starts or when spawned
void APlayArea::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnTiles();
	SpawnFood();
}

// Called every frame
void APlayArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayArea::SpawnTiles() const
{
	if (!IsValid(FloorMeshInstances))
	{
		UE_LOG(LogTemp, Error, TEXT("Floor mesh is invalid"));
		return;
	}

	const FVector BaseWorldPos = SceneComponent->GetComponentLocation();
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
				WallMeshInstances->AddInstance(WallTransform, true);
			}
			else
			{
				const FVector TilePosition = BaseWorldPos + FVector(x * TileSize, y * TileSize, TileZOffset);
				FTransform TileTransform = FTransform(TilePosition);
				const FVector TileScaleVector = FVector(TileScale, TileScale, TileScale);
				TileTransform.SetScale3D(TileScaleVector);
				FloorMeshInstances->AddInstance(TileTransform, true);
			}
		}
	}
}

FVector APlayArea::GetRandomFloorLocation() const
{
	FTransform RandomFloorTransform;
	while (!FloorMeshInstances->GetInstanceTransform(
		FMath::RandRange(0, FloorMeshInstances->GetInstanceCount() - 1),
		RandomFloorTransform, true))
	{
	}
	
	return RandomFloorTransform.GetLocation();
}

void APlayArea::SpawnFood() const
{
	FVector RandomFloorLocation = GetRandomFloorLocation();
	FoodChildActorComponent->CreateChildActor();
	AActor* ChildActor = FoodChildActorComponent->GetChildActor();
	// Without this it crashes on player death, idk why
	if (!IsValid(ChildActor))
	{
		return;
	}
	
	RandomFloorLocation.Z += TileZOffset * -1;
	ChildActor->SetActorTransform(FTransform(RandomFloorLocation));
	AFood* FoodActor = Cast<AFood>(ChildActor);
	if (FoodActor)
	{
		FoodActor->OnDestroyed.AddDynamic(this, &APlayArea::OnFoodDestroyed);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayArea::SpawnFood() - FoodActor is null, ChildActorComponent prob has wrong class"));
	}
}
